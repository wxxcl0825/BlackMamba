set(THIRD_PARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/thirdParty)

add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glfw)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/glfw/include)

option(USE_GLES "Use OpenGL ES" OFF)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glad)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/glad/include)

add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glm)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/glm)

include(${THIRD_PARTY_LIBRARY_PATH}/imgui.cmake)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/imgui)

include(${THIRD_PARTY_LIBRARY_PATH}/stb.cmake)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/stb)

add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/assimp)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/assimp/include)

add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/JoltPhysics/Build)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/JoltPhysics)

add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/openal)
include_directories(${THIRD_PARTY_LIBRARY_PATH}/openal/include)