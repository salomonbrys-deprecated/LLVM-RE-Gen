//============================================================================
// Name        : LLVM-RE-Gen.cpp
// Author      : Salomon BRYS
// Version     :
// Copyright   : Salomon BRYS, Apache Lisence
// Description : Compile a regexp in LLVM
//============================================================================

#include <iostream>
#include <iomanip>

#include "INode.h"

int main()
{
//	std::string regexp = "a*ab?c?.(bb)?.b(cc)?.?"; // test : acbb
	std::string regexp = "[[:alpha:]_][[:word:]]*"; // test : acbb

	try
	{
		INode * n = parseRegExp(regexp.begin(), regexp.end());
		std::cout << *n << std::endl;
		State start;
		n->stateify(&start, 0, true);
		delete n;

		const std::list<State*> & states = State::StateList();
		for (std::list<State*>::const_iterator sIt = states.begin(); sIt != states.end(); ++sIt)
		{
			std::cout << "STATE " << (*sIt)->Name();
			if ((*sIt)->Final())
				std::cout << ((*sIt)->Success() ? " (S)" : " (F)");
			std::cout << std::endl;
			for (std::multimap<int, IState*>::const_iterator tIt = (*sIt)->Transitions().begin(); tIt != (*sIt)->Transitions().end(); ++tIt)
				std::cout	<< std::setw(5) << tIt->first << ':'
							<< (isgraph(tIt->first) ? (char)tIt->first : ' ')
							<< " -> " << tIt->second->Name() << std::endl;
		}
	}
	catch (const std::string & str)
	{
		std::cout	<< std::endl
					<< "=== ERROR =======================" << std::endl
					<< str << std::endl
					<< "=================================" << std::endl;
	}

	return 0;
}
