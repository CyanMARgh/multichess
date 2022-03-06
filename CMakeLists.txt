set(SOURCE main.cpp window.cpp uielements.cpp box2.cpp chess.h scenes.h)
set(PROJECT_NAME multichess)

cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

#SET(GCC_COVERAGE_COMPILE_FLAGS "-no-pie")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

#set(SFML_DIR "/home/cyanmargh/SFML-compiled")
#set(CMAKE_MODULE_PATH "/home/cyanmargh/SFML-compiled" ${CMAKE_MODULE_PATH})
#set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} sfml-graphics)