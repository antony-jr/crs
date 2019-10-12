CMAKE_MINIMUM_REQUIRED( VERSION 2.8)
project(crs)

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

# Include Directories.
include_directories(include)

add_executable(crs include/char2hex.h
	           src/char2hex.c
		   src/main.c)
target_include_directories(crs PUBLIC . include)
install(
	TARGETS crs
	RUNTIME DESTINATION bin
)
