
include_directories ( BEFORE ../../src/ )

add_executable ( Test_Devel TestMain.cpp ${source_files} )

llvm_map_components_to_libraries ( REQ_LLVM_LIBRARIES bitwriter core bitwriter interpreter jit native target )
target_link_libraries ( Test_Devel ${REQ_LLVM_LIBRARIES} )

if ( UNIX )
	target_link_libraries ( Test_Devel dl pthread )
endif ()
