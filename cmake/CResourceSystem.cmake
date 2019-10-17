include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

function(CResourceSystemInit metafilePath prefixPath)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	ExternalProject_Add(
		CRSToolchain
		PREFIX "CRSToolchain"
		GIT_REPOSITORY "https://github.com/antony-jr/crs"
		CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/CRSToolchain/src/CRSToolchain/
		BUILD_COMMAND make -j$(nproc)
		INSTALL_COMMAND "" 
	)
	add_custom_target(
		CRSGeneration
		ALL
	        DEPENDS	crs.c crs.h
	)
        set(CRSGeneration ${CRSGeneration} PARENT_SCOPE)
	add_custom_command(
		OUTPUT crs.c crs.h
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/CRSToolchain/src/CRSToolchain-build/crs 
		ARGS ${metafilePath} -p ${prefixPath}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		DEPENDS CRSToolchain
	)	
endfunction()
