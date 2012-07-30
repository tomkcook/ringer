/*
 * Controller.cpp
 *
 *  Created on: 18 Jul 2012
 *      Author: tkcook
 */

#include "Controller.h"

#include "Bell.h"
#include "Method.h"
#include "Enums.h"
#include <queue>

#include <sys/timeb.h>

static double WallTime()
{
	struct timeb t;
	ftime(&t);
	return t.time + t.millitm/1000.0;
}

class Command
{
public:
	Command(double t) : m_Time(t) { }
	virtual ~Command() { }

	virtual void operator()() const {}

	double TargetTime() const { return m_Time; }
protected:
	double m_Time;
};

class IssueBellCommand : public Command
{
public:
	IssueBellCommand(double t, Bell& bell, BellCommand cmd) :
		Command(t),
		m_Bell(bell),
		m_Cmd(cmd)
	{}

	void operator()() const
	{
		m_Bell.Command(m_Cmd);
	}

protected:
	Bell& m_Bell;
	BellCommand m_Cmd;
};

bool CompareCommands::operator ()(Command*& a, Command*& b)
{
	return a->TargetTime() > b->TargetTime();
}

struct Controller::Impl
{
	Impl(double startTime, int bellCount) :
		m_StartTime(startTime),
		m_LastLeadStrikeTime(0),
		m_Slow(false),
		m_Method(0),
		m_CurrentChange(bellCount),
		m_NextChange(2, Change(6))
	{
		for(int i = 0; i < bellCount; i++)
		{
			m_Bells.push_back(Bell(20/10, 20/9.0, 0.05));
		}
	}

	~Impl()
	{
	}

	Bell& operator[](int index) {
		return m_Bells.at(index);
	}

	std::vector<Bell> m_Bells;
	double m_Time;
	CommandQueue m_Commands;
	double m_StartTime;
	double m_LastLeadStrikeTime;
	double m_LastLeadEnergyFraction;

	bool m_Slow;
	std::auto_ptr<Method> m_Method;
	Change m_CurrentChange;
	std::vector<Change> m_NextChange;

	enum ControllerState : unsigned char {
		IDLE,
		RING_UP,
		RING,
		RING_DOWN
	} m_State;
};

Controller::Controller(int bellCount) :
		pimpl(new Controller::Impl(WallTime(), bellCount))
{
	for(int i = 0; i < bellCount; i++)
	{
		(*pimpl)[i].Sounded.connect(sigc::bind(sigc::mem_fun(this, &Controller::Sounded), i));
	}
}

Controller::~Controller()
{
	SetMethod(0);
}

Bell& Controller::operator [](int i)
{
	return (*pimpl)[i];
}

int Controller::size()
{
	return pimpl->m_Bells.size();
}

void Controller::GoSlow()
{
	pimpl->m_Slow = !pimpl->m_Slow;
}

void Controller::SetMethod(Method *m) {
	pimpl->m_Method.reset(m);
}

bool Controller::Update()
{
	double now = SimTime();
	double dt = now - pimpl->m_Time;
	pimpl->m_Time = now;

	if(!pimpl->m_Commands.empty())
	{
		const Command* top = pimpl->m_Commands.top();
		if(top->TargetTime() < now)
		{
			(*top)();
			pimpl->m_Commands.pop();
			delete(top);
		}
	}

	std::vector<Bell>::iterator end = pimpl->m_Bells.end();
	for(std::vector<Bell>::iterator it = pimpl->m_Bells.begin(); it != end; it++)
	{
		(*it).Update(dt, now);
	}

	return true;
}

double Controller::SimTime() const
{
	return WallTime() - pimpl->m_StartTime;
}

void Controller::RingUp()
{
	pimpl->m_Method.reset(new CalledMethod(6));
	(*pimpl)[0].Command(BellCommand::LEAD_UP);
	for(unsigned i = 1; i < pimpl->m_Bells.size(); i++)
	{
		pimpl->m_Commands.push(
				new IssueBellCommand(SimTime() + 0.5*double(i) / 6.0, (*pimpl)[i], BellCommand::RING_UP));
	}
}

