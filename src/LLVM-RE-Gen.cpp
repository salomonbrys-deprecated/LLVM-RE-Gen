//============================================================================
// Name        : LLVM-RE-Gen.cpp
// Author      : Salomon BRYS
// Version     :
// Copyright   : Salomon BRYS, Apache Lisence
// Description : Compile a regexp in LLVM
//============================================================================

#include <iostream>
#include <string>
#include <iomanip>

#ifdef _WIN
#include <windows.h>
#endif

#include "INode.h"

int main()
{
	std::string regexp = "a*ab?c?.(bb)?.b(cc)?.?"; // test : acbb
//	std::string regexp = "a(b?(b|c)*|c?(b|c)*)"; // test the reduction

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
		for (StateMap::const_iterator sIt = helper.second.begin(); sIt != helper.second.end(); ++sIt)
		{
			std::cout << "ND-STATE " << sIt->first << (sIt->second->Final() ? " (F)" : "") << std::endl;
			for (StateTransitions::const_iterator tIt = sIt->second->Transitions().begin(); tIt != sIt->second->Transitions().end(); ++tIt)
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

			for (DStateTransitions::const_iterator tIt = sIt->second->transitions.begin(); tIt != sIt->second->transitions.end(); ++tIt)
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

			for (DStateTransitions::const_iterator tIt = sIt->second->transitions.begin(); tIt != sIt->second->transitions.end(); ++tIt)
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
