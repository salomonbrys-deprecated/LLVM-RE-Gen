
#ifndef LLVM_RE_GEN_H_
#define LLVM_RE_GEN_H_

#include <string>

#if !defined(__LLVMRE_Dll)
	#if defined(_WIN) && (defined(_WINDLL) || defined(LLVMRE_USE_WINDLL))
		#if defined(_WINDLL)
			#define __LLVMRE_Dll __declspec( dllexport )
		#else
			#define __LLVMRE_Dll __declspec( dllimport )
		#endif
	#else
		#define __LLVMRE_Dll
	#endif
#endif

#if defined(_WIN)
#include <windows.h>
#define int32_t INT32
#else
#include <sys/types.h>
#endif

namespace llvm
{
	class Function;
	class Module;
	class raw_ostream;
}

// Singleton class accessible by the function LLVMRE_Instance().
// Used to create regular expressions LLVMRE::Func objects.
class LLVMRE
{
public:

	class Func
	{
	public:
		// Virtual destructor
		virtual ~Func() {}

		// Execute the regular expression function on the given C-String.
		// Return the number of matched character (or 0 if none).
		virtual int operator () (const char *str) = 0;
		// Execute the regular expression function on the given C-String.
		// Return the number of matched character (or 0 if none).
		virtual int execute(const char *str) = 0;
		// Execute the regular expression function on the given C-String.
		// Return the number of matched character (or 0 if none).
		virtual int operator () (const char *str) const = 0;
		// Execute the regular expression function on the given C-String.
		// Return the number of matched character (or 0 if none).
		virtual int execute(const char *str) const = 0;

		// Force the compiling and JITing of the regular expression if it has not already been compiled or JIT.
		// Permit to compile the function on a on demand, and not just before an execution.
		// If the name is not set, than it will be [prefix]_[X] where prefix is set by LLVMRE_Instance().setDefaultPrefix()
		//   or "llvmre_" if not set and X is the creation number of the regular expression (starting at 0).
		virtual void JITFunc(int optimizationLevel = 0, const std::string & name = "") = 0;
		// Force the compiling of the regular expression if it has not already been compiled.
		// If the name is not set, than it will be [prefix]_[X] where prefix is set by LLVMRE_Instance().setDefaultPrefix()
		//   or "llvmre_" if not set and X is the creation number of the regular expression (starting at 0).
		virtual void compileInLLVM(int optimizationLevel = 0, const std::string & name = "") = 0;

		// Return a LLVM::Function that correspond to the regular expression.
		// Compile the regular expression if it has not been compiled yet.
		virtual const llvm::Function * getLLVMFunction() = 0;
		// Return a LLVM::Function that correspond to the regular expression.
		// Retrun 0 if the regular expression has not been compiled yet.
		virtual const llvm::Function * getLLVMFunction() const = 0;

		// The C function pointer type.
		// A regular expression C function is a function that takes the C-String to test
		//   and returns the number of matched character (or 0 if none).
		typedef int32_t (*CFuncPtr)(const char *);

		// Return a C function pointer corresponding to the regular expression.
		// Compile and JIT the regular expression if it has not been compiled yet.
		virtual CFuncPtr getCFunc() = 0;
		// Return a C function pointer corresponding to the regular expression.
		// Retrun 0 if the regular expression has not been compiled or JITed yet.
		virtual CFuncPtr getCFunc() const = 0;

		// Return wether the function has already been compiled and JIT.
		virtual bool isJIT() const = 0;

		// Return the name function if it has been compiled (or "" if not).
		virtual std::string getFuncName() const = 0;
		// Return the regular expression string.
		virtual const std::string & getRegexp() const = 0;

		// Generate a random string that should match completely the regular expression.
		// This method is for test and benchmark purpose.
		virtual std::string getRandomTryString() const = 0;

		// The Compiling Policy Enum
		enum Policy
		{
			// Always Compile and JIT the regular expression (happens on the first execution).
			JIT_ALWAYS,
			// Automatically JIT the regular expression after 10000 executions.
			JIT_AUTO,
			// Never automatically JIT the regular expression.
			// It can still be manually compiled and JIT by calling the JitFunc method.
			JIT_NEVER
		};

		// Get the current compiling policy.
		virtual Policy getPolicy() const = 0;
		// Set the policy for the regular expression.
		virtual void setPolicy(Func::Policy) = 0;
	};

	// Virtual destructor
	virtual ~LLVMRE() {}

	// Create a regular expressions LLVMRE::Func object from a regular expression string.
	// The stopAtFirstMatch parameter is very important: consider the a[bc]*c regular expression
	//   and the "abcbce" test string. With a false stopAtFirstMatch (default), the function will
	//   match 5 characters ("abcbc"). With a true stopAtFirstMatch, the function will only match
	//   3 characters as "abc" is the first correct match for the given string.
	virtual Func * createRE(const std::string & regexp, bool stopAtFirstMatch = false) = 0;

	// Force the initialization of LLVM. Is normally called on the first time a regular expression
	//   needs to be compiled. Can be called to explicitely initialize LLVM and thus quicken the
	//   first compilation when it happens (usually automatically before an execution).
	virtual void initilizeLLVM() = 0;
	// Force the initialization of LLVM JIT Engine. Is normally called on the first time a regular
	//   expression needs to be JITed. Can be called to explicitely the JIT Engine and thus quicken
	//   the first JIT when it happens (usually automatically before an execution).
	// Can also be called to set the optimization level (0, 1, 2 or 3) which would be 2 otherwize.
	virtual void initializeJITEngine(int optimizationLevel = 2) = 0;

	// By default, the compiled / JITed functions are using an internal LLVM Module.
	// You can set one if you want the module to be write-accessible.
	// ATTENTION: You can only set a module BEFORE any regular expression creation.
	virtual void setLLVMModule(llvm::Module * m) = 0;
	// Get the LLVM module on which the functions are written.
	virtual const llvm::Module * getLLVMModule() const = 0;

	// Proxy function that basically does *os << Module;
	virtual void WriteBitcodeToFile(llvm::raw_ostream * os) const = 0;

	// Get the default regular expression compiling policy.
	// If it has not been change, is LLVMRE::Func::JIT_AUTO
	virtual Func::Policy getDefaultPolicy() const = 0;
	// Set the default regular expression compiling policy for all the regulare expressions to be created.
	virtual void setDefaultPolicy(Func::Policy) = 0;
	// Return the default prefix for function names. If it has not been se, is "llvmre"
	virtual const std::string & getDefaultPrefix() = 0;
	// Set the default prefix for function names.
	virtual void setDefaultPrefix(const std::string &) = 0;
};

extern "C"
{
	// Get the LLVMRE Singleton Instance.
	__LLVMRE_Dll LLVMRE & LLVMRE_Instance();
}

#endif /* LLVM_RE_GEN_H_ */
