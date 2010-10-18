//============================================================================
// Name        : LLVM-RE-Gen.cpp
// Author      : Salomon BRYS
// Version     :
// Copyright   : Salomon BRYS, Apache Lisence
// Description : Compile a regexp in LLVM
//============================================================================

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <iomanip>

#ifdef _WIN
#include <windows.h>
#endif

#include "INode.h"

struct DState
{
	std::map<int, int> transitions;
	bool final;

	bool operator == (const DState & l) { return (final == l.final && transitions == l.transitions); }
};

typedef std::map<int, DState*> DFSM; // = Determinist Finite State Machine

void determine(std::map<int, State*> ndStateList, DFSM & dfsm)
{
	std::map<std::set<int>, int> connections;
	std::queue<std::set<int> > toDo;

	std::set<int> startSet;
	startSet.insert(0);
	connections.insert(std::pair<std::set<int>, int>(startSet, 0));
	toDo.push(startSet);

	int id = 0;
	while (!toDo.empty())
	{
		DState *dstate = new DState;
		dstate->final = false;

		//std::cout << "D-STATE " << connections[toDo.front()] << std::endl;

		//std::cout << "  ND-STATEs: ";

		std::set<int> involved;
		for (std::set<int>::const_iterator stId = toDo.front().begin(); stId != toDo.front().end(); ++stId)
		{
			//std::cout << *stId << ' ';

			if (ndStateList[*stId]->Final())
				dstate->final = true;

			for (std::multimap<int, IState*>::const_iterator tr = ndStateList[*stId]->Transitions().begin(); tr != ndStateList[*stId]->Transitions().end(); ++tr)
				involved.insert(tr->first);
		}

		//std::cout << (dstate->final ? " (F)" : "") << std::endl;

		//std::cout << "  CHARs" << std::endl;

		for (std::set<int>::const_iterator chars = involved.begin(); chars != involved.end(); ++chars)
		{
			//std::cout << std::setw(7) << *chars << ':' << (isgraph(*chars) ? (char)*chars : ' ');
			
			std::set<int> nextState;
			for (std::set<int>::const_iterator stId = toDo.front().begin(); stId != toDo.front().end(); ++stId)
			{
				std::pair<std::multimap<int, IState*>::const_iterator, std::multimap<int, IState*>::const_iterator> eqr;
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
		std::cout << "REDUCE ITERATION" << std::endl;
		cont = false;
		for (DFSM::iterator i = dfsm.begin(); i != dfsm.end(); ++i)
		{
			DFSM::iterator j = i;
			for (++j; j != dfsm.end(); ++j)
				if (*j->second == *i->second)
				{
					std::cout << "SAME: " << i->first << ", " << j->first << std::endl;
					for (DFSM::iterator st = dfsm.begin(); st != dfsm.end(); ++st)
						for (std::map<int, int>::iterator tr = st->second->transitions.begin(); tr != st->second->transitions.end(); ++tr)
							if (tr->second == j->first)
							{
								tr->second = i->first;
								cont = true;
							}
					dfsm.erase(j);
				}
		}
	}
}

int main()
{
	std::string regexp = "a*ab?c?.(bb)?.b(cc)?.?"; // test : acbb

	try
	{
		INode * n = parseRegExp(regexp.begin(), regexp.end());

		std::cout << std::endl << "===== AST ==============================" << std::endl;
		std::cout << *n;
		std::cout << "========================================" << std::endl;

		StateHelper helper;
		new State(helper);
		n->stateify(helper.second[0], 0, true, helper);
		delete n;

		std::cout << std::endl << "===== ND-FSM ===========================" << std::endl;
		for (std::map<int, State*>::const_iterator sIt = helper.second.begin(); sIt != helper.second.end(); ++sIt)
		{
			std::cout << "ND-STATE " << sIt->first << (sIt->second->Final() ? " (F)" : "") << std::endl;
			for (std::multimap<int, IState*>::const_iterator tIt = sIt->second->Transitions().begin(); tIt != sIt->second->Transitions().end(); ++tIt)
				std::cout	<< std::setw(5) << tIt->first << ':'
							<< (isgraph(tIt->first) ? (char)tIt->first : ' ')
							<< " -> " << tIt->second->Name() << std::endl;
		}
		std::cout << "========================================" << std::endl;

		DFSM dfsm;
		determine(helper.second, dfsm);

		std::cout << std::endl << "===== D-FSM ============================" << std::endl;
		for (DFSM::const_iterator sIt = dfsm.begin(); sIt != dfsm.end(); ++sIt)
		{
			std::cout << "D-STATE " << sIt->first << (sIt->second->final ? " (F)" : "") << std::endl;

			for (std::map<int, int>::const_iterator tIt = sIt->second->transitions.begin(); tIt != sIt->second->transitions.end(); ++tIt)
				std::cout	<< std::setw(5) << tIt->first << ':'
							<< (isgraph(tIt->first) ? (char)tIt->first : ' ')
							<< " -> " << tIt->second << std::endl;
		}
		std::cout << "========================================" << std::endl;

		reduce(dfsm);

		std::cout << std::endl << "===== R-D-FSM ==========================" << std::endl;
		for (DFSM::const_iterator sIt = dfsm.begin(); sIt != dfsm.end(); ++sIt)
		{
			std::cout << "D-STATE " << sIt->first << (sIt->second->final ? " (F)" : "") << std::endl;

			for (std::map<int, int>::const_iterator tIt = sIt->second->transitions.begin(); tIt != sIt->second->transitions.end(); ++tIt)
				std::cout	<< std::setw(5) << tIt->first << ':'
							<< (isgraph(tIt->first) ? (char)tIt->first : ' ')
							<< " -> " << tIt->second << std::endl;
		}
		std::cout << "========================================" << std::endl;
}
	catch (const std::string & str)
	{
		std::cout	<< std::endl
					<< "--- ERROR -----------------------" << std::endl
					<< str << std::endl
					<< "---------------------------------" << std::endl;
	}

	#ifdef _WIN
		system("pause");
	#endif

	return 0;
}
