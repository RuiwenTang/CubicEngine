cmake_minimum_required(VERSION 3.20)

project(CubicEngine)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
	set(BUILD_SHARED_LIBS TRUE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_subdirectory(Engine)

add_subdirectory(Sandbox)
