/*
 * Bell.cpp
 *
 *  Created on: 11 Jul 2012
 *      Author: tkcook
 */

#include "Bell.h"
#include "Enums.h"
#include <cstdint>
#include <queue>

// The angle of rotation to hit the stay
static const double STAY_POSITION = 182.0 * M_PI / 180.0;
// The maximum angle of the clapper from the central axis of the bell.
// Make sure this is less than or equal to the maximum possible excursion
// of your bell mesh, or you will see the clapper go through the side of the bell.
static const double CLAPPER_LIMIT = M_PI/8.0;
// The angle of rotation of the garter hole from the central axis of the bell.
static const double GARTER_HOLE_THETA = -5.0*M_PI/8.0;
// The gain applied to striking errors
static const double K_STRIKE = 1000;

// Utility - return the sign of the input.
static inline int sign(double x)
{
	if(x >= 0)
		return 1;
	else
		return -1;
}

// Utility - return the input value clipped to limits.
static inline double limit(double x, double upper, double lower)
{
	if(x > upper)
		return upper;
	if(x < lower)
		return lower;
	return x;
}

// Acceleration due to gravity
static const double g = 9.81;

// The equivalent length of a small-amplitude pendulum given its period of swing
static inline double lengthFromPeriod(double period)
{
	return g * pow(period, 2) / (4 * pow(M_PI, 2));
}

struct Bell::Impl {
	Impl(double bellLength, double clapperLength, double axisSep) :
		m_State({ 0, 0, 0, 0 }),
		A(4, 4, CV_64F),
		m_BellLength(lengthFromPeriod(bellLength)),
		m_ClapperLength(lengthFromPeriod(clapperLength)),
		m_axisSep(axisSep),
		m_BellMass(600),
		m_BellOverride(false),
		m_ClapperOverride(false),
		m_LastChime(BellStroke::NO_STROKE),
		m_CurrentStroke(BellStroke::HAND_STROKE),
		m_NextTarget(0),
		m_LastTarget(0),
		m_HowFarUp(0),
		m_TimeOfLastStrike(0),
		m_PeriodOfLastStroke(0),
		m_ErrorOnLastStroke(0),
		m_EnergyFraction(0),
		m_TargetEnergyFraction(0.2),
		m_CurrentCommand(BellCommand::IDLE)
	{
		A = cv::Matx<double, 4, 4>(0, 0, 0, 0,
									0, 0, 0, 0,
									0, 0, 0, 0,
									0, 0, 0, 0);
		A(0, 1) = 1.0;
		A(2, 3) = 1.0;
	}
	// A state vector - values are [ theta  theta'  phi  phi' ]
	// where theta is the bell angle and phi is the clapper angle
	StateVector m_State;
	TransitionMatrix A;

	double m_BellLength;
	double m_ClapperLength;
	double m_axisSep;
	double m_BellMass;

	bool m_BellOverride;
	bool m_ClapperOverride;

	BellStroke m_LastChime;
	BellStroke m_CurrentStroke;
	double m_NextTarget;
	double m_LastTarget;
	std::queue<double> m_TargetQueue;

	double m_HowFarUp;
	double m_TimeOfLastStrike;
	double m_PeriodOfLastStroke;
	double m_ErrorOnLastStroke;
	double m_EnergyFraction;
	double m_TargetEnergyFraction;
	double m_ExcessEnergyFraction;

	BellCommand m_CurrentCommand;
};

Bell::Bell(double bellLength, double clapperLength, double axisSep) :
		imp(new Bell::Impl(bellLength, clapperLength, axisSep))
{
}

Bell::Bell(const Bell& other) : imp(new Bell::Impl(*(other.imp)))
{
}

Bell& Bell::operator =(const Bell& other)
{
	imp.reset(new Bell::Impl(*(other.imp)));
	return *this;
}

Bell::~Bell()
{
}

