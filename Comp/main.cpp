/*
 * main.cpp
 *
 *  Created on: Oct 27, 2010
 *      Author: salomon
 */

#include <iostream>
#include <iomanip>
#include <tclap/CmdLine.h>

#include <LLVMREGen.h>

#include <llvm/Module.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include "LLVMREGen.h"

class OptimLevelConstraint : public TCLAP::Constraint<int>
{
	public:
		virtual std::string description() const { return "Integer between 0 and 3"; }
		virtual std::string shortID() const { return "int"; }
		virtual bool check(const int& value) const { return value >= 0 && value <= 3; }
		virtual ~OptimLevelConstraint() {}
};

int main(int argc, char ** argv)
{
	try
	{
		TCLAP::CmdLine cmd("LLVM Compiled Regular Expression Generator by Salomon BRYS", ' ', "0.1");

		TCLAP::ValueArg<std::string> outputFile("o", "output-file", "Write output to <file>", false, "", "string");
		cmd.add(outputFile);
		TCLAP::SwitchArg noPrint("n", "no-print-fnames", "Do NOT print the generated function name for each given regexp when output-file is set", false);
		cmd.add(noPrint);

		OptimLevelConstraint olc;
		TCLAP::ValueArg<int> optimizationLevel("O", "optimization-level", "Optimization level (0, 1, 2: def, 3)", false, 2, &olc);
		cmd.add(optimizationLevel);

		TCLAP::UnlabeledMultiArg<std::string> regexp("regexp", "Regular expression to be compiled", true, "string");
		cmd.add(regexp);

		cmd.parse(argc, argv);

		LLVMRE_Instance().initializeJITEngine(3);
		for (std::vector<std::string>::const_iterator it = regexp.getValue().begin(); it != regexp.getValue().end(); ++it)
		{
			LLVMRE::Func * f = LLVMRE_Instance().createRE(*it);
			f->JITFunc(3);
			if (!outputFile.getValue().empty() && !noPrint.getValue())
				std::cout << std::setw(10) << f->getFuncName() << ": " << *it << std::endl;
		}

		if (outputFile.getValue().empty())
			LLVMRE_Instance().WriteBitcodeToFile(&llvm::outs());
		else
		{
			std::string errorInfo;
			llvm::raw_fd_ostream out(outputFile.getValue().c_str(), errorInfo);
			if (!errorInfo.empty())
				throw errorInfo;
			LLVMRE_Instance().WriteBitcodeToFile(&out);
		}
	}
	catch (TCLAP::ArgException &e)
	{
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	catch (std::string &e)
	{
		std::cerr << "Error: " << e << std::endl;
	}
}
