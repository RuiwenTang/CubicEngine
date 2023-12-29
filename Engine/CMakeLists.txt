
add_library(CubicEngine SHARED)

target_include_directories(
	CubicEngine
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

target_include_directories(
	CubicEngine
	PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src
)

target_sources(
	CubicEngine
	PRIVATE
	
	# headers
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/core/application.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/core/log.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/cubic.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/platform.h

	# sources
	${CMAKE_CURRENT_LIST_DIR}/src/core/application.cc
	${CMAKE_CURRENT_LIST_DIR}/src/core/log.cc
)

# spdlog
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(CubicEngine PUBLIC spdlog::spdlog)

