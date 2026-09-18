file(GLOB_RECURSE SOURCES
    src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/My2DRenderer/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/MyGlAbstraction/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/MyGui/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/MyUtils/src/*.cpp
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# make an .exe in build
add_executable(chessClient ${SOURCES})

# "recursively" go into these folder's and their associated CMAKE files and use them from building
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLM/glm)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLFW3)

# Create a static lib from stb_image, but make sure that its headers are included
add_library(stb_image STATIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb_image/src/stb_image.cpp)
target_include_directories(stb_image PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/stb_image/include
)

# Same, create a static lib out of glad loader
add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLAD/src/glad.c)
target_include_directories(glad PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLAD/include
)

# add the headers incldue paths of my own source code to the project.
target_include_directories(chessClient PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/My2DRenderer/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/MyGlAbstraction/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/MyGui/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/MyUtils/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/GLM
)

# we need opengl from somewhere -> find it -> link it
find_package(OpenGL REQUIRED)

target_link_libraries(chessClient PRIVATE
    chessLibrary
    glfw
    glm::glm
    OpenGL::GL
    glad
    stb_image
)

# just a build time copying of the assets folder(we need it at a different location)
add_custom_command(
    TARGET chessClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:chessClient>/assets"
)