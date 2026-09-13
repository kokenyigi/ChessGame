file(GLOB_RECURSE SOURCES
    src/*.cpp
    GLAD/src/*.cpp
    stb_image/src/*.cpp
    Utils/src/*.cpp
)

add_executable(chessClient ${SOURCES})

add_subdirectory(chessClient GLM/glm)
add_subdirectory(chessClient GLFW3)



target_include_directories(chessClient PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/GLAD/include
    ${CMAKE_CURRENT_SOURCE_DIR}/stb_image/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils/include

)

target_link_libraries(chessClient PRIVATE
    chessLibrary
)