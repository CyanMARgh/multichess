set(SOURCE main.cpp window.cpp uielements.cpp box2.cpp chess.h connection.cpp specials.cpp chess.cpp sprites.cpp)
set(PROJECT_NAME multichess)

cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-network)