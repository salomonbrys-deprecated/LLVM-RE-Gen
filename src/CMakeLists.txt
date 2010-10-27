
file (
	GLOB_RECURSE
	source_files
	*
)

add_library (
	LLVMREGen
	SHARED
	${source_files}
)

add_definitions (
	-D__STDC_LIMIT_MACROS
	-D__STDC_CONSTANT_MACROS
	-D_CXX
)

if (WIN32)
	add_definitions (
		-D_WINDLL
		-D_WIN
	)
endif ()

set (
	CMAKE_CXX_FLAGS
	"${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter"
)

include_directories ( ${LLVM_ROOT}/include )
link_directories ( ${LLVM_ROOT}/lib )
llvm_map_components_to_libraries ( REQ_LLVM_LIBRARIES bitwriter core interpreter jit native target )

target_link_libraries ( LLVMREGen ${REQ_LLVM_LIBRARIES} )

if (UNIX)
	target_link_libraries ( LLVMREGen dl pthread )
endif ()

install (
	TARGETS LLVMREGen
	DESTINATION lib
)
install (
	FILES LLVMREGen-c.h  LLVMREGen-c-llvm.h  LLVMREGen.h
	DESTINATION include
)
