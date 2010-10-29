//============================================================================
// Name        : State.cpp
// Author      : Salomon BRYS
// Copyright   : Salomon BRYS, Apache Lisence
//============================================================================

#include "State.h"

#include <queue>

void StateHelper::clear()
{
	for (StateVector::iterator it = states.begin(); it != states.end(); ++it)
		if (*it)
			delete *it;

	states.clear();

	while (!queue.empty())
	{
		delete queue.front();
		queue.pop();
	}
}


void determine(StateVector & ndStateList, DFSM & dfsm)
{
	typedef std::set<int> DStateSet;
	typedef std::map<DStateSet, int> ConnectionsMap;
	ConnectionsMap connections;
	std::queue<DStateSet> toDo;

	DStateSet startSet;
	startSet.insert(0);
	connections.insert(ConnectionsMap::value_type(startSet, 0));
	toDo.push(startSet);

	int id = 0;
	while (!toDo.empty())
	{
		DState *dstate = new DState;
		dstate->final = false;
		DStateSet & front = toDo.front();

		DStateSet involved;
		for (DStateSet::const_iterator stId = front.begin(); stId != front.end(); ++stId)
		{
			State * state = ndStateList[*stId];
			if (state->Final())
				dstate->final = true;

			for (StateTransitions::const_iterator tr = state->Transitions().begin(); tr != state->Transitions().end(); ++tr)
				involved.insert(tr->first);
		}

		for (DStateSet::const_iterator chars = involved.begin(); chars != involved.end(); ++chars)
		{
			DStateSet nextState;
			for (DStateSet::const_iterator stId = front.begin(); stId != front.end(); ++stId)
			{
				State * state = ndStateList[*stId];
				std::pair<StateTransitions::const_iterator, StateTransitions::const_iterator> eqr;
				eqr = state->Transitions().equal_range(*chars);
				for (; eqr.first != eqr.second; ++eqr.first)
					nextState.insert(eqr.first->second->Name());
				eqr = state->Transitions().equal_range(-1);
				for (; eqr.first != eqr.second; ++eqr.first)
					nextState.insert(eqr.first->second->Name());
			}

			int nextStateId;
			ConnectionsMap::const_iterator nextStateIt = connections.find(nextState);
			if (nextStateIt != connections.end())
				nextStateId = nextStateIt->second;
			else
			{
				nextStateId = ++id;
				//connections[nextState] = nextStateId;
				connections.insert(ConnectionsMap::value_type(nextState, nextStateId));
				toDo.push(nextState);
			}

			DStateTransitions::const_iterator transitionIt = dstate->transitions.find(-1);
			if (transitionIt == dstate->transitions.end() || transitionIt->second != nextStateId)
				//dstate->transitions[*chars] = nextStateId;
				dstate->transitions.insert(DStateTransitions::value_type(*chars, nextStateId));
		}

		//dfsm[connections[toDo.front()]] = dstate;
		dfsm.insert(DFSM::value_type(connections[front], dstate));

		toDo.pop();
	}
}

void reduce(DFSM & dfsm)
{
	bool cont = true;
	std::map<int, DStateTransitions> RecursionTransitions;
	while (cont)
	{
		cont = false;

		for (DFSM::iterator i = dfsm.begin(); i != dfsm.end(); ++i)
		{
			DStateTransitions & rTr = (RecursionTransitions[i->first] = i->second->transitions);
			for (DStateTransitions::iterator tr = rTr.begin(); tr != rTr.end(); ++tr)
				if (tr->second == i->first)
					tr->second = -2;
		}

		for (DFSM::iterator i = dfsm.begin(); i != dfsm.end(); ++i)
		{
			DFSM::iterator j = i;

			DStateTransitions & iRecursTr = RecursionTransitions[i->first];

			++j;
			while (j != dfsm.end())
			{
				if (j->second->final == i->second->final && (RecursionTransitions[j->first] == iRecursTr || j->second->transitions == i->second->transitions ))
				{
					for (DFSM::iterator st = dfsm.begin(); st != dfsm.end(); ++st)
					{
						for (DStateTransitions::iterator tr = st->second->transitions.begin(); tr != st->second->transitions.end(); ++tr)
							if (tr->second == j->first)
							{
								tr->second = i->first;
								cont = true;
							}
					}
					DFSM::iterator erase = j;
					++j;
					delete erase->second;
					dfsm.erase(erase);
				}
				else
					++j;
			}
		}
	}
}
