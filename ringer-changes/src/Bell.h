/*
 * Bell.h
 *
 *  Created on: 11 Jul 2012
 *      Author: tkcook
 */

#ifndef BELL_H_
#define BELL_H_

#include <opencv2/opencv.hpp>
#include <sigc++/sigc++.h>
#include <memory>

enum BellCommand : unsigned char;
enum BellStroke : unsigned char;

// Note - do not rely on the copy constructor of this class to create
// multiple bells.  The signals presented by this bell share slot tables
// with all copies, so if you use the copy constructor then you end up
// with bells with all inter-mixed signals.
// Note that this also means that this:
// std::vector<Bell> bells(6, Bell(...));
// will do some very strange things.
class Bell {
public:
	typedef cv::Matx<double, 4, 1> StateVector;
	typedef cv::Matx<double, 4, 4> TransitionMatrix;

	Bell(double bellLength, double clapperLength, double axisSep);
	Bell(const Bell& other);
	Bell& operator=(const Bell& other);

	virtual ~Bell();

	virtual double BellPosition() const;
	virtual double ClapperPosition() const;
	virtual void Update(double dt, double now);

	void OverrideBell(double pos, double velocity);
	void OverrideClapper(double pos, double velocity);

	double RopePosition();
	double RopeVelocity();

	void QueueStrike(double time, double targetFraction);
	unsigned QueueLength();

	void Command(BellCommand cmd);
	BellCommand CurrentCommand();
	double HowFarUp();
	double TimeOfLastStrike();
	double PeriodOfLastStroke();
	double ErrorOnLastStroke();
	double EnergyFraction();
	double ExcessEnergyFraction();

	sigc::signal<void, BellStroke> Sounded;
	sigc::signal<void, double, double> Moved;
	sigc::signal<void, double> Indicate;

protected:
	struct Impl;
	std::auto_ptr<Impl> imp;

	StateVector SSFun(const StateVector& x, double inputTorque);
	StateVector Estimate(const StateVector& x, double inputTorque, double dt, BellStroke& strike);
	// Estimate the time until the bell strikes stroke 'stroke'.
	// If the bell will not strike within the next 5 seconds then returns -1.
	double EstimateTimeToStrike(StateVector x, BellStroke s);

	double ControlLeadUp(double ropePos, double ropeVel, double estError);
	double ControlRingUp(double ropePos, double ropeVel, double estError);
	double ControlLead(double ropePos, double ropeVel, double estError);
	double ControlRing(double ropePos, double ropeVel, double estError);

	void NextTarget();

};

#endif /* BELL_H_ */
