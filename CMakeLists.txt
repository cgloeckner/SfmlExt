cmake_minimum_required (VERSION 2.8)
project (sfmlext)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall")

# Include headers
include_directories(include)

# Add source
set(SFMLEXT_SRC
	src/logger.cpp
	src/menu.cpp
	src/fader.cpp
)

# Specify library settings
set(SFMLEXT_LIB ${PROJECT_NAME})
set(LIBRARY_OUTPUT_PATH lib)

add_library(${SFMLEXT_LIB} SHARED ${SFMLEXT_SRC})
