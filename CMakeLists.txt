set(SOURCE main/main.cpp ui/window.cpp
        ui/uitilemap.cpp utils/Box2.cpp connection/connection.cpp
        utils/utils.cpp chess/chess.cpp ui/sprites.cpp ui/uibasic.cpp
        ui/uibutton.cpp ui/uigroups.cpp ui/formparser.cpp ui/appmanager.cpp pinboard/pinboard.h)

set(PROJECT_NAME multichess)

cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wextra -Wall -flto")

if(CMAKE_BUILD_TYPE MATCHES Release)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-network)

#file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
#configure_file(resources COPYONLY)
#add_custom_command(
#        TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy_directory
#        ${CMAKE_CURRENT_SOURCE_DIR}/resources
#        ${CMAKE_CURRENT_BINARY_DIR}/resources
#)

add_custom_target(copy-files ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_BINARY_DIR}/resources
)
add_dependencies(${PROJECT_NAME} copy-files)
