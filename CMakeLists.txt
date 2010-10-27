
cmake_minimum_required (VERSION 2.8)
project (LLVMREGen)

if ( WIN32 )
	set ( LLVM_ROOT "C:\Program Files\LLVM" CACHE PATH "Root of LLVM install." )
else ()
	set ( LLVM_ROOT "/usr/" CACHE PATH "Root of LLVM install." )
endif ()

if ( NOT EXISTS ${LLVM_ROOT}/include/llvm )
	message ( FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install" )
endif ()
set ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_ROOT}/share/llvm/cmake" )
include ( LLVM )

add_subdirectory ( src )

option (
	BUILD_TESTS
	"Build VERY simple test, just to ensure that the library links"
	OFF
)

if ( BUILD_TESTS )
	add_subdirectory ( Test/C )
	add_subdirectory ( Test/CXX )
endif ()
