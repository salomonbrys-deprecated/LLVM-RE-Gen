/*
 * State.h
 *
 *  Created on: 14 oct. 2010
 *      Author: So
 */

#ifndef STATE_H_
#define STATE_H_

#include <map>
#include <set>

struct IState;
struct State;
struct DState;

typedef std::map<int, State*> StateMap;
typedef std::multimap<int, IState*> StateTransitions;

typedef std::pair<int, StateMap> StateHelper;

typedef std::map<int, DState*> DFSM; // = Determinist Finite State Machine
typedef std::map<int, int> DStateTransitions;

struct IState
{
	virtual ~IState() {}

	virtual void Final(bool) = 0;
	virtual bool Final(void) const = 0;
	virtual const StateTransitions & Transitions(void) const = 0;
	virtual void addTransition(int c, IState* s) = 0;
	virtual int Name(void) const  = 0;
};

struct State : public IState
{
	State(StateHelper & helper) : _helper(helper), _name(helper.first), _final(true) { ++helper.first; helper.second.insert(StateMap::value_type(_name, this)); }
	virtual ~State() { _helper.second.erase(_name); }

	virtual void Final(bool is) { _final = is; }
	virtual bool Final(void) const { return _final; }
	virtual const StateTransitions & Transitions(void) const { return _transitions; }
	virtual void addTransition(int c, IState* s) { _transitions.insert(StateTransitions::value_type(c, s)); }
	virtual int Name(void) const { return _name; }

private:
	StateHelper & _helper;

	const int	_name;
	bool _final;
	StateTransitions _transitions;

	State(const State &);
	State & operator = (const State &);
};

struct StateReplicator : public IState
{
	StateReplicator(IState * orig, IState * copy) : _orig(orig), _copy(copy) {}
	virtual ~StateReplicator() {}

	virtual void Final(bool is) { _orig->Final(is); _copy->Final(is); }
	virtual bool Final(void) const { return _orig->Final(); }
	virtual const StateTransitions & Transitions(void) const { return _orig->Transitions(); };
	virtual void addTransition(int c, IState* s) { _orig->addTransition(c, s); _copy->addTransition(c, s); }
	virtual int Name(void) const { return _orig->Name(); }

private:
	IState * _orig;
	IState * _copy;

	StateReplicator(const State &);
	State & operator = (const State &);
};

struct DState
{
	DStateTransitions transitions;
	bool final;
};
void determine(StateMap, DFSM &);
void reduce(DFSM &);

#endif /* STATE_H_ */
