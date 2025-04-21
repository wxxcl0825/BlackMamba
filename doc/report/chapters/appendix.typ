= Appendix: 环境配置

#import "@preview/codly:1.1.1": *
#import "@preview/codly-languages:0.1.3": *

#let H(
  title: none
) = {
  codly(
    languages: codly-languages,
    header: [*#title*],
    header-cell-args: (align: center)
    )
}

使用CMake进行管理，项目结构如下

#codly(number-format: none)

```
.
├── CMakeLists.txt
├── common
│   └── checkError.cpp
├── game
│   ├── entity
│   │   ├── camera.cpp
│   │   ├── player.cpp
│   │   ├── skybox.cpp
│   │   └── terrain.cpp
│   ├── game.cpp
│   ├── material
│   │   ├── explosionMaterial.cpp
│   │   ├── phongMaterial.cpp
│   │   ├── skyboxMaterial.cpp
│   │   ├── terrainMaterial.cpp
│   │   └── transparentMaterial.cpp
│   └── utils
│       └── utils.cpp
├── include
│   ├── common
│   │   ├── common.h
│   │   └── macro.h
│   ├── game
│   │   ├── entity
│   │   │   ├── camera.h
│   │   │   ├── player.h
│   │   │   ├── skybox.h
│   │   │   └── terrain.h
│   │   ├── game.h
│   │   ├── material
│   │   │   ├── explosionMaterial.h
│   │   │   ├── phongMaterial.h
│   │   │   ├── skyboxMaterial.h
│   │   │   ├── terrainMaterial.h
│   │   │   └── transparentMaterial.h
│   │   └── utils
│   │       └── utils.h
│   └── runtime
│       ├── engine.h
│       ├── framework
│       │   ├── component
│       │   │   ├── audio
│       │   │   │   └── audio.h
│       │   │   ├── camera
│       │   │   │   └── camera.h
│       │   │   ├── component.h
│       │   │   ├── light
│       │   │   │   └── light.h
│       │   │   ├── mesh
│       │   │   │   └── mesh.h
│       │   │   ├── physics
│       │   │   │   └── rigidBody.h
│       │   │   ├── terrain
│       │   │   │   └── terrain.h
│       │   │   └── transform
│       │   │       └── transform.h
│       │   ├── object
│       │   │   └── gameObject.h
│       │   └── system
│       │       ├── audioSystem.h
│       │       ├── jolt
│       │       │   └── utils.h
│       │       ├── physicalSystem.h
│       │       ├── renderSystem.h
│       │       └── windowSystem.h
│       └── resource
│           ├── audio
│           │   └── audio.h
│           ├── geometry
│           │   └── geometry.h
│           ├── material
│           │   ├── material.h
│           │   └── whiteMaterial.h
│           ├── resourceManager.h
│           ├── shader
│           │   └── shader.h
│           └── texture
│               └── texture.h
├── main.cpp
└── runtime
    ├── engine.cpp
    ├── framework
    │   ├── component
    │   │   ├── audio
    │   │   │   └── audio.cpp
    │   │   ├── camera
    │   │   │   └── camera.cpp
    │   │   ├── physics
    │   │   │   └── rigidBody.cpp
    │   │   └── transform
    │   │       └── transform.cpp
    │   ├── object
    │   │   └── gameObject.cpp
    │   └── system
    │       ├── audioSystem.cpp
    │       ├── jolt
    │       │   └── utils.cpp
    │       ├── physicalSystem.cpp
    │       ├── renderSystem.cpp
    │       └── windowSystem.cpp
    └── resource
        ├── audio
        │   └── audio.cpp
        ├── geometry
        │   └── geometry.cpp
        ├── material
        │   └── whiteMaterial.cpp
        ├── resourceManager.cpp
        ├── shader
        │   └── shader.cpp
        └── texture
            └── texture.cpp
```

`CMakeLists`内容如下

#H(title: "CMakeLists.txt")

```cmake
cmake_minimum_required(VERSION 3.12)

project(BlackMamba)

set(CMAKE_CXX_STANDARD 17)
set(BUILD_SHARED_LIBS OFF)

file(GLOB ASSETS "./assets")
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR})

add_definitions(-DDEBUG)
add_compile_options(-Wno-unknown-pragmas)

add_subdirectory(thirdParty)
add_subdirectory(src)
```


#H(title: "src/CMakeLists.txt")

```cmake
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

target_link_libraries(main
  glfw
  glad
  glm
  imgui
  stb
  assimp
  Jolt
  OpenAL
)
```

#H(title: "thirdParty/CMakeLists.txt")

```cmake
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
```

