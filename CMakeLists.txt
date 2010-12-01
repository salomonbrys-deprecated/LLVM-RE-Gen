
cmake_minimum_required (VERSION 2.8)
project (LLVMREGen)



if ( WIN32 )
	if ( EXISTS "C:\\Program Files (x86)\\LLVM" )
		set ( LLVM_ROOT "C:\\Program Files (x86)\\LLVM" CACHE PATH "Root of LLVM install." )
	else ()
	    set ( LLVM_ROOT "C:\\Program Files\\LLVM" CACHE PATH "Root of LLVM install." )
	endif ()
else ()
	if ( EXISTS /usr/local/include/llvm )
		set ( LLVM_ROOT "/usr/local" CACHE PATH "Root of LLVM install." )
	else ()
		set ( LLVM_ROOT "/usr/" CACHE PATH "Root of LLVM install." )
	endif ()
endif ()

if ( NOT EXISTS ${LLVM_ROOT}/include/llvm OR NOT EXISTS ${LLVM_ROOT}/lib )
	message ( FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install, please set it to a valid LLVM install directory !" )
endif ()

if ( NOT EXISTS ${LLVM_ROOT}/share/llvm/cmake )
	message ( FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM CMAKE install: cannot find the LLVM CMAKE module directory !" )
endif ()

set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_ROOT}/share/llvm/cmake" )
include ( LLVM )

include_directories ( AFTER ${LLVM_ROOT}/include )
link_directories ( ${LLVM_ROOT}/lib )



option (
	LLVMREGEN_BUILD_SHARED_LIBRARY
	"Build and install LLVMREGen as a shared library"
	ON
)

option (
	LLVMREGEN_BUILD_STATIC_LIBRARY
	"Build and install LLVMREGen as a static library"
	OFF
)

set ( LLVMREGEN_BUILD_WITH LLVMREGen )
if ( NOT LLVMREGEN_BUILD_SHARED_LIBRARY )
	if ( NOT LLVMREGEN_BUILD_STATIC_LIBRARY )
		message ( FATAL_ERROR "At least one of LLVMREGEN_BUILD_SHARED_LIBRARY or LLVMREGEN_BUILD_STATIC_LIBRARY must be on !" )
	endif ()
	set ( LLVMREGEN_BUILD_WITH LLVMREGen-s )
endif ()



option (
	LLVMREGEN_BUILD_TESTS
	"Build VERY simple test, just to ensure that the library links"
	OFF
)



add_subdirectory ( src )
add_subdirectory ( Comp )



if ( LLVMREGEN_BUILD_TESTS )
	add_subdirectory ( Test/CXX )
endif ()
