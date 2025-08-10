cmake_minimum_required(VERSION 3.0)

project(engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(ENGINE_SRCS Engine/main.cpp Engine/Core/Engine.cpp)

add_executable(${PROJECT_NAME} ${ENGINE_SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE glfw vulkan dl pthread)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:ENGINE_DEBUG=1>
    $<$<CONFIG:Release>:ENGINE_DEBUG=0>
)