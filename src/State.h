/*
 * State.h
 *
 *  Created on: 14 oct. 2010
 *      Author: So
 */

#ifndef STATE_H_
#define STATE_H_

#include <queue>
#include <map>
#include <set>

struct IState;
struct State;
struct StateReplicator;
struct DState;

typedef std::queue<StateReplicator*> ReplicatorQueue;
//typedef std::map<int, State*> StateMap;
typedef std::vector<State*> StateVector;
typedef std::multimap<int, IState*> StateTransitions;

//typedef std::pair<int, StateMap> StateHelper;
struct StateHelper
{
	StateHelper() {}
	StateVector states;
	ReplicatorQueue queue;

	void clear();
};

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
	State(StateHelper & helper) : _helper(helper), _name(helper.states.size()), _final(true) { helper.states.push_back(this); }
	virtual ~State() { _helper.states[_name] = 0; }

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
	StateReplicator(IState * orig, IState * copy, StateHelper & helper) : _orig(orig), _copy(copy) { helper.queue.push(this); }
	virtual ~StateReplicator() {}

	virtual void Final(bool is) { _orig->Final(is); _copy->Final(is); }
	virtual bool Final(void) const { return _orig->Final(); }
	virtual const StateTransitions & Transitions(void) const { return _orig->Transitions(); }
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

struct DFSM : public std::map<int, DState*> // = Determinist Finite State Machine
{
	void clearStates()
	{
		for (DFSM::iterator it = begin(); it != end(); )
		{
			DState * state = it->second;
			DFSM::iterator stateIt = it;
			++it;
			erase(stateIt);
			delete state;
		}
	}
};

void determine(StateVector &, DFSM &);
void reduce(DFSM &);

#endif /* STATE_H_ */
