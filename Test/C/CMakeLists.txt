
add_executable ( Test_C TestMain.c )

include_directories ( ../../src/ )

if ( WIN32 )
	add_definitions (
		-D_WIN
		-DLLVMRE_USE_WINDLL
	)
endif ()

target_link_libraries ( Test_C LLVMREGen )
