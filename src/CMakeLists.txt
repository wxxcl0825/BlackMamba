file(GLOB_RECURSE MAIN_SOURCES ./ *.cpp)

set(SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
include_directories(${SRC_INCLUDE_DIR})

add_executable(main ${MAIN_SOURCES})
target_link_libraries(main glfw glad glm imgui stb assimp)