
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
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/core/window.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/graphic/geometry.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/graphic/material/color_material.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/graphic/material.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/graphic/mesh.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/graphic/scene.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/graphic/transform.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/bind_group.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/buffer.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/command_buffer.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/command_queue.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/pipeline_layout.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/render_pass.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/render_pipeline.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/render_system.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/renderer.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/shader_module.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/texture.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/render/types.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/cubic.h
	${CMAKE_CURRENT_LIST_DIR}/include/cubic/platform.h

	# sources
	${CMAKE_CURRENT_LIST_DIR}/src/core/application.cc
	${CMAKE_CURRENT_LIST_DIR}/src/core/log.cc
	${CMAKE_CURRENT_LIST_DIR}/src/core/window_impl.cc
	${CMAKE_CURRENT_LIST_DIR}/src/core/window_impl.h
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/material/color_material.cc
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/mesh.cc
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/plane_geometry.cc
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/scene.cc
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/shader_generator.cc
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/shader_generator.h
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/stage_buffer.cc
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/stage_buffer.h
	${CMAKE_CURRENT_LIST_DIR}/src/graphic/transform.cc
	${CMAKE_CURRENT_LIST_DIR}/src/render/render_system_priv.cc
	${CMAKE_CURRENT_LIST_DIR}/src/render/render_system_priv.h
	${CMAKE_CURRENT_LIST_DIR}/src/render/renderer.cc
	${CMAKE_CURRENT_LIST_DIR}/src/render/shader_compiler.cc
	${CMAKE_CURRENT_LIST_DIR}/src/render/shader_compiler.h
	${CMAKE_CURRENT_LIST_DIR}/src/render/types.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/attribute.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/attribute.h
	${CMAKE_CURRENT_LIST_DIR}/src/slang/function.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/function.h
	${CMAKE_CURRENT_LIST_DIR}/src/slang/node.h
	${CMAKE_CURRENT_LIST_DIR}/src/slang/operation.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/operation.h
	${CMAKE_CURRENT_LIST_DIR}/src/slang/program.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/program.h
	${CMAKE_CURRENT_LIST_DIR}/src/slang/statement.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/statement.h
	${CMAKE_CURRENT_LIST_DIR}/src/slang/types.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/types.h
	${CMAKE_CURRENT_LIST_DIR}/src/slang/uniform.cc
	${CMAKE_CURRENT_LIST_DIR}/src/slang/uniform.h
)

if(APPLE)
	# only apple use Metal
	target_sources(CubicEngine
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/src/core/mtl/window_impl_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/core/mtl/window_impl_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/bind_group_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/bind_group_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/buffer_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/buffer_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/command_buffer_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/command_buffer_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/command_queue_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/command_queue_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/mtl_types.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/mtl_types.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/render_pass_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/render_pass_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/render_pipeline_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/render_pipeline_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/render_system_info_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/render_system_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/render_system_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/shader_module_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/shader_module_mtl.mm
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/texture_mtl.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/mtl/texture_mtl.mm
	)

	target_link_libraries(
		CubicEngine
		PRIVATE
		"-framework Metal"
		"-framework QuartzCore"
	)
else()
	target_sources(CubicEngine
		PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/src/core/vk/swap_chain.cc
		${CMAKE_CURRENT_LIST_DIR}/src/core/vk/swap_chain.h
		${CMAKE_CURRENT_LIST_DIR}/src/core/vk/window_impl_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/core/vk/window_impl_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/util/attachment_builder.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/util/attachment_builder.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/bind_group_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/bind_group_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/buffer_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/buffer_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/command_buffer_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/command_buffer_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/command_queue_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/command_queue_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/pipeline_layout_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/pipeline_layout_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_pass_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_pass_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_pipeline_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_pipeline_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_system_info_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_system_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/render_system_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/shader_module_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/shader_module_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/texture_vk.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/texture_vk.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/vulkan_device.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/vulkan_device.h
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/vulkan_types.cc
		${CMAKE_CURRENT_LIST_DIR}/src/render/vk/vulkan_types.h
	)

	# volk
	find_package(volk CONFIG REQUIRED)

	target_link_libraries(CubicEngine PRIVATE volk::volk)

	# vma
	find_package(VulkanMemoryAllocator CONFIG REQUIRED)
	target_link_libraries(CubicEngine PRIVATE GPUOpen::VulkanMemoryAllocator)

	target_compile_definitions(CubicEngine PRIVATE VK_NO_PROTOTYPES)

	if(WIN32)
		target_compile_definitions(CubicEngine PRIVATE VK_USE_PLATFORM_WIN32_KHR=1)
	endif()
endif()

# spdlog
find_package(spdlog CONFIG REQUIRED)

target_link_libraries(CubicEngine PUBLIC spdlog::spdlog)

# glfw
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(CubicEngine PRIVATE glfw)

# shaderc
find_package(unofficial-shaderc CONFIG REQUIRED)
target_link_libraries(CubicEngine PRIVATE unofficial::shaderc::shaderc)

# glm
find_package(glm CONFIG REQUIRED)
target_link_libraries(CubicEngine PUBLIC glm::glm)

# spirv-cross for Metal
if(APPLE)
	find_package(spirv_cross_core CONFIG REQUIRED)
	find_package(spirv_cross_glsl CONFIG REQUIRED)
	find_package(spirv_cross_cpp CONFIG REQUIRED)
	find_package(spirv_cross_msl CONFIG REQUIRED)

	target_link_libraries(CubicEngine PRIVATE spirv-cross-core spirv-cross-cpp spirv-cross-glsl spirv-cross-msl)
endif()
