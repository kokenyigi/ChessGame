cmake_minimum_required(VERSION 3.16)
project(MyChessProject)

option(BUILD_CLIENT "Build the client application" ON)
option(BUILD_SERVER "Build the server application" ON)

add_subdirectory(shared_dependencies/ChessLibrary)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_SERVER)
    add_subdirectory(server)
endif()