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

int main(int argc, char ** argv)
{
	try
	{
		TCLAP::CmdLine cmd("LLVM Compiled Regular Expression Generator by Salomon BRYS", ' ', "0.1");

		TCLAP::ValueArg<std::string> outputFile("o", "output-file", "Write output to <file>.", false, "", "string");
		cmd.add(outputFile);
		TCLAP::ValueArg<std::string> prefix("p", "prefix", "Prefix of the functions.", false, "", "identifier");
		cmd.add(prefix);
		TCLAP::SwitchArg noPrint("d", "disable-display", "Do NOT print the generated function name for each given regexp when output-file is set.", false);
		cmd.add(noPrint);

		TCLAP::UnlabeledMultiArg<std::string> regexp("regexp",
				"Regular expression to be compiled.\n"
				"Start with a '!' if you want the regular expression to stop at first match. This is *not* part of the regexp syntax but part of this program syntax.",
			true, "string");
		cmd.add(regexp);

		cmd.parse(argc, argv);

		if (!prefix.getValue().empty())
			LLVMRE_Instance().setDefaultPrefix(prefix.getValue());

		int align = 0;
		if (!outputFile.getValue().empty() && !noPrint.getValue())
			for (std::vector<std::string>::const_iterator it = regexp.getValue().begin(); it != regexp.getValue().end(); ++it)
				if (align < (int)it->length())
					align = it->length();

		LLVMRE_Instance().initializeJITEngine(3);
		for (std::vector<std::string>::const_iterator it = regexp.getValue().begin(); it != regexp.getValue().end(); ++it)
		{
			std::string re = *it;

			bool stopAtFirstMatch = false;
			if ((*it)[0] == '!')
			{
				stopAtFirstMatch = true;
				re = re.substr(1);
			}

			LLVMRE::Func * f = LLVMRE_Instance().createRE(re, stopAtFirstMatch);
			f->JITFunc(3);
			if (!outputFile.getValue().empty() && !noPrint.getValue())
				std::cout << std::setw(align + 2) << re << ": int " << f->getFuncName() << "(const char *);" << std::endl;
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
