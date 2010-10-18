/*
 * DState.cpp
 *
 *  Created on: Oct 18, 2010
 *      Author: salomon
 */

#include "State.h"

#include <queue>

void determine(StateMap ndStateList, DFSM & dfsm)
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

		//std::cout << "D-STATE " << connections[toDo.front()] << std::endl;

		//std::cout << "  ND-STATEs: ";

		DStateSet involved;
		for (DStateSet::const_iterator stId = toDo.front().begin(); stId != toDo.front().end(); ++stId)
		{
			//std::cout << *stId << ' ';

			if (ndStateList[*stId]->Final())
				dstate->final = true;

			for (StateTransitions::const_iterator tr = ndStateList[*stId]->Transitions().begin(); tr != ndStateList[*stId]->Transitions().end(); ++tr)
				involved.insert(tr->first);
		}

		//std::cout << (dstate->final ? " (F)" : "") << std::endl;

		//std::cout << "  CHARs" << std::endl;

		for (DStateSet::const_iterator chars = involved.begin(); chars != involved.end(); ++chars)
		{
			//std::cout << std::setw(7) << *chars << ':' << (isgraph(*chars) ? (char)*chars : ' ');

			DStateSet nextState;
			for (DStateSet::const_iterator stId = toDo.front().begin(); stId != toDo.front().end(); ++stId)
			{
				std::pair<StateTransitions::const_iterator, StateTransitions::const_iterator> eqr;
				eqr = ndStateList[*stId]->Transitions().equal_range(*chars);
				for (; eqr.first != eqr.second; ++eqr.first)
					nextState.insert(eqr.first->second->Name());
				eqr = ndStateList[*stId]->Transitions().equal_range(-1);
				for (; eqr.first != eqr.second; ++eqr.first)
					nextState.insert(eqr.first->second->Name());
			}

			int nextStateId;
			if (connections.find(nextState) != connections.end())
				nextStateId = connections[nextState];
			else
			{
				nextStateId = ++id;
				connections[nextState] = nextStateId;
				toDo.push(nextState);
			}

			if (dstate->transitions.find(-1) == dstate->transitions.end() || dstate->transitions.find(-1)->second != nextStateId)
				dstate->transitions[*chars] = nextStateId;

			//std::cout << " -> " << nextStateId << std::endl;
		}

		dfsm[connections[toDo.front()]] = dstate;

		toDo.pop();
	}
}

void reduce(DFSM & dfsm)
{
	bool cont = true;
	while (cont)
	{
//		std::cout << "REDUCE ITERATION" << std::endl;
		cont = false;
		for (DFSM::iterator i = dfsm.begin(); i != dfsm.end(); ++i)
		{
			DFSM::iterator j = i;

			DStateTransitions iTransitions = i->second->transitions;
			for (DStateTransitions::iterator tr = iTransitions.begin(); tr != iTransitions.end(); ++tr)
				if (tr->second == i->first)
					tr->second = -2;

			++j;
			while (j != dfsm.end())
			{
//				std::cout << "testing " << i->first << " and " << j->first << std::endl;
				DStateTransitions jTransitions = j->second->transitions;
				for (DStateTransitions::iterator tr = jTransitions.begin(); tr != jTransitions.end(); ++tr)
					if (tr->second == j->first)
						tr->second = -2;
				if (j->second->final == i->second->final && (jTransitions == iTransitions || j->second->transitions == i->second->transitions ))
				{
//					std::cout << "SAME: " << i->first << ", " << j->first << std::endl;
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
					dfsm.erase(erase);
				}
				else
					++j;
			}
		}
	}
}