// The physical simulation of the bell swing.
// Note that a lot of the control is trial-and-error tuning for now.
// It may get a more theoretical treatment later.
//
// Some of the control code is quite confusing unless you really understand what is
// going on.  The most important thing to understand is that the sounding of a stroke
// happens quite a long time after the ringer applies force to the rope for that
// stroke.  In fact, the clapper hits the bell to sound the hand stroke while the
// ringer is already pulling the back stroke.  So by the time the hand stroke sounds,
// we have to already be well into thinking about how hard to pull in order to get
// the back stroke at the right place and vice versa.
void Bell::Update(double dt, double now)
{
	// Extract some numbers for future reference
	double theta = imp->m_State(0, 0);
	double theta_dot = imp->m_State(1, 0);

	// This is the torque applied through the rope.
	double torque = 0;

	// energySurplus is the difference between the current kinetic energy of the bell
	// and the potential energy that needs to be added to reach the top of the swing.
	// Then calculate a torque that should cause the energy surplus to hit 0 at the
	// top.
	double kineticEnergy = 0.5*imp->m_BellMass*pow(imp->m_BellLength, 2)*pow(theta_dot, 2);
	double potentialEnergy = imp->m_BellMass * g * imp->m_BellLength * (1 - cos(theta));
	double totalEnergy = kineticEnergy + potentialEnergy;
	double energyAtTop = 2 * imp->m_BellMass * imp->m_BellLength * g;
	double energySurplus = totalEnergy - energyAtTop;
	imp->m_EnergyFraction = totalEnergy / energyAtTop;
	imp->m_ExcessEnergyFraction = imp->m_EnergyFraction - imp->m_TargetEnergyFraction;

	if(imp->m_CurrentCommand == BellCommand::RING_UP ||
			imp->m_CurrentCommand == BellCommand::LEAD_UP)
	{
		imp->m_CurrentStroke = (RopePosition() > 0 ?
				BellStroke::HAND_STROKE :
				BellStroke::BACK_STROKE);
		NextTarget();
	}
	else if(imp->m_CurrentCommand == BellCommand::RING)
	{
		if(imp->m_CurrentStroke == BellStroke::HAND_STROKE && RopePosition() < 0)
		{
			imp->m_CurrentStroke = BellStroke::BACK_STROKE;
			NextTarget();
		}
		else if(imp->m_CurrentStroke == BellStroke::BACK_STROKE && RopePosition() > 0)
		{
			imp->m_CurrentStroke = BellStroke::HAND_STROKE;
			NextTarget();
		}
	}
	double estimatedStrikeOffset = EstimateTimeToStrike(imp->m_State, imp->m_CurrentStroke);
	double estimateStrike = now + estimatedStrikeOffset;
	double strikingError = estimateStrike - imp->m_NextTarget;

	torque = 0;
	switch(imp->m_CurrentCommand)
	{
	case BellCommand::IDLE:
		torque = 0;
		break;
	case BellCommand::LEAD_UP:
		torque = ControlLeadUp(RopePosition(), RopeVelocity(), strikingError);
		break;
	case BellCommand::RING_UP:
		torque = ControlRingUp(RopePosition(), RopeVelocity(), strikingError);
		break;
	case BellCommand::RING_DOWN:
		// Pull on the up part of the back-stroke
		if(RopeVelocity() > 0 && RopePosition() < 0)
			torque = -300 * sign(theta_dot);
		if(totalEnergy / energyAtTop < 0.03)
			imp->m_CurrentCommand = BellCommand::IDLE;
		break;
	case BellCommand::RING:
		torque = ControlRing(RopePosition(), RopeVelocity(), strikingError);
		break;
	case BellCommand::LEAD:
		torque = ControlLead(RopePosition(), RopeVelocity(), strikingError);
		break;
	case BellCommand::HOLD_UP:
		break;
	case BellCommand::CUT_IN:
		break;
	case BellCommand::STAND:
		if(RopeVelocity() > 0)
		{
			if(RopePosition() < 2*GARTER_HOLE_THETA && energySurplus > -1)
				torque = energySurplus * -sign(theta) * 3;
			if(RopePosition() > 0 && energySurplus > 5)
				torque = energySurplus * -sign(theta) * 3;
		}
		if(RopeVelocity() < 0 && energySurplus < 50)
			torque = -200*sign(theta);
		Indicate(fabs(imp->m_ExcessEnergyFraction));
		break;
	}

	//indication = (RopeVelocity() > 0 ? 1 : 0);

	// A fourth-order Runge-Kutta approximation to the integral
	BellStroke strike = BellStroke::NO_STROKE;
	StateVector nx = Estimate(imp->m_State, torque, dt, strike);
	if(strike != BellStroke::NO_STROKE && strike != imp->m_LastChime)
	{
		if(imp->m_LastChime != BellStroke::NO_STROKE && imp->m_LastChime != strike)
		{
			imp->m_PeriodOfLastStroke = now - imp->m_TimeOfLastStrike;
		}
		imp->m_TimeOfLastStrike = now;
		imp->m_ErrorOnLastStroke = now - imp->m_LastTarget;
		imp->m_LastChime = strike;
		Sounded(strike);
	}

	// Allow overrides of bell and clapper position.
	// The only time you'd want the clapper is when you are setting initial conditions
	// but you should be able to measure the position of a real bell, feed it in here
	// and simulate a clapper accurately.
	if(!imp->m_BellOverride)	{
		imp->m_State(0,0) = nx(0,0);
		imp->m_State(1,0) = nx(1,0);
	}
	if(!imp->m_ClapperOverride)
	{
		imp->m_State(2,0) = nx(2,0);
		imp->m_State(3,0) = nx(3,0);
	}

	imp->m_BellOverride = imp->m_ClapperOverride = false;


	if(fabs(imp->m_State(0,0)) > STAY_POSITION)
	{
		imp->m_State(0, 0) = STAY_POSITION * sign(imp->m_State(0,0));
		imp->m_State(1,0) = 0;
	}

	Moved.emit(imp->m_State(0,0), imp->m_State(2,0));
}

