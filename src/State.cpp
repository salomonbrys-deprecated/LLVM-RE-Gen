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
	std::vector<ConstStateTransitionsRange> anyRanges(ndStateList.size());

	for (StateVector::iterator state = ndStateList.begin(); state != ndStateList.end(); ++state)
		anyRanges.push_back((*state)->Transitions().equal_range(-1));

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
				ConstStateTransitionsRange eqr;
				eqr = state->Transitions().equal_range(*chars);
				for (; eqr.first != eqr.second; ++eqr.first)
					nextState.insert(eqr.first->second->Name());
				eqr = state->Transitions().equal_range(-1);
				for (StateTransitions::const_iterator anyEqr = anyRanges[*stId].first; anyEqr != anyRanges[*stId].second; ++anyEqr)
					nextState.insert(anyEqr->second->Name());
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
		//dfsm.insert(DFSM::value_type(connections[front], dstate));
		dfsm.push_back(dstate);

		toDo.pop();
	}
}

void reduce(DFSM & dfsm)
{
	bool cont = true;
	std::vector<DStateTransitions> RecursionTransitions;
	while (cont)
	{
		cont = false;

		int index = 0;
		for (DFSM::iterator st = dfsm.begin(); st != dfsm.end(); ++st, ++index)
			if (*st)
			{
				RecursionTransitions.push_back((*st)->transitions);
				for (DStateTransitions::iterator tr = RecursionTransitions[index].begin(); tr != RecursionTransitions[index].end(); ++tr)
					if (tr->second == index)
						tr->second = -2;
			}

		int masterIndex = 0;
		for (DFSM::iterator master = dfsm.begin(); master != dfsm.end(); ++master, ++masterIndex)
			if (*master)
			{
				DFSM::iterator slave = master;

				DStateTransitions & masterRecursTr = RecursionTransitions[masterIndex];


				int slaveIndex = masterIndex + 1;
				for (++slave; slave != dfsm.end(); ++slave, ++slaveIndex)
					if (*slave && (*slave)->final == (*master)->final && (RecursionTransitions[slaveIndex] == masterRecursTr || (*slave)->transitions == (*master)->transitions))
					{
						for (DFSM::iterator st = dfsm.begin(); st != dfsm.end(); ++st)
							if (*st)
								for (DStateTransitions::iterator tr = (*st)->transitions.begin(); tr != (*st)->transitions.end(); ++tr)
									if (tr->second == slaveIndex)
									{
										tr->second = masterIndex;
										cont = true;
									}
						delete *slave;
						*slave = 0;
					}
			}
	}
}