void Controller::Ring()
{
	double startTime = SimTime();
	pimpl->m_State = Impl::ControllerState::RING;
	SetMethod(new CalledMethod(pimpl->m_Bells.size()));
//	pimpl->m_CurrentChange = pimpl->m_Method->Next();
	pimpl->m_NextChange[0] = pimpl->m_Method->Next();
	pimpl->m_NextChange[1] = pimpl->m_Method->Next();

	for(unsigned i = 0; i < pimpl->m_Bells.size(); i++)
	{
		(*pimpl)[i].QueueStrike(startTime + (2.5 * i / 6.0) + 1.5, 1);
		(*pimpl)[i].QueueStrike(startTime + (2.5 * i / 6.0) + 4, 1);
	}
	(*pimpl)[0].Command(BellCommand::RING);
	for(unsigned i = 1; i < pimpl->m_Bells.size(); i++)
		pimpl->m_Commands.push(
				new IssueBellCommand(startTime + 2.5*double(i) / 6.0, (*pimpl)[i], BellCommand::RING));
}

void Controller::GoToTop()
{
	for(unsigned i = 0; i < pimpl->m_Bells.size();i++)
	{
		(*pimpl)[i].OverrideBell(M_PI * 182.0 / 180.0, 0);
		(*pimpl)[i].OverrideClapper(M_PI / 8.0, 0);
	}
}

void Controller::DoChange()
{
	CalledMethod* m = dynamic_cast<CalledMethod*>(pimpl->m_Method.get());
	m->Swap(3);
}

void Controller::GoPlainHuntDoubles()
{
	pimpl->m_Method.reset(new StoredMethod(6));
}

void Controller::Sounded(BellStroke stroke, unsigned bell)
{
	Bell& b = (*pimpl)[bell];
	if(bell == 0)
		std::cout << "Sounded bell 0 with error " << b.ErrorOnLastStroke() << std::endl;
	double T = CurrentPeriod();

	// Record the last lead stroke
	RecordLeadTime(bell, b);

	// The control algorithm.  First, ringing up - lead takes care of itself here
	if(b.CurrentCommand() == BellCommand::RING_UP && stroke == BellStroke::HAND_STROKE)
	{
		if(T <= 0)
		{
			T = 1;
		}
		double offset = 0.7*T * double(bell) / 6.0;
		//TODO: This code tries to be too clever by half.  I'm sure it could be much simpler...
		while(pimpl->m_LastLeadStrikeTime + offset + T < pimpl->m_Time)
			offset += 2*T;
		while(b.QueueLength() < 1)
			b.QueueStrike(pimpl->m_LastLeadStrikeTime + offset + T * (1 + b.QueueLength()), pimpl->m_LastLeadEnergyFraction);
	}
	else if(pimpl->m_State == Impl::ControllerState::RING)
	{
		// If we are just starting the next stroke, then swap in the next change
		if(bell == pimpl->m_NextChange[0].WhoIsIn(0))
		{
			pimpl->m_CurrentChange = pimpl->m_NextChange[0];
			pimpl->m_NextChange[0] = pimpl->m_NextChange[1];
			pimpl->m_NextChange[1] = pimpl->m_Method->Next();
		}
		int place = pimpl->m_NextChange[1].WhereIs(bell);
		double offset = T * double(place) / pimpl->m_Bells.size();
		b.QueueStrike(pimpl->m_LastLeadStrikeTime + offset + 2*T, 1);
	}
}

double Controller::CurrentPeriod()
{
	if(pimpl->m_State == Impl::ControllerState::RING)
		return 2.5;
	else
		return (*pimpl)[0].PeriodOfLastStroke();
}

void Controller::RecordLeadTime(unsigned bell, Bell& b)
{
	if(pimpl->m_State == Impl::ControllerState::RING_UP && bell == 0)
	{
		pimpl->m_LastLeadStrikeTime = pimpl->m_Time;
		pimpl->m_LastLeadEnergyFraction = b.EnergyFraction();
	}
	else if(pimpl->m_State == Impl::ControllerState::RING &&
			bell == pimpl->m_NextChange[0].WhoIsIn(0))
	{
		pimpl->m_LastLeadStrikeTime = pimpl->m_Time;
		pimpl->m_LastLeadEnergyFraction = b.EnergyFraction();
	}
}
