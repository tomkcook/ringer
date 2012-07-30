/*
 * Controller.h
 *
 *  Created on: 18 Jul 2012
 *      Author: tkcook
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <memory>
#include <vector>
#include <queue>

class Bell;
class Command;
class Method;
enum BellCommand : unsigned char;
enum BellStroke : unsigned char;

class CompareCommands
{
public:
	bool operator()(Command*& a, Command*& b);
};

typedef std::priority_queue<Command*, std::vector<Command*>, CompareCommands> CommandQueue;

class Controller
{
public:
	Controller(int bellCount);
	virtual ~Controller();

	bool Update();

	Bell& operator[](int i);
//	std::vector<Bell>::iterator begin();
//	std::vector<Bell>::iterator end();
	int size();

	void GoSlow();

	void RingUp();
	void Ring();
	void GoToTop();
	void DoChange();
	void GoPlainHuntDoubles();

protected:
	struct Impl;
	std::auto_ptr<Impl> pimpl;

	double SimTime() const;
	void Sounded(BellStroke stroke, unsigned bell);
	void SetMethod(Method *m);
	double CurrentPeriod();
	void RecordLeadTime(unsigned bell, Bell& b);
};

#endif /* CONTROLLER_H_ */
