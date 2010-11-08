
include_directories ( BEFORE ../src/ tclap/ )

add_definitions (
	-D__STDC_LIMIT_MACROS
	-D__STDC_CONSTANT_MACROS
	-D_CXX
)

if ( WIN32 )
	add_definitions (
		-D_WIN
		-DLLVMRE_USE_WINDLL
	)
endif ()

if ( NOT MSVC )
	set (
		CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter"
	)
else ()
	set (
		CMAKE_CXX_FLAGS
		"${CMAKE_CXX_FLAGS} /wd4146 /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS"
	)
endif ()

file (
	GLOB_RECURSE
	source_files
	*
)

add_executable ( llvm-regexp ${source_files} )

llvm_map_components_to_libraries ( REQ_LLVM_LIBRARIES core bitwriter )

target_link_libraries ( llvm-regexp ${REQ_LLVM_LIBRARIES} LLVMREGen )

install (
	TARGETS llvm-regexp
	RUNTIME DESTINATION bin
)
