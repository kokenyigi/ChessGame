file(GLOB SOURCES src/*.cpp)

add_library(chessNetworkingProtocol STATIC ${SOURCES})

target_include_directories(chessNetworkingProtocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
