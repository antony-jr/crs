CMAKE_MINIMUM_REQUIRED( VERSION 2.8)
project(crs)

#set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#include(CResourceSystem)

# Let cmake know that this is a release build.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# add git commit to bulid
if(GIT_COMMIT_STR)
	add_definitions(-DGIT_COMMIT_STR="${GIT_COMMIT_STR}")
endif()

add_definitions(-DJSON_TRACK_SOURCE) # tell json-parser to track the line from json files.

# Include Directories.
include_directories(include)

add_executable(crs include/cutelog.h
	           include/cstr.h
	           include/cstr_list.h	
	           include/char2hex.h
		   include/metafile.h
		   include/json.h
		   include/src_packer.h
		   src/src_packer.c
		   src/metafile.c
		   src/json.c
		   src/cutelog.c
	           src/cstr.c
		   src/cstr_list.c
		   src/char2hex.c
		   src/main.c)
target_link_libraries(crs PUBLIC m)
target_include_directories(crs PUBLIC . include)

#CResourceSystemInit(${CMAKE_SOURCE_DIR} ${PREFIX_PATH})
#add_dependencies(crs CRSGeneration)

install(
	TARGETS crs
	RUNTIME DESTINATION bin
)
