cmake_minimum_required(VERSION 3.25)
project(Aquarium__)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES "simulation/*.cpp")
include_directories(simulation)

add_executable(Aquarium__ main.cpp)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)

target_link_libraries(${CMAKE_PROJECT_NAME}
                      PRIVATE
                      $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
                      $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
                      )
target_link_libraries(${CMAKE_PROJECT_NAME}
                      PRIVATE
                      $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
                      )