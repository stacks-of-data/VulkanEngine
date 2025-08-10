cmake_minimum_required(VERSION 3.0)

project(VulkanEngine)

set(ENGINE_SRCS Engine/main.cpp Engine/Core/Engine.cpp)

add_subdirectory(glfw glfw_build)
add_compile_options(-Wall -Wextra -Werror -O3 -g)
add_executable(${PROJECT_NAME} ${ENGINE_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw -lvulkan -ldl -lpthread)