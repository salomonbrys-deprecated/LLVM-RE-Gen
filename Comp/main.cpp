/*
 * main.cpp
 *
 *  Created on: Oct 27, 2010
 *      Author: salomon
 */

#if defined(WIN32)
#define NOMINMAX
#endif

#include <LLVMREGen.h>

#include <llvm/Module.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/raw_ostream.h>

#include <tclap/CmdLine.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>

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
		TCLAP::SwitchArg noOpt("n", "no-optimizations", "Do NOT optimize the LLVM / binary code. Should only be used for debbugging", false);
		cmd.add(noOpt);

		std::vector<std::string> headerAllowed;
		headerAllowed.push_back("C");
		headerAllowed.push_back("C++");
		TCLAP::ValuesConstraint<std::string> headerAllowedConstraint(headerAllowed);
		TCLAP::ValueArg<std::string> header("H", "header", "Generate a header for the given language. Uses the prefix for the file name.", false, "", &headerAllowedConstraint);
		cmd.add(header);

		TCLAP::UnlabeledMultiArg<std::string> regexp("regexp",
				"Regular expression to be compiled.\n"
				"Start with a '!' if you want the regular expression to stop at first match. This is *not* part of the regexp syntax but part of this program syntax.",
			true, "string");
		cmd.add(regexp);

		cmd.parse(argc, argv);

		if (!prefix.getValue().empty())
			LLVMRE_Instance().setDefaultPrefix(prefix.getValue());

		std::ofstream ofs;
		std::string protection;
		if (!header.getValue().empty())
		{
			ofs.open((LLVMRE_Instance().getDefaultPrefix() + ".h").c_str(), std::ios_base::out | std::ios_base::trunc);
			protection = std::string() + "_LLVMRE_" + LLVMRE_Instance().getDefaultPrefix() + "_INCLUDED";
			ofs << "// This file has been generated by llvm-regexp. Do NOT modify." << std::endl << std::endl;
			ofs << "#ifndef " << protection << std::endl << "#define " << protection << std::endl << std::endl;
			if (header.getValue() == "C++")
				ofs << "extern \"C\" {" << std::endl << std::endl;
		}

		int align = 0;
		if ((!outputFile.getValue().empty() && !noPrint.getValue()) || !header.getValue().empty())
			for (std::vector<std::string>::const_iterator it = regexp.getValue().begin(); it != regexp.getValue().end(); ++it)
				if (align < (int)it->length())
					align = it->length();

		int optLvl = noOpt.getValue() ? 0 : 3;

		LLVMRE_Instance().initializeJITEngine(optLvl);
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
			f->JITFunc(optLvl);

			std::string definition = std::string() + "int " + f->getFuncName() + "(const char *);";

			std::string rePrint = (stopAtFirstMatch ? "! " : "  ") + re;

			if (!outputFile.getValue().empty() && !noPrint.getValue())
				std::cout << std::setw(align + 1) << rePrint << ": " << definition << std::endl;

			if (!header.getValue().empty())
				ofs << "/*" << std::setw(align + 1) << rePrint << " */ " << definition << std::endl;
		}

		if (!header.getValue().empty())
		{
			if (header.getValue() == "C++")
				ofs << std::endl << "}" << std::endl;
			ofs << std::endl << "#endif // !" << protection << std::endl;
			ofs.close();
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
