
include_directories ( ../../src/ )

add_definitions (
	-D_CXX
)

if ( WIN32 )
	add_definitions (
		-D_WIN
		-DLLVMRE_USE_WINDLL
	)
endif ()

add_executable ( Test_CXX TestMain.cpp )

target_link_libraries ( Test_CXX LLVMREGen )
