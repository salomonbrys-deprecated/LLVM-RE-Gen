/*
 * State.h
 *
 *  Created on: 14 oct. 2010
 *      Author: So
 */

#ifndef STATE_H_
#define STATE_H_

#include <map>
#include <list>

struct IState
{
	virtual ~IState() {}

	virtual void Final(bool) = 0;
	virtual bool Final(void) const = 0;
	virtual void Success(bool) = 0;
	virtual bool Success(void) const = 0;
	virtual const std::multimap<int, IState*> & Transitions(void) const = 0;
	virtual void addTransition(int c, IState* s) = 0;
	virtual int Name(void) const  = 0;
};

struct State : public IState
{
	State() : _name(_nextName), _final(true), _success(true) { ++_nextName; _stateList.push_back(this); }
	virtual ~State() { _stateList.remove(this); }

	virtual void Final(bool is) { _final = is; }
	virtual bool Final(void) const { return _final; }
	virtual void Success(bool is) { _success = is; }
	virtual bool Success(void) const { return _success; }
	virtual const std::multimap<int, IState*> & Transitions(void) const { return _transitions; }
	virtual void addTransition(int c, IState* s) { _transitions.insert(std::pair<int, IState*>(c, s)); }
	virtual int Name(void) const { return _name; }

	static const std::list<State*> & StateList(void) { return _stateList; }

private:
	int	_name;
	bool _final;
	bool _success;
	std::multimap<int, IState*> _transitions;

	static std::list<State*> _stateList;
	static int _nextName;
};

struct StateReplicator : public IState
{
	StateReplicator(IState * orig, IState * copy) : _orig(orig), _copy(copy) {}
	virtual ~StateReplicator() {}

	virtual void Final(bool is) { _orig->Final(is); _copy->Final(is); }
	virtual bool Final(void) const { return _orig->Final(); }
	virtual void Success(bool is) { _orig->Success(is); _copy->Success(is); }
	virtual bool Success(void) const { return _orig->Success(); }
	virtual const std::multimap<int, IState*> & Transitions(void) const { return _orig->Transitions(); };
	virtual void addTransition(int c, IState* s) { _orig->addTransition(c, s); _copy->addTransition(c, s); }
	virtual int Name(void) const { return _orig->Name(); }

private:
	IState * _orig;
	IState * _copy;
};

#endif /* STATE_H_ */