void Bell::OverrideBell(double pos, double velocity)
{
	imp->m_State(0,0) = pos;
	imp->m_State(1,0) = velocity;
	imp->m_BellOverride = true;
}

void Bell::OverrideClapper(double pos, double velocity)
{
	imp->m_State(2,0) = pos;
	imp->m_State(3,0) = velocity;
	imp->m_ClapperOverride = true;
}

// This is how far the rope has risen.  Zero is the maximum downwards excursion of the rope.
// Positive numbers indicate the hand-stroke side of this point, negative numbers the back-stroke.
double Bell::RopePosition()
{
	return imp->m_State(0,0) + GARTER_HOLE_THETA;
}

// The velocity at which the rope is rising.  At present this assumes that the rope
// contacts the wheel right around to where it is in line with the ringer, and so there is a
// sharp discontinuity in the velocity as the hole in the wheel passes the bottom of the turn.
// In future some separation should be added so that a smoother transition is acheived.
//
// Note carefully that this is *not* the 1st derivative of RopePosition() - this velocity is
// only whether the rope is rising or falling, regardless of which stroke.
double Bell::RopeVelocity()
{
	if(RopePosition() > 0)
		return imp->m_State(1,0);
	else
		return -imp->m_State(1,0);
}

Bell::StateVector Bell::SSFun(const Bell::StateVector& x, double inputTorque)
{
	double theta = x(0,0);
	double theta_dot = x(1,0);
	double phi = x(2,0);
	//double phi_dot = x(3,0);
	double theta_double_dot = - g * sin(theta) / imp->m_BellLength + inputTorque/imp->m_BellMass;

	StateVector xd(
	{
			0,
			theta_double_dot, // - 0.01*theta_dot,
			0,
			- theta_double_dot * (1 + imp->m_axisSep * cos(phi) / imp->m_ClapperLength)
			- g * sin(theta + phi) / imp->m_ClapperLength
			- imp->m_axisSep * sin(phi) * pow(theta_dot, 2) / imp->m_ClapperLength
	});

	StateVector integrals = imp->A*x;
	xd = integrals + xd;
	return xd;
}

Bell::StateVector Bell::Estimate(const StateVector& x, double inputTorque, double dt, BellStroke& strike)
{
	StateVector k1 = dt*SSFun(x, inputTorque);
	StateVector k2 = dt*SSFun(StateVector(x + 0.5*k1), inputTorque);
	StateVector k3 = dt*SSFun(x + 0.5*k2, inputTorque);
	StateVector k4 = dt*SSFun(x + k3, inputTorque);
	StateVector est = x + (1.0/6.0) * (k1 + 2*k2 + 2*k3 + k4);

	strike = BellStroke::NO_STROKE;

	if(est(2,0) >= CLAPPER_LIMIT)
	{
		est(2,0) = CLAPPER_LIMIT;
		est(3,0) = 0;
		if(x(2,0) < CLAPPER_LIMIT)
			strike = BellStroke::BACK_STROKE;
	}
	if(est(2,0) <= -CLAPPER_LIMIT)
	{
		est(2,0) = -CLAPPER_LIMIT;
		est(3,0) = 0;
		if(x(2,0) > -CLAPPER_LIMIT)
			strike = BellStroke::HAND_STROKE;
	}
	return est;
}

double Bell::EstimateTimeToStrike(StateVector x, BellStroke s)
{
	// If we are at rest at the bottom then it will never strike...
	if(x(0,0) == 0 && x(1,0) == 0)
		return -1;

	double tts = 0;
	BellStroke strike = BellStroke::NO_STROKE;
	double time_between_strikes = 0;

	while(strike != s)
	{
		x = Estimate(x, 0, 0.05, strike);
		tts += 0.05;

		time_between_strikes += (strike == BellStroke::NO_STROKE ? 0.05 : 0);

		if(time_between_strikes >= 10)
			return -1;
	}

	return tts;
}

