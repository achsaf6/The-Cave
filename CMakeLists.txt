cmake_minimum_required(VERSION 3.22)
project(The_Cave)

set(CMAKE_CXX_STANDARD 14)


include_directories(${CMAKE_CURRENT_LIST_DIR}/Classes)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()

add_executable(The_Cave
        main.cpp
        Classes/Face.h
        Classes/Face.cpp
        Classes/Model.h
        Classes/Model.cpp
        Classes/Camera.h
        Classes/Camera.cpp
        Classes/Canvas.h
        Classes/Canvas.cpp
        )
