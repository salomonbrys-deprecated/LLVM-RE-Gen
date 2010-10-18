/*
 * State.h
 *
 *  Created on: 14 oct. 2010
 *      Author: So
 */

#ifndef STATE_H_
#define STATE_H_

//#include <list>
#include <map>

struct State;
typedef std::pair<int, std::map<int, State*> > StateHelper;

struct IState
{
	virtual ~IState() {}

	virtual void Final(bool) = 0;
	virtual bool Final(void) const = 0;
	virtual const std::multimap<int, IState*> & Transitions(void) const = 0;
	virtual void addTransition(int c, IState* s) = 0;
	virtual int Name(void) const  = 0;
};

struct State : public IState
{
	State(StateHelper & helper) : _helper(helper), _name(helper.first), _final(true) { ++helper.first; helper.second.insert(std::pair<int, State*>(_name, this)); }
	virtual ~State() { _helper.second.erase(_name); }

	virtual void Final(bool is) { _final = is; }
	virtual bool Final(void) const { return _final; }
	virtual const std::multimap<int, IState*> & Transitions(void) const { return _transitions; }
	virtual void addTransition(int c, IState* s) { _transitions.insert(std::pair<int, IState*>(c, s)); }
	virtual int Name(void) const { return _name; }

private:
	StateHelper & _helper;

	const int	_name;
	bool _final;
	std::multimap<int, IState*> _transitions;

	State(const State &);
	State & operator = (const State &);
};

struct StateReplicator : public IState
{
	StateReplicator(IState * orig, IState * copy) : _orig(orig), _copy(copy) {}
	virtual ~StateReplicator() {}

	virtual void Final(bool is) { _orig->Final(is); _copy->Final(is); }
	virtual bool Final(void) const { return _orig->Final(); }
	virtual const std::multimap<int, IState*> & Transitions(void) const { return _orig->Transitions(); };
	virtual void addTransition(int c, IState* s) { _orig->addTransition(c, s); _copy->addTransition(c, s); }
	virtual int Name(void) const { return _orig->Name(); }

private:
	IState * _orig;
	IState * _copy;

	StateReplicator(const State &);
	State & operator = (const State &);
};

#endif /* STATE_H_ */