double Bell::ControlLeadUp(double ropePos, double ropeVel,
		double estError)
{
	double torque = 0;
	if(ropeVel <= 0 && ropePos < 0)
	{
		if(imp->m_PeriodOfLastStroke <= 0)
			// Get the thing chiming
			torque = 400;
		else
			torque = 90;
	}
	Indicate(torque / 400);
	if(imp->m_EnergyFraction >= 1)
		imp->m_CurrentCommand = BellCommand::LEAD;
	return torque;
}

double Bell::ControlRingUp(double ropePos, double ropeVel,
		double estError)
{
	double torque = 0;
	// Pull on the down part of the back-stroke
	if(ropeVel <= 0 && ropePos < 0)
	{
		if(imp->m_PeriodOfLastStroke <= 0)
			torque = 400 * -sign(ropePos);
		else
		{
			torque = 90 - 2500 * imp->m_ExcessEnergyFraction;
			if(estError < 0)
				torque -= (1500 * estError);
			if(torque < 0)
				torque = 0;
		}
	}
	if(RopeVelocity() > 0 && RopePosition() < 0)
	{
		if(imp->m_ExcessEnergyFraction > 0.15 && estError > 0)
			torque = 30 * estError;
	}
	if(imp->m_EnergyFraction > 1)
		imp->m_CurrentCommand = RING;
	Indicate.emit(torque / 400.0);
	return torque;
}

double Bell::ControlLead(double ropePos, double ropeVel,
		double estError)
{
	double torque = 0;
	double excessEnergyFraction = imp->m_EnergyFraction - imp->m_TargetEnergyFraction;
	double bellPos = BellPosition();
	// If we're stuck on the stay, give it a big yank to get it off
	if(fabs(BellPosition()) >= M_PI && excessEnergyFraction < 0.0005)
	{
		torque = 500 * -sign(ropePos);
	}
	// If we're a long way down, ring back up.
	else if(imp->m_ExcessEnergyFraction < -0.1)
	{
		if(ropeVel < 0 && ropePos < 0)
			torque = 400;
	}
	// Pull as the bell approaches top, on either stroke
	else if(ropeVel >= 0 && fabs(bellPos) > fabs(GARTER_HOLE_THETA))
	{
		if(estError > 0)
			torque = estError * K_STRIKE;
		if(excessEnergyFraction > 0)
			torque = std::max(excessEnergyFraction * 3 * 10000, torque);
		torque *= -sign(ropePos);
	}
	// Back stroke pull
	else if(ropeVel <= 0 && ropePos < 2*GARTER_HOLE_THETA)
	{
		if(excessEnergyFraction < 0)
			torque = excessEnergyFraction * -3 * 10000;
//		if(estError > 0)
//			torque = std::max(estError * K_STRIKE, torque);
	}
	// Hand stroke pull
	else if(ropeVel <= 0 && ropePos > 0)
	{
		if(excessEnergyFraction < 0)
			torque = excessEnergyFraction * 3 * 10000;
//		if(estError > 0)
//			torque = std::min(estError * -K_STRIKE, torque);
	}
	Indicate(torque / 600);
	return torque;
}

double Bell::ControlRing(double ropePos, double ropeVel,
		double estError)
{
	return ControlLead(ropePos, ropeVel, estError);
}

double Bell::BellPosition() const
{
	return imp->m_State(0,0);
}

double Bell::ClapperPosition() const
{
	return imp->m_State(2,0);
}

void Bell::QueueStrike(double time, double targetFraction)
{
	imp->m_TargetQueue.push(time);
	imp->m_TargetEnergyFraction = targetFraction;
}

unsigned Bell::QueueLength()
{
	return imp->m_TargetQueue.size();
}

void Bell::Command(BellCommand cmd)
{
	imp->m_CurrentCommand = cmd;
	NextTarget();
	std::cout << "Command " << cmd << std::endl;
}

BellCommand Bell::CurrentCommand()
{
	return imp->m_CurrentCommand;
}

double Bell::HowFarUp()
{
	return imp->m_HowFarUp;
}

double Bell::TimeOfLastStrike()
{
	return imp->m_TimeOfLastStrike;
}

double Bell::PeriodOfLastStroke()
{
	return imp->m_PeriodOfLastStroke;
}

double Bell::ErrorOnLastStroke()
{
	return imp->m_ErrorOnLastStroke;
}

double Bell::EnergyFraction()
{
	return imp->m_EnergyFraction;
}

double Bell::ExcessEnergyFraction()
{
	return imp->m_ExcessEnergyFraction;
}

void Bell::NextTarget()
{
	if(!imp->m_TargetQueue.empty())
	{
		imp->m_LastTarget = imp->m_NextTarget;
		imp->m_NextTarget = imp->m_TargetQueue.front();
		imp->m_TargetQueue.pop();
	}
}
