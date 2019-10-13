include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

function(CResourceSystemInit metafile)
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	ExternalProject_Add(
		CRS
		PREFIX "crs-toolchain"
		GIT_REPOSITORY "https://github.com/antony-jr/crs"
		CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_BINARY_DIR}/crs-toolchain/src/CRS/
		BUILD_COMMAND make -j$(nproc)
		INSTALL_COMMAND "" 
	)
	add_custom_target(
		CRSGeneration
		COMMAND ${CMAKE_CURRENT_BINARY_DIR}/crs-toolchain/src/CRS-build/crs ${metafile}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	set(CRSGeneration ${CRSGeneration} PARENT_SCOPE)
endfunction()
