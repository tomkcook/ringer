/*
 * Method.h
 *
 *  Created on: 27 Jun 2012
 *      Author: tkcook
 */

#ifndef METHOD_H_
#define METHOD_H_

#include <vector>
#include <stdexcept>

// Represents a single change / round of ringing
// Note that both bells and places are 0-based in this class!
class Change : public std::vector<int> {
public:
	Change(int bell_count);
	// Find out what place a bell is ringing in this change
	int WhereIs(int bell) throw(std::out_of_range);
	// Swap the bells ringing place-1 and place.
	// A called change of '2 to 3' means Swap(WhereIs(3))
	void Swap(int place) throw(std::out_of_range);
};

class Method {
public:
	Method(int bell_count);
	virtual ~Method();

	virtual void Reset() = 0;
	virtual bool HasNext() = 0;
	virtual Change& Next() = 0;
	virtual Change& Current() = 0;
};

class StoredMethod : public std::vector<Change>, public Method {
	StoredMethod(int bell_count);
	virtual ~StoredMethod();

	void Reset();
	bool HasNext();
	Change& Next() throw(std::out_of_range);
	Change& Current();

protected:
	int m_iCurrentIndex;
};

class CalledMethod : public Method {
	CalledMethod(int bell_count);
	virtual ~CalledMethod();

	void Reset();
	bool HasNext();
	Change& Next() throw();
	Change& Swap(int place);
	Change& Current();

protected:
	Change m_CurrentChange;
};

#endif /* METHOD_H_ */
