cmake_minimum_required(VERSION 2.9)

project(VulkanEngine)

set(ENGINE_SRCS Engine/main.cpp Engine/Core/Engine.cpp)

add_compile_options(-Wall -Wextra -Werror -O3)
add_executable(${PROJECT_NAME} ${ENGINE_SRCS})
# target_link_libraries(${PROJECT_NAME} glfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi)
target_link_libraries(${PROJECT_NAME} glfw -lvulkan -ldl -lpthread)