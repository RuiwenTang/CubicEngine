
add_executable(sandbox main.cc)

target_link_libraries(sandbox CubicEngine)

add_executable(shader_generator_test shader_generator_test.cc)

target_include_directories(shader_generator_test PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../Engine/src)
target_link_libraries(shader_generator_test CubicEngine)

add_executable(renderer_test renderer_test.cc)
target_link_libraries(renderer_test CubicEngine)