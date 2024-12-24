set(THIRD_PARTY_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/thirdParty)

add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glfw)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glad)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/glm)
include(${THIRD_PARTY_LIBRARY_PATH}/imgui.cmake)
include(${THIRD_PARTY_LIBRARY_PATH}/stb.cmake)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/assimp)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/JoltPhysics/Build)

set(LIBTYPE STATIC)
add_subdirectory(${THIRD_PARTY_LIBRARY_PATH}/openal)