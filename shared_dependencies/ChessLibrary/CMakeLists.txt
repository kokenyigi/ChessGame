file(GLOB SOURCES src/*.cpp)

add_library(chessLibrary STATIC ${SOURCES})

target_include_directories(chessLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)