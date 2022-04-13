set(SOURCE main/main.cpp ui/Window.cpp ui/uitilemap.cpp specials/Box2.cpp connection/connection.cpp specials/specials.cpp chess/chess.cpp ui/sprites.cpp ui/uibasic.cpp ui/uibutton.cpp ui/uigroups.cpp)
set(PROJECT_NAME multichess)

cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-network)