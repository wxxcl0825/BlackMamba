file(GLOB_RECURSE MAIN_SOURCES ./ *.cpp)

set(SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
add_executable(main ${MAIN_SOURCES})

target_include_directories(main PUBLIC
  ${SRC_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/thirdParty/glfw/include
  ${CMAKE_SOURCE_DIR}/thirdParty/glad/include
  ${CMAKE_SOURCE_DIR}/thirdParty/glm
  ${CMAKE_SOURCE_DIR}/thirdParty/imgui
  ${CMAKE_SOURCE_DIR}/thirdParty/stb
  ${CMAKE_SOURCE_DIR}/thirdParty/assimp/include
  ${CMAKE_SOURCE_DIR}/thirdParty/JoltPhysics
  ${CMAKE_SOURCE_DIR}/thirdParty/openal/include
)

find_package(OpenAL REQUIRED)

target_link_libraries(main
  glfw
  glad
  glm
  imgui
  stb
  assimp
  Jolt
  OpenAL::OpenAL
)