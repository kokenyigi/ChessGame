file(GLOB_RECURSE SOURCES src./*cpp)

add_executable(chessServer ${SOURCES})

target_include_directories(chessServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(chessServer PRIVATE chessLibrary)