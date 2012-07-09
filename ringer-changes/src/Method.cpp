/*
 * Method.cpp
 *
 *  Created on: 27 Jun 2012
 *      Author: tkcook
 */

#include "Method.h"

Method::Method(int bell_count) {
	// TODO Auto-generated constructor stub

}

Method::~Method() {
	// TODO Auto-generated destructor stub
}

Change::Change(int bell_count) {
	for(int i = 0; i < bell_count; i++)
		push_back(i);
}

int Change::WhereIs(int bell) throw(std::out_of_range) {
	int place = 0;
	for(iterator i = begin(); i != end(); i++, place++) {
		if(*i == bell)
			return place;
	}
	throw std::out_of_range("Out of range");
}

void Change::Swap(int place) throw (std::out_of_range) {
	if(place >= size() || place == 0)
		throw std::out_of_range("Out of range");

	int tmp = at(place);
	at(place) = at(place-1);
	at(place-1) = tmp;
}

StoredMethod::StoredMethod(int bell_count)
	: Method(bell_count), std::vector<Change>()
{
	push_back(Change(bell_count));
}

StoredMethod::~StoredMethod() {
}

void StoredMethod::Reset() {
	m_iCurrentIndex = 0;
}

bool StoredMethod::HasNext() {
	return m_iCurrentIndex < size()-1;
}

Change& StoredMethod::Next() throw(std::out_of_range){
	return at(++m_iCurrentIndex);
}

Change& StoredMethod::Current() {
	return at(m_iCurrentIndex);
}

CalledMethod::CalledMethod(int bell_count) :
	Method(bell_count), m_CurrentChange(bell_count)
{
}

CalledMethod::~CalledMethod() {
}

void CalledMethod::Reset() {
	m_CurrentChange = Change(m_CurrentChange.size());
}

bool CalledMethod::HasNext() {
	return true;
}

Change& CalledMethod::Next() throw() {
	return m_CurrentChange;
}

Change& CalledMethod::Swap(int place) {
	m_CurrentChange.Swap(place);
	return m_CurrentChange;
}

Change& CalledMethod::Current() {
	return m_CurrentChange;
}


