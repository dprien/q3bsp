cmake_minimum_required(VERSION 2.6) 

project(q3bsp)    
include_directories(${q3bsp_SOURCE_DIR}/../)

set(CMAKE_CXX_FLAGS "-std=c++14 -pipe -fstrict-aliasing -fno-common -O3 -g -Wall -Wextra -Wpedantic -Wformat=2 -Wformat-nonliteral -Wshadow -Wcast-qual -Wcast-align -Wstrict-aliasing=1")

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

pkg_search_module(SDL2_image REQUIRED SDL2_image)
include_directories(${SDL2_image_INCLUDE_DIRS})

pkg_search_module(LIBZIP REQUIRED libzip)
include_directories(${LIBZIP_INCLUDE_DIRS})

find_package(Boost 1.55.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenGL COMPONENTS GL GLU REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})


add_executable(q3bsp
    archive.cc
    binio.cc
    bsp.cc
    image.cc
    main.cc
    tex.cc
    time.cc
)

target_link_libraries(q3bsp ${Boost_SYSTEM_LIBRARY})
target_link_libraries(q3bsp ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(q3bsp ${OPENGL_LIBRARIES})
target_link_libraries(q3bsp ${SDL2_LIBRARIES})
target_link_libraries(q3bsp ${SDL2_image_LIBRARIES})
target_link_libraries(q3bsp ${LIBZIP_LIBRARIES})

target_link_libraries(q3bsp)
