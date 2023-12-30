
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
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/render_system.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/cubic.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/platform.h

	# sources
	${CMAKE_CURRENT_LIST_DIR}/src/core/application.cc
	${CMAKE_CURRENT_LIST_DIR}/src/core/log.cc
	${CMAKE_CURRENT_LIST_DIR}/src/render/render_system_priv.cc
	${CMAKE_CURRENT_LIST_DIR}/src/render/render_system_priv.h
)

if (APPLE)
	# only apple use Metal
else()
	target_sources(CubicEngine
		PRIVATE

		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_system_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_system_vk.h
	)

	if (WIN32)
		target_compile_definitions(CubicEngine PRIVATE VK_USE_PLATFORM_WIN32_KHR=1)
		find_package(Vulkan REQUIRED)

		target_link_libraries(CubicEngine PRIVATE Vulkan::Vulkan)
	endif()
	# volk
	find_package(volk CONFIG REQUIRED)

	target_link_directories(CubicEngine PRIVATE volk::volk)
endif()

# spdlog
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(CubicEngine PUBLIC spdlog::spdlog)

