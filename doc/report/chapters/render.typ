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

#let __icon(image-filename) = {
  box(
    image(image-filename, height: 0.9em),
    baseline: 0.05em,
    inset: 0pt,
    outset: 0pt,
  ) + h(0.3em)
}

#let SH(
  title: none
) = {
  codly(
    languages: (
      vert: (
        name: [vert], color: purple, icon: __icon("../images/shader.svg")
      ),
      frag: (
        name: [frag], color: purple, icon: __icon("../images/shader.svg")
      ),
      geom: (
        name: [geom], color: purple, icon: __icon("../images/shader.svg")
      ),
      tesc: (
        name: [tesc], color: purple, icon: __icon("../images/shader.svg")
      ),
      tese: (
        name: [tese], color: purple, icon: __icon("../images/shader.svg")
      )
    ),
    header: [*#title*],
    header-cell-args: (align: center)
  )
}

= 渲染系统

== 资源管理

由于游戏中的资源种类繁多，且存在大量重复与需要写入GPU的资源(这些资源需要合理地从GPU上释放)，因此需要一个专门的系统———资源管理系统来管理这些资源。资源管理系统的主要功能是加载、管理和释放资源，以及提供资源的查询接口，其内部实现是很多个资源池，按照资源类型进行分类，每种资源类型都有一个对应的资源池。

资源管理系统声明如下

#H(title: "include/runtime/resource/resourceManager.h")

```cpp
class ResourceManager {
  friend class Engine;

public:
  ~ResourceManager();

  Texture *loadTexture(const std::string &filePath);
  Texture *loadTexture(const std::vector<std::string> &filePaths);
  Texture *loadTexture(const std::string &filePath, unsigned char *dataIn,
                       uint32_t widthIn, uint32_t heightIn);

  Shader *loadShader(const std::string &vertexPath,
                     const std::string &fragmentPath);
  Shader *loadShader(const std::string &vertexPath,
                     const std::string &geometryPath,
                     const std::string &fragmentPath);
  Shader *loadShader(const std::string &vertexPath,
                     const std::string &tessCtrlPath,
                     const std::string &tessEvalPath,
                     const std::string &fragmentPath);

  Geometry *createBoxGeometry(float size);
  Geometry *createPlaneGeometry(float width, float height);

  Geometry *loadGeometry(const std::vector<glm::vec3> &vertices,
                         const std::vector<glm::vec2> &uvs,
                         const std::vector<glm::vec3> &normals,
                         const std::vector<unsigned int> &indices);
  Geometry *loadGeometry(const std::vector<glm::vec3> &vertices,
                         const std::vector<glm::vec2> &uvs);

  Audio *loadAudio(const std::string &filePath);

  void pauseAllAudios();
  void startAllAudios();

private:
  std::unordered_map<std::string, Texture *> _textureMap{};
  std::unordered_map<std::string, Shader *> _shaderMap{};
  std::unordered_map<std::string, Audio *> _audioMap{};

  std::vector<Geometry *> _geometryList{};

  ResourceManager() = default;
};
```
- 资源管理系统负责纹理、着色器、顶点(几何)数据和音频数据的管理(音频数据将在音效系统进行介绍)
- 资源池使用字符串哈希实现，以资源路径作为索引构建哈希表，保证同一资产不会发生重复导入，从而降低内存占用
- 对各类数据对象的管理，通过私有数据对象的构造函数实现；各类数据对象声明对`ResourceManager`的友元关系，从而在资源管理器中实现统一创建与销毁

资源统一销毁实现如下

#H(title: "runtime/resource/resourceManager.cpp")

```cpp
ResourceManager::~ResourceManager() {
  for (auto it = _textureMap.begin(); it != _textureMap.end(); it++)
    delete it->second;
  _textureMap.clear();
  for (auto it = _shaderMap.begin(); it != _shaderMap.end(); it++)
    delete it->second;
  _shaderMap.clear();
  for (auto it = _geometryList.begin(); it != _geometryList.end(); it++)
    delete *it;
  _geometryList.clear();
  for (auto it = _audioMap.begin(); it != _audioMap.end(); it++)
    delete it->second;
  _audioMap.clear();
}
```

由于在4.3版本的OpenGL以前，并不提供错误回调，需要在每次调用OpenGL API时手动检错，实现如下

#H(title: "include/common/macro.h")

```cpp
#define Log(format, ...)                                \
    printf("\33[1;35m[%s,%d,%s] " format "\33[0m\n",    \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__)

#define Err(format, ...) {                              \
    printf("\33[1;31m[%s,%d,%s] " format "\33[0m\n",    \
        __FILE__, __LINE__, __func__, ## __VA_ARGS__);  \
    while(1);                                           \
}

void checkGLError(const char *file, int line, const char *func);

#ifdef DEBUG
#define GL_CALL(func)                                                        \
  func;                                                                      \
  checkGLError(__FILE__, __LINE__, __func__);
#else
#define GL_CALL(func) func;
#endif
```

#H(title: "common/checkError.cpp")

```cpp
void checkGLError(const char *file, int line, const char *func) {
  GLenum errorCode = glGetError();
  std::string error = "";
  if (errorCode != GL_NO_ERROR) {
    switch (errorCode) {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    default:
      error = "UNKNOWN";
      break;
    }
    Err("OpenGL error: %s, file: %s, line: %d, func: %s", error.c_str(), file,
        line, func);
  }
}
```

渲染过程中涉及的如下三种资源：顶点数据、纹理数据与着色器。

=== 纹理资源

纹理资源是游戏中最常用的资源之一，用于渲染模型的表面，其在渲染时会绑定到一个纹理单元上参与运算，其主要属性有宽度，高度，纹理单元序号等，其定义如下：

#H(title: "include/runtime/resource/texture/texture.h")

```cpp
class Texture {
    friend class ResourceManager;

public:
    int getWidth() const { return _width; };
    int getHeight() const { return _height; };
    GLuint getTextureID() const { return _textureID; };
    unsigned int getUnit() const { return _unit; };

    void setUnit(unsigned int unit) { _unit = unit; };
    void bind() const;

private:
    int _width{0};
    int _height{0};
    GLuint _textureID{0};
    unsigned int _unit{0};
    unsigned int _textureTarget{GL_TEXTURE_2D};

    Texture(const std::string& path, unsigned int unit);
    Texture(const std::vector<std::string>& paths, unsigned int unit);
    Texture(unsigned char* dataIn, uint32_t widthIn, uint32_t heightIn, unsigned int unit);
    ~Texture();

};
```

其在初始化时，通过传入纹理的路径或者数据，来创建一个纹理对象，其内部会调用OpenGL的API来生成纹理对象与Mipmap，并将纹理数据写入GPU中。在渲染时，通过调用bind函数来绑定纹理到指定的纹理单元上。这些实现代码如下：

#H(title: "runtime/resource/texture/texture.cpp")

```cpp
void Texture::bind() const{
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _unit));
    GL_CALL(glBindTexture(_textureTarget, _textureID));
}

Texture::Texture(const std::string& path, unsigned int unit) : _unit(unit){

    int _nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &_width, &_height, &_nrChannels, STBI_rgb_alpha);

    if(!data){
        Log("Failed to load texture: %s", path.c_str());
        stbi_image_free(data);
    }

    GL_CALL(glGenTextures(1, &_textureID));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _textureID));

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

}

// skybox: right left top bottom back front
Texture::Texture(const std::vector<std::string>& paths, unsigned int unit) : _unit(unit){
    _textureTarget = GL_TEXTURE_CUBE_MAP;

    stbi_set_flip_vertically_on_load(false);

    GL_CALL(glGenTextures(1, &_textureID));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _unit));
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID));

    int width, height, nrChannels;
    for (unsigned int i = 0; i < paths.size(); i++){
        unsigned char *data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
        if (data){
            GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
            stbi_image_free(data);
        } else {
            Log("Failed to load texture: %s", paths[i].c_str());
            stbi_image_free(data);
        }
    }

    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_R, GL_REPEAT));

    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
}

Texture::Texture(unsigned char* dataIn, uint32_t widthIn, uint32_t heightIn, unsigned int unit) : _unit(unit), _width(widthIn), _height(heightIn){
    int _nrChannels;
    stbi_set_flip_vertically_on_load(true);

    uint32_t dataInSize = !heightIn ? widthIn : (widthIn * heightIn * 4);
    unsigned char *data = stbi_load_from_memory(dataIn, dataInSize, &_width, &_height, &_nrChannels, STBI_rgb_alpha);

    if(!data){
        Err("Failed to load texture from memory");
        stbi_image_free(data);
    }

    GL_CALL(glGenTextures(1, &_textureID));
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _textureID));

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
```
- 使用stb/image@stb 读取图片数据
- 对于Cubemap, 需按照右、左、上、下、后、前的顺序载入6张贴图
- 为了防止远处贴图出现明显失真，使用Mipmap构建多级采样，并设置纹理过滤类型为`GL_NEAREST_MIPMAP_LINEAR`，在相邻Mipmap层级间进行插值

=== 顶点数据

几何体资源是游戏中的另一种常用资源，用于渲染模型的形状，其在渲染时会将所需数据按照对应格式传入GPU参与运算，其主要属性有VAO（储存数据格式），VBO（储存顶点, uv, 法线等信息, 供着色器以attribute数据形式进行调用）和EBO（顶点索引信息）等，其声明如下：

#H(title: "include/runtime/resource/geometry/geometry.h")

```cpp
class Geometry {
  friend class ResourceManager;

public:
  GLuint getVao() const { return _vao; }
  GLuint getPosVbo() const { return _posVbo; }
  GLuint getUvVbo() const { return _uvVbo; }
  GLuint getNormalVbo() const { return _normalVbo; }
  GLuint getEbo() const { return _ebo; }
  uint32_t getNumIndices() const { return _numIndices; }

private:
  GLuint _vao{0};
  GLuint _posVbo{0};
  GLuint _uvVbo{0};
  GLuint _normalVbo{0};
  GLuint _ebo{0};

  uint32_t _numIndices{0};

  Geometry();
  Geometry(const std::vector<glm::vec3> &vertices,
           const std::vector<glm::vec2> &uvs,
           const std::vector<glm::vec3> &normals,
           const std::vector<unsigned int> &indices);
  Geometry(const std::vector<glm::vec3> &vertices,
           const std::vector<glm::vec2> &uvs);
  ~Geometry();

  template <typename T>
  void uploadBuffer(GLuint &buffer, const std::vector<T> &data,
                    GLuint attributeIndex, GLint componentCount);
  };
```

其中VAO, VBO, EBO的创建，实现如下

#H(title: "runtime/resource/geometry/geometry.cpp")

```cpp
template <typename T>
void Geometry::uploadBuffer(GLuint &buffer, const std::vector<T> &data,
                            GLuint attributeIndex, GLint componentCount) {
  GL_CALL(glGenBuffers(1, &buffer));
  GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
  GL_CALL(glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(),
                       GL_STATIC_DRAW));
  GL_CALL(glEnableVertexAttribArray(attributeIndex));
  GL_CALL(glVertexAttribPointer(attributeIndex, componentCount, GL_FLOAT,
                                GL_FALSE, componentCount * sizeof(float),
                                (void *)0));
}

Geometry::Geometry(const std::vector<glm::vec3> &vertices,
                   const std::vector<glm::vec2> &uvs,
                   const std::vector<glm::vec3> &normals,
                   const std::vector<unsigned int> &indices) {
  _numIndices = indices.size();

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);

  uploadBuffer(_posVbo, vertices, 0, 3);
  uploadBuffer(_uvVbo, uvs, 1, 2);
  uploadBuffer(_normalVbo, normals, 2, 3);

  glGenBuffers(1, &_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}
```
- 使用single buffer模式管理各类顶点数据
- 使用模版统一不同数据类型VBO的创建
- 不考虑顶点数据在运行时的动态变化，使用`GL_STATIC_DRAW`开辟GPU侧静态缓冲区

#colbreak()

=== 着色器资源

着色器资源是管线中的一个重要组成部分，用于控制渲染的流程，其储存着代码片段，在渲染时传入GPU进行编译和链接，需要维护着色器的ID，在析构时，需要根据着色器ID释放着色器资源，其声明如下：

#H(title: "include/runtime/resource/shader/shader.h")

```cpp
class Shader {
  friend class ResourceManager;

public:
  void begin() const;
  void end() const;

  GLuint getProgram() const { return _program; }

  void setUniform(const std::string &name, float value) const;
  void setUniform(const std::string &name, int value) const;
  void setUniform(const std::string &name, glm::vec3 value) const;
  void setUniform(const std::string &name, glm::mat3 value) const;
  void setUniform(const std::string &name, glm::mat4 value) const;

private:
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  Shader(const std::string &vertexPath, const std::string &geometryPath, const std::string &fragmentPath);
  Shader(const std::string &vertexPath, const std::string &tessCtrlPath, const std::string &tessEvalPath, const std::string &fragmentPath);
  ~Shader();

  enum class ErrorType { COMPILE, LINK };

  GLuint _program{0};

  static void checkShaderErrors(GLuint target, ErrorType type);

  GLuint compileShader(const std::string &path, GLenum shaderType);
  void linkProgram(const std::vector<GLuint> &shaders);
};
```
- 着色器准备流程为：从文件中读取着色器代码，编译，链接
- 由于在OpenGL可编程管线中，几何着色器、镶嵌评估/控制着色器为可选着色器，为了兼容不同的着色器组合，在链接着色器时，使用可变长数组，同时提供多种着色器生成接口
- 使用函数重载统一不同类型的`Uniform`变量的设置
- 由于着色器准备过程中涉及编译链接，需为其提供完备的错误检查

其中着色器资源的准备，实现如下(以最简单的顶点+片元着色器为例)

#H(title: "runtime/resource/shader/shader.cpp")

```cpp
Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
  GLuint vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
  GLuint fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
  linkProgram({vertex, fragment});
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::checkShaderErrors(GLuint target, ErrorType type) {
  int success = 0;
  int infoLogLength = 0;

  if (type == ErrorType::COMPILE) {
    glGetShaderiv(target, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderiv(target, GL_INFO_LOG_LENGTH, &infoLogLength);
      std::vector<char> infoLog(infoLogLength);
      glGetShaderInfoLog(target, infoLogLength, NULL, infoLog.data());
      Err("Shader compile error: %s", infoLog.data());
    }
  } else if (type == ErrorType::LINK) {
    glGetProgramiv(target, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramiv(target, GL_INFO_LOG_LENGTH, &infoLogLength);
      std::vector<char> infoLog(infoLogLength);
      glGetProgramInfoLog(target, infoLogLength, NULL, infoLog.data());
      Err("Shader link error: %s", infoLog.data());
    }
  }
}

GLuint Shader::compileShader(const std::string &path, GLenum shaderType) {
  std::ifstream file;
  std::string code;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path.c_str());
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();
  } catch (std::ifstream::failure &e) {
    Err("Shader file error: %s", e.what());
  }
  GLuint shader = glCreateShader(shaderType);
  const char *source = code.c_str();
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);
  checkShaderErrors(shader, ErrorType::COMPILE);
  return shader;
}

void Shader::linkProgram(const std::vector<GLuint> &shaders) {
  _program = glCreateProgram();
  for (auto s : shaders) {
    glAttachShader(_program, s);
  }
  glLinkProgram(_program);
  checkShaderErrors(_program, ErrorType::LINK);
}
```

== 渲染流程

OpenGL的渲染管线为顶点着色、图元装配、剪裁剔除、光栅化、片元着色、测试与混合。具体到实现上，即先遍历所有需要渲染的物体，设置渲染状态，然后绑定着色器与纹理单元，设置着色器中的Uniform变量，最后绑定VAO，下发DrawCall.

渲染过程中涉及的数据及其来源如下图所示

#figure(image("../images/3.1.png", width: 60%), caption: "渲染过程涉及的数据")

- Mesh = Geometry + Material，即网格体组件提供渲染所需的几何形体与材质信息
- Material = Texture + Uniform(Texture等), 即Material维护在GPU上实际运行的程序和其所需要的与材质本身相关的数据
- Transform组件的目的是提供顶点着色器的ModelMatrix，供顶点着色器使用
- Camera组件的目的是提供顶点着色器的ViewMatrix和ProjectionMatrix，供顶点着色器使用；此外，对于光照计算所需的相机位置，则可有相机的Transform组件提供
- Light组件的目的是提供Shader中与光照相关的Uniform变量的值

渲染本质上是要汇聚这些信息，然后对每个物体执行渲染流程，完成图形的渲染。

=== 着色器

着色器作为渲染过程的核心，一切数据都是围绕着色器进行设置，如VAO, 本质上是提供着色器的属性数据，而其它数据则是为Uniform变量提供数据。

最简单的着色器由顶点着色器和片元着色器构成。以下是最简单的白模的着色器，用作默认材质的着色器

#figure()[

#SH(title: "assets/shaders/white/white.vert")
```vert
#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
```
]<l2>

- 对于顶点数据，在VAO中已经描述了位置信息、UV信息和法线信息
- 顶点着色器负责对顶点位置进行MVP变换，先将坐标转换为齐次坐标，最后输出到内置变量`gl_Position`中

#SH(title: "assets/shaders/white/white.frag")
```frag
#version 410 core

out vec4 FragColor;

uniform vec3 color;

void main() { 
    FragColor = vec4(color, 1.0);
}
```

=== 变换

Transform组件对物体的变换信息进行维护，目的是提供物体的模型矩阵用于渲染、为物理系统提供位置信息等。

仿照Unity@unity,游戏对象默认携带Transform组件。声明、实现如下

#H(title: "include/runtime/framework/component/transform/transform.h")

```cpp
class TransformComponent : public Component {
public:
  TransformComponent() = default;
  ~TransformComponent() override{};

  glm::mat4 getModel() const;
  glm::vec3 getPositionLocal() const { return _position; }
  glm::vec3 getPositionWorld() const { return getModel() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); }
  glm::vec3 getForwardVec() const {
    return  glm::normalize(glm::vec3(glm::vec4{_forwardVec,0.0f}  * glm::yawPitchRoll(glm::radians(_angle.y), glm::radians(_angle.x), glm::radians(-_angle.z)) ));
  }
  glm::vec3 getUpVec() const {
    return  glm::normalize(glm::vec3(glm::vec4{_upVec,0.0f}  * glm::yawPitchRoll(glm::radians(_angle.y), glm::radians(_angle.x), glm::radians(-_angle.z)) ));
  }
  glm::vec3 getRightVec() const {
    return  glm::normalize(glm::vec3(glm::vec4{_rightVec,0.0f}  * glm::yawPitchRoll(glm::radians(_angle.y), glm::radians(_angle.x), glm::radians(-_angle.z)) ));
  }

  glm::vec3 getAngle() const { return _angle; }
  glm::vec3 getScale() const { return _scale; }
  glm::mat4 getParentModel() const { return _parentModel; }

  void setPositionLocal(const glm::vec3 &position) { _position = position; }
  void setAngle(const glm::vec3 &angle) { _angle = angle; }
  void setScale(const glm::vec3 &scale) { _scale = scale; }
  void setParentModel(const glm::mat4 &model) { _parentModel = model; }

private:
  glm::vec3 _position{glm::zero<float>()};
  glm::vec3 _angle{glm::zero<float>()};
  glm::vec3 _scale{glm::one<float>()};

  glm::vec3 _forwardVec{0.0f, 0.0f, 1.0f};
  glm::vec3 _upVec{0.0f, 1.0f, 0.0f};
  glm::vec3 _rightVec{-1.0f, 0.0f, 0.0f};

  glm::mat4 _parentModel{glm::one<float>()};
};
```

#H(title: "runtime/framework/component/transform/transform.cpp")

```cpp
glm::mat4 TransformComponent::getModel() const {
  glm::mat4 model{1.0f};
  model = glm::scale(model, _scale);
  model = glm::rotate(model, glm::radians(_angle.x), glm::vec3(1.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(_angle.y), glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::rotate(model, glm::radians(_angle.z), glm::vec3(0.0f, 0.0f, 1.0f));
  return _parentModel * glm::translate(glm::mat4(1.0f), _position) * model;
}
```

- `getPositionLocal`, `getPositionWorld`用于获取物体的局部坐标与全局坐标
- `getForwardVec`, `getUpVec`, `getRightVec`用于确定物体的朝向，用于物理系统

在分发过程中，对父节点的ModelView信息进行维护，具体见@l1.

渲染时，通过获取游戏对象的Transform组件，得到相应的ModelView.

=== 相机

Camera组件目的是提供View与Projection Matrix, 根据投影方式的不同，分为正交投影和透视投影两种。

声明、实现如下

#H(title: "include/runtime/framework/component/camera/camera.h")

```cpp
class CameraComponent : public Component {
public:
  enum class Type { Ortho, Perspective, Invalid };

  CameraComponent(float left, float right, float top, float bottom, float near, float far);
  CameraComponent(float fovy, float aspect, float near, float far);

  ~CameraComponent() override{};

  bool isMain() const { return _isMain; }
  void pick() { _isMain = true; }
  void put() { _isMain = false; }

  glm::mat4 getView(glm::vec3 position) const;
  glm::mat4 getProjection() const;

  const glm::vec3 &getUpVec() const { return _upVec; }
  const glm::vec3 &getRightVec() const { return _rightVec; }

  void setUpVec(const glm::vec3 &up) { _upVec = up; }
  void setRightVec(const glm::vec3 &right) { _rightVec = right; }

private:
  glm::vec3 _upVec{0.0f, 1.0f, 0.0f}, _rightVec{1.0f, 0.0f, 0.0f};
  Type _type{Type::Invalid};

  bool _isMain{true};

  float _top, _bottom, _left, _right, _near, _far, _fovy, _aspect;
};
```

#H(title: "runtime/framework/component/camera/camera.cpp")
```cpp
CameraComponent::CameraComponent(float left, float right, float top, float bottom, float near, float far) {
  _type = Type::Ortho;
  _left = left, _right = right, _top = top, _bottom = bottom, _near = near,
  _far = far;
}

CameraComponent::CameraComponent(float fovy, float aspect, float near, float far) {
  _type = Type::Perspective;
  _fovy = fovy, _aspect = aspect, _near = near, _far = far;
}

glm::mat4 CameraComponent::getView(glm::vec3 position) const {
  glm::vec3 front = glm::cross(_upVec, _rightVec);
  return glm::lookAt(position, position + front, _upVec);
}

glm::mat4 CameraComponent::getProjection() const {
  if (_type == Type::Ortho)
    return glm::ortho(_left, _right, _bottom, _top);
  else if (_type == Type::Perspective)
    return glm::perspective(glm::radians(_fovy), _aspect, _near, _far);
  Log("Unknown camera type!");
  return glm::mat4(glm::one<float>());
}
```

- 通过重载构造函数，实现不同类型相机组件的构造

=== 光照

Light组件用于记录不同光源所需的信息，包括环境光、平行光、点光源、探照灯四种类型，声明如下

#H(title: "include/runtime/framework/component/light/light.h")

```cpp
class LightComponent : public Component {
public:
  enum class Type { Directional, Point, Spot, Ambient, Invalid };

  LightComponent(const glm::vec3 &color) : _color(color), _type(Type::Ambient) {}
  LightComponent(const glm::vec3 &color, const glm::vec3 &direction,
        float specularIntensity)
      : _color(color), _direction(direction),
        _specularIntensity(specularIntensity), _type(Type::Directional) {}
  LightComponent(const glm::vec3 &color, float specularIntensity, float k2, float k1,
        float kc)
      : _color(color), _specularIntensity(specularIntensity), _k2(k2), _k1(k1),
        _kc(kc), _type(Type::Point) {}
  LightComponent(const glm::vec3 &color, const glm::vec3 &direction,
        float specularIntensity, float inner, float outer)
      : _color(color), _direction(direction),
        _specularIntensity(specularIntensity), _inner(inner), _outer(outer),
        _type(Type::Spot) {}
  ~LightComponent() override{};

  Type getType() const { return _type; }
  glm::vec3 getColor() const { return _color; }
  glm::vec3 getDirection() const { return _direction; }
  float getSpecularIntensity() const { return _specularIntensity; }
  float getK2() const { return _k2; }
  float getK1() const { return _k1; }
  float getKc() const { return _kc; }
  float getInner() const { return _inner; }
  float getOuter() const { return _outer; }

private:
  glm::vec3 _color, _direction;
  float _specularIntensity, _k2, _k1, _kc, _inner, _outer;

  Type _type{Type::Invalid};
};
```

其中光源声明如下

#H(title: "include/runtime/framework/component/light/light.h")

```cpp
struct DirectionalLight {
  glm::vec3 color;
  glm::vec3 direction;
  float specularIntensity;
};

struct PointLight {
  glm::vec3 position;
  glm::vec3 color;
  float specularIntensity;
  float k2;
  float k1;
  float kc;
};

struct SpotLight {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 direction;
  float specularIntensity;
  float inner;
  float outer;
};

struct AmbientLight {
  glm::vec3 color;
};
```

与片元着色器中的声明保持一致

#SH(title: "assets/shaders/phong/phong.frag")

```frag
struct DirectionalLight {
  vec3 color;
  vec3 direction;
  float specularIntensity;
};

struct PointLight {
  vec3 position;
  vec3 color;
  float specularIntensity;
  float k2;
  float k1;
  float kc;
};

struct SpotLight {
  vec3 position;
  vec3 color;
  vec3 direction;
  float specularIntensity;
  float inner;
  float outer;
};

struct AmbientLight {
  vec3 color;
};
```

=== 渲染实现

渲染系统只需按照上述流程收集所有信息，再对每个物体进行绘制即可。

实现如下：

#H(title: "include/runtime/framework/system/renderSystem.h")

```cpp
struct ModelInfo {
  glm::mat4 model;
};

struct CameraInfo {
  glm::vec3 position;
  glm::mat4 view;
  glm::mat4 project;
};

struct LightInfo {
  std::vector<DirectionalLight> directionalLights{};
  std::vector<PointLight> pointLights{};
  std::vector<SpotLight> spotLights{};
  std::vector<AmbientLight> ambientLights{};
};

struct RenderInfo {
  ModelInfo &modelInfo;
  CameraInfo &cameraInfo;
  LightInfo &lightInfo;
};

class RenderSystem {
public:
  RenderSystem() = default;
  ~RenderSystem() = default;

  void dispatch(GameObject *object);
  void tick();

private:
  std::vector<GameObject *> _meshes[2], _lights, _cameras;

  void initState();
  void setDepthState(Material *material);
  void setBlendState(Material *material);

  void clear();
};
```
- 声明模型信息、相机信息与光照信息结构体，便于传参

```cpp
void RenderSystem::dispatch(GameObject *object) {
  if (object->getComponent<MeshComponent>())
    _meshes[object->getComponent<MeshComponent>()->getMaterial()->getBlend()]
        .push_back(object);
  if (object->getComponent<LightComponent>())
    _lights.push_back(object);
  if (object->getComponent<CameraComponent>())
    _cameras.push_back(object);
  if (object->getComponent<TerrainComponent>())
    _meshes[0].push_back(object);
}

void RenderSystem::tick() {
  initState();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GameObject *mainCamera = nullptr;
  if (_cameras.empty())
    Err("No camera found!");
  for (auto camera : _cameras) {
    if (camera->getComponent<CameraComponent>()->isMain()) {
      mainCamera = camera;
      break;
    }
  }
  if (!mainCamera) {
    Log("No main camera found! Use default.");
    _cameras[0]->getComponent<CameraComponent>()->pick();
    mainCamera = _cameras[0];
  }
  std::shared_ptr<CameraComponent> mainCameraComp =
      mainCamera->getComponent<CameraComponent>();
  std::shared_ptr<TransformComponent> mainCameraTransfrom =
      mainCamera->getComponent<TransformComponent>();
  glm::vec3 mainCameraPosition = mainCameraTransfrom->getPositionWorld();
  CameraInfo cameraInfo{.position = mainCameraPosition,
                        .view = mainCameraComp->getView(mainCameraPosition),
                        .project = mainCameraComp->getProjection()};

  LightInfo lightInfo;
  for (auto light : _lights) {
    std::shared_ptr<LightComponent> lightComp =
        light->getComponent<LightComponent>();
    switch (lightComp->getType()) {
    case LightComponent::Type::Directional:
      lightInfo.directionalLights.emplace_back(DirectionalLight{
          .color = lightComp->getColor(),
          .direction = lightComp->getDirection(),
          .specularIntensity = lightComp->getSpecularIntensity()});
      break;
    case LightComponent::Type::Point:
      lightInfo.pointLights.emplace_back(PointLight{
          .position =
              light->getComponent<TransformComponent>()->getPositionWorld(),
          .color = lightComp->getColor(),
          .specularIntensity = lightComp->getSpecularIntensity(),
          .k2 = lightComp->getK2(),
          .k1 = lightComp->getK1(),
          .kc = lightComp->getKc()});
      break;
    case LightComponent::Type::Spot:
      lightInfo.spotLights.emplace_back(SpotLight{
          .position =
              light->getComponent<TransformComponent>()->getPositionWorld(),
          .color = lightComp->getColor(),
          .direction = lightComp->getDirection(),
          .specularIntensity = lightComp->getSpecularIntensity(),
          .inner = lightComp->getInner(),
          .outer = lightComp->getOuter()});
      break;
    case LightComponent::Type::Ambient:
      lightInfo.ambientLights.emplace_back(
          AmbientLight{.color = lightComp->getColor()});
      break;
    default:
      Log("Unknow light type. Abort.");
      break;
    }
  }

  std::sort(
      _meshes[1].begin(), _meshes[1].end(),
      [&cameraInfo](const GameObject *a, const GameObject *b) {
        return (cameraInfo.view *
                glm::vec4(
                    a->getComponent<TransformComponent>()-> getPositionWorld(),
                    1.0))
                   .z <
               (cameraInfo.view *
                glm::vec4(
                    b->getComponent<TransformComponent>()-> getPositionWorld(),
                    1.0))
                   .z;
      });

  for (auto meshes : _meshes)
    for (auto mesh : meshes) {
      std::shared_ptr<MeshComponent> meshComp =
          mesh->getComponent<MeshComponent>();
      Geometry *geometry = meshComp->getGeometry();
      Material *material = meshComp->getMaterial();
      ModelInfo modelInfo{
          .model = mesh->getComponent<TransformComponent>()->getModel()};

      setDepthState(material);
      setBlendState(material);

      material->apply(RenderInfo{.modelInfo = modelInfo,
                                 .cameraInfo = cameraInfo,
                                 .lightInfo = lightInfo});
      GL_CALL(glBindVertexArray(geometry->getVao()));
      if (mesh->getComponent<TerrainComponent>()) {
        int rez = mesh->getComponent<TerrainComponent>()->getRez();
        GL_CALL(glPatchParameteri(GL_PATCH_VERTICES, 4));
        GL_CALL(glDrawArrays(GL_PATCHES, 0, 4 * rez * rez));
      } else {
        GL_CALL(glDrawElements(GL_TRIANGLES, geometry->getNumIndices(),
                               GL_UNSIGNED_INT, 0));
      }
      material->finish();
    }
  clear();
}

void RenderSystem::clear() {
  _meshes[0].clear();
  _meshes[1].clear();
  _lights.clear();
  _cameras.clear();
}

void RenderSystem::initState() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
}

void RenderSystem::setDepthState(Material *material) {
  if (material->getDepthTest()) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(material->getDepthFunc());
  } else
    glDisable(GL_DEPTH_TEST);

  if (material->getDepthWrite())
    glDepthMask(GL_TRUE);
  else
    glDepthMask(GL_FALSE);
}

void RenderSystem::setBlendState(Material *material) {
  if (material->getBlend()) {
    glEnable(GL_BLEND);
    glBlendFunc(material->getSFactor(), material->getDFactor());
  } else
    glDisable(GL_BLEND);
}
```
- 分发阶段，需对物体进行分类，确定网格体、光照与相机
- 更新阶段，由于考虑到透明材质与地形的渲染，需对基础的渲染流程进行调整
  - 基础流程：设定渲染状态，收集信息，绑定着色器，施用材质，绑定VAO，调用DrawCall
  - 由于透明材质的实现，本质上是利用了颜色混合功能，故需修改渲染顺序。对于渲染系统而言，游戏对象乱序分发，原先基于深度缓冲的绘制算法无法保证透明材质能混合到正确的颜色；若透明材质先于不透明材质而绘制，则由于透明材质不进行深度写入，若不透明材质的深度值小于透明材质，则会将透明物体遮蔽；故正确的渲染流程为：先绘制不透明物体，再按距离相机由远及近的顺序依次绘制透明物体，以确保颜色能正确混合。上述实现中，不透明物体被放置在`_mesh[0]`, 透明物体被放置在`_mesh[1]`；在获取到相机信息后，即可计算出与相机的距离(变换到相机系，取z分量)并排序
  - 对于地形数据，由于绘制的补丁顶点数不同，且地形绘制并不依赖于EBO, 故需对地形进行特殊判断，调用地形绘制的DrawCall

此处的渲染流程亦可扩展，若需进行阴影渲染等后处理，可以为其添加帧缓冲，并渲染多个pass，如在前一次渲染的颜色附件、深度模版附件上进行采样等。由于时间关系，此处并未做过多处理，若后续有机会再引入阴影系统的处理。

== 材质

CPU端准备了复杂且精美的美术模型，而实时、高级、细腻的特效则依赖于GPU端的着色器，如天空盒、地形生成、爆炸效果等。GPU端的程序(着色器)及其数据（材质）被封装为Material，声明如下

#H(title: "include/runtime/resource/material/material.h")
```cpp
class Material {
public:
  virtual ~Material() = 0;

  Material() = default;
  Material(const Material& other) {
    _shader = other._shader;
    _depthTest = other._depthTest;
    _depthFunc = other._depthFunc;
    _depthWrite = other._depthWrite;
    _blend = other._blend;
    _sFactor = other._sFactor;
    _dFactor = other._dFactor;
  }

  virtual Material *clone() const = 0;
  virtual void apply(const RenderInfo &info) = 0;

  bool getDepthTest() const { return _depthTest; }
  GLenum getDepthFunc() const { return _depthFunc; }
  bool getDepthWrite() const { return _depthWrite; }
  bool getBlend() const { return _blend; }
  unsigned int getSFactor() const { return _sFactor; }
  unsigned int getDFactor() const { return _dFactor; }

  virtual void setDiffuse(Texture *diffuse){};

  void finish() {
    if (_shader)
      _shader->end();
  };

protected:
  Shader *_shader = nullptr;

  // Depth test
  bool _depthTest = true;
  GLenum _depthFunc = GL_LEQUAL;
  bool _depthWrite = true;

  // Color blending
  bool _blend = false;
  unsigned int _sFactor = GL_SRC_ALPHA;
  unsigned int _dFactor = GL_ONE_MINUS_SRC_ALPHA;
};
```
- 为了利用C++多态特性，提供`apply`作为虚方法，在渲染时通过`apply`接口来调用各种不同材质统一设置着色器参数
- 由于模型载入时，需对材质进行深拷贝，提供`clone`方法实现材质拷贝

这样，添加新材质时，只需重载这些虚方法即可。以白模为例

#H(title: "include/runtime/resource/material/whiteMaterial.h")

```cpp
class WhiteMaterial : public Material {
public:
  WhiteMaterial();
  WhiteMaterial(const WhiteMaterial &other) : Material(other) {
    _color = other._color;
  }

  ~WhiteMaterial() override{};
  void setColor(const glm::vec3 &color);

  WhiteMaterial* clone() const override { return new WhiteMaterial(*this); }
  void apply(const RenderInfo &info) override;

private:
  glm::vec3 _color = glm::vec3(1.0f, 1.0f, 1.0f);
};
```

实现如下：

```cpp
WhiteMaterial::WhiteMaterial(){
    ResourceManager *resourceManager = Engine::getEngine()-> getResourceManager();
    _shader = resourceManager->loadShader("assets/shaders/white/white.vert", "assets/shaders/white/white.frag");
}

void WhiteMaterial::setColor(const glm::vec3& color){
    _color = color;
}

void WhiteMaterial::apply(const RenderInfo &info){
    _shader->begin();

    _shader->setUniform("model",info.modelInfo.model);
    _shader->setUniform("view",info.cameraInfo.view);
    _shader->setUniform("projection",info.cameraInfo.project);

    _shader->setUniform("color", _color);
}
```

- 由于`RenderInfo`里面已经汇集了全部信息，只需给对应的Uniform变量赋值即可

其着色器见@l2.

=== 冯氏光照模型

冯氏模型(Phong Material)作为最经典的光照模型，可以非常简单的实现较为真实的光照

$
  I = I_"ambient" + I_"diffuse" + I_"specular"
$

故只需在着色器中累计不同光的贡献即可。着色器实现如下

#SH(title: "assets/shaders/phong/phong.vert")

```vert
#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

out VS_OUT {
  vec2 uv;
  vec3 normal;
  vec3 worldPosition;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
  vec4 transformPosition = model * vec4(aPosition, 1.0);

  gl_Position = projection * view * transformPosition;
  uv = aUv;
  normal = normalMatrix * aNormal;
  worldPosition = transformPosition.xyz;
  vs_out.uv = aUv;
  vs_out.normal = aNormal;
  vs_out.worldPosition = worldPosition;
}
```
- 为了复用顶点着色器，为其设置两组输出变量。利用着色器可存在冗余输出的特性，其中`VS_OUT`为提供给几何着色器的输出，其它输出为提供给片元着色器的输出，从而实现这份顶点着色器编译后即可以只与片元着色器进行链接，也可以与几何着色器、片元着色器一同链接
- NormalMatrix为法线矩阵，用于对法线进行变换，其大小为$("Model"^(-1))^top$. 由于GPU端求逆阵计算量大，并行性差(需使用高斯消元法转化为TRSM求解，或直接暴力求解)，且每个顶点都需计算，浪费GPU算力，故在CPU端计算一次后直接下发计算结果

#SH(title: "assets/shaders/phong/phong.frag")

#show figure: set block(breakable: true)

#figure()[
```frag
#version 410 core

#define MAX_LIGHT_NUM 5

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

out vec4 FragColor;

struct DirectionalLight {
  vec3 color;
  vec3 direction;
  float specularIntensity;
};

struct PointLight {
  vec3 position;
  vec3 color;
  float specularIntensity;
  float k2;
  float k1;
  float kc;
};

struct SpotLight {
  vec3 position;
  vec3 color;
  vec3 direction;
  float specularIntensity;
  float inner;
  float outer;
};

struct AmbientLight {
  vec3 color;
};

uniform sampler2D diffuse;
uniform float shiness;

uniform vec3 cameraPosition;

uniform int numDirectionalLight;
uniform DirectionalLight directionalLight[MAX_LIGHT_NUM];

uniform int numPointLight;
uniform PointLight pointLight[MAX_LIGHT_NUM];

uniform int numSpotLight;
uniform SpotLight spotLight[MAX_LIGHT_NUM];

uniform AmbientLight ambientLight;

uniform sampler2D alphaMap;
uniform float useAlphaMap;

vec3 calDiffuse(vec3 lightColor, vec3 objectColor, vec3 lightDir, vec3 normal) {
  float diffuse = clamp(dot(-lightDir, normal), 0.0, 1.0);
  return lightColor * diffuse * objectColor;
}

vec3 calSpecular(vec3 lightColor, vec3 lightDir, vec3 normal, vec3 viewDir,
                 float intensity) {
  vec3 lightReflect = normalize(reflect(lightDir, normal));
  float specular = clamp(dot(lightReflect, -viewDir), 0.0, 1.0);
  specular = pow(specular, shiness);
  float back = step(0.0, dot(-lightDir, normal));
  return lightColor * specular * intensity * back;
}

vec3 calDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
  vec3 objectColor = texture(diffuse, uv).xyz;
  vec3 lightDir = normalize(light.direction);
  vec3 diffuseColor = calDiffuse(light.color, objectColor, lightDir, normal);
  vec3 specularColor = calSpecular(light.color, lightDir, normal, viewDir,
                                   light.specularIntensity);
  return diffuseColor + specularColor;
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 viewDir) {
  vec3 objectColor = texture(diffuse, uv).xyz;
  vec3 lightDir = normalize(worldPosition - light.position);
  float dist = length(worldPosition - light.position);
  float attenuation =
      1.0 / (light.k2 * dist * dist + light.k1 * dist + light.kc);
  vec3 diffuseColor = calDiffuse(light.color, objectColor, lightDir, normal);
  vec3 specularColor = calSpecular(light.color, lightDir, normal, viewDir,
                                   light.specularIntensity);
  return (diffuseColor + specularColor) * attenuation;
}

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
  vec3 objectColor = texture(diffuse, uv).xyz;
  vec3 lightDir = normalize(worldPosition - light.position);
  vec3 targetDir = normalize(light.direction);
  float cGamma = dot(lightDir, targetDir);
  float intensity =
      clamp((cGamma - light.outer) / (light.inner - light.outer), 0.0, 1.0);
  vec3 diffuseColor = calDiffuse(light.color, objectColor, lightDir, normal);
  vec3 specularColor = calSpecular(light.color, lightDir, normal, viewDir,
                                   light.specularIntensity);
  return (diffuseColor + specularColor) * intensity;
}

vec3 calAmbientLight(AmbientLight light) {
  return light.color * texture(diffuse, uv).xyz;
}

void main() {
  vec3 normalN = normalize(normal);
  vec3 viewDir = normalize(worldPosition - cameraPosition);

  vec3 result = vec3(0.0);

  for (int i = 0; i < numDirectionalLight; i++)
    result += calDirectionalLight(directionalLight[i], normalN, viewDir);
  for (int i = 0; i < numPointLight; i++)
    result += calPointLight(pointLight[i], normalN, viewDir);
  for (int i = 0; i < numSpotLight; i++)
    result += calSpotLight(spotLight[i], normalN, viewDir);
  result += calAmbientLight(ambientLight);

  float alphaValue = mix(1.0, texture(alphaMap, uv).r, useAlphaMap);
  FragColor = vec4(result, alphaValue);
}
```
]<l3>
- 各种光照计算公式如下

  - 漫反射计算公式：$I_"diffuse" = k_d dot (L dot N) dot I_L$
  使用从漫反射贴图采样得到的物体颜色作为漫反射系数$k_d$，$L$ 是光线方向，$N$ 是法线方向，使用光源颜色代替光源强度$I_L$

  - 镜面反射计算公式：$I_"specular" = k_s dot (R dot V)^n dot I_L$
  其中，$k_s$ 是镜面反射系数，$R$ 是反射光方向，$V$ 是视线方向，$n$ 是高光指数

  - 环境光计算公式：$I_"ambient" = k_a dot I_a$
  其中，$k_a$ 是环境光系数，使用环境光颜色代替环境光强度$I_a$

  - 平行光计算公式：$I = I_"diffuse" + I_"specular"$
  
  - 点光源计算公式：$I = (I_"diffuse" + I_"specular")/(k_c + k_l dot d + k_q dot d^2)$
  其中，$k_c$ 是常数衰减系数，$k_1$ 是线性衰减系数，$k_2$ 是二次衰减系数，$d$ 是光源到点的距离

  - 探照灯计算公式：$I=(L dot D - cos(theta_"outer"))/(cos(theta_"inner")-cos(theta_"outer"))(I_"diffuse"+I_"specular")$
  其中，$D$ 是探照灯方向，$theta_"inner"$ 是内锥角，$theta_"outer"$ 是外锥角

- 为了实现着色器复用，在着色器中埋了透明材质的计算。默认情况下，`useAlphaMap`恒为0，只有在启用透明材质时才会参与混合
  - 为了降低条件判断对GPU执行效率的影响，使用`mix`函数代替条件分支

使用类`PhongMaterial`继承抽象类`Material`，实现如下

#H(title: "include/game/material/phongMaterial.h")

```cpp
class PhongMaterial : public Material {
public:
  PhongMaterial();
  PhongMaterial(const PhongMaterial &other) : Material(other) {
    _diffuse = other._diffuse;
    _shiness = other._shiness;
  }
  PhongMaterial(const Material &other) : Material(other) {
    ResourceManager *resourceManager = Game::getGame()->getEngine()-> getResourceManager();
    _shader = resourceManager->loadShader(
      "assets/shaders/phong/phong.vert", "assets/shaders/phong/phong.frag");
    _diffuse = nullptr;
    _shiness = 32.0f;
  }

  PhongMaterial* clone() const override { return new PhongMaterial(*this); }
  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override { _diffuse = diffuse; }
  void setShiness(float shiness) { _shiness = shiness; }

private:
  Texture *_diffuse{nullptr};
  float _shiness{32.0f};
};
```

- 从`Material`拷贝构造出`PhongMaterial`时，需为其添加着色器

#H(title: "game/material/phongMaterial.cpp")

```cpp
PhongMaterial::PhongMaterial() {
  ResourceManager *resourceManager = Game::getGame()->getEngine()-> getResourceManager();
  _shader = resourceManager->loadShader(
      "assets/shaders/phong/phong.vert", "assets/shaders/phong/phong.frag");
}

void PhongMaterial::apply(const RenderInfo &info) {
  _shader->begin();

  _shader->setUniform("model", info.modelInfo.model);
  _shader->setUniform("view", info.cameraInfo.view);
  _shader->setUniform("projection", info.cameraInfo.project);
  _shader->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(info.modelInfo.model))));

  _diffuse->bind();
  _shader->setUniform("diffuse", (int)_diffuse->getUnit());
  _shader->setUniform("shiness", _shiness);
  _shader->setUniform("cameraPosition", info.cameraInfo.position);

  _shader->setUniform("numDirectionalLight", int(info.lightInfo.directionalLights.size()));
  for (int i = 0; i < info.lightInfo.directionalLights.size(); i++) {
    auto &dl = info.lightInfo.directionalLights[i];
    std::string prefix = "directionalLight[" + std::to_string(i) + "]";
    _shader->setUniform(prefix + ".color", dl.color);
    _shader->setUniform(prefix + ".direction", dl.direction);
    _shader->setUniform(prefix + ".specularIntensity", dl.specularIntensity);
  }

  _shader->setUniform("numPointLight", int(info.lightInfo.pointLights.size()));
  for (int i = 0; i < info.lightInfo.pointLights.size(); i++) {
    auto &pl = info.lightInfo.pointLights[i];
    std::string prefix = "pointLight[" + std::to_string(i) + "]";
    _shader->setUniform(prefix + ".position", pl.position);
    _shader->setUniform(prefix + ".color", pl.color);
    _shader->setUniform(prefix + ".specularIntensity", pl.specularIntensity);
    _shader->setUniform(prefix + ".k2", pl.k2);
    _shader->setUniform(prefix + ".k1", pl.k1);
    _shader->setUniform(prefix + ".kc", pl.kc);
  }

  _shader->setUniform("numSpotLight", int(info.lightInfo.spotLights.size()));
  for (int i = 0; i < info.lightInfo.spotLights.size(); i++) {
    auto &sl = info.lightInfo.spotLights[i];
    std::string prefix = "spotLight[" + std::to_string(i) + "]";
    _shader->setUniform(prefix + ".position", sl.position);
    _shader->setUniform(prefix + ".color", sl.color);
    _shader->setUniform(prefix + ".direction", sl.direction);
    _shader->setUniform(prefix + ".specularIntensity", sl.specularIntensity);
    _shader->setUniform(prefix + ".inner", sl.inner);
    _shader->setUniform(prefix + ".outer", sl.outer);
  }

  _shader->setUniform("ambientLight.color", info.lightInfo.ambientLights[0].color);

  _shader->setUniform("useAlphaMap", .0f);
}
```

由于冯氏模型实现了基本的光照模型，故后续部分材质直接在`PhongMaterial`上派生，继承关系如下

#figure(image("../images/3.2.png", width: 60%), caption: "材质继承关系")

凡是共用着色器的材质都可采用继承关系实现。

=== 天空盒

天空盒即为蒙在头顶、跟随相机移动的立方体，通过对CubeMap进行采样实现。

首先，需准备天空盒的顶点数据

```cpp
Geometry *ResourceManager::createBoxGeometry(float size) {
  std::vector<glm::vec3> vertices = {glm::vec3(size / 2, size / 2, size / 2),
                                    glm::vec3(-size / 2, size / 2, size / 2),
                                    glm::vec3(-size / 2, size / 2, -size / 2),
                                    glm::vec3(size / 2, size / 2, -size / 2),

                                    glm::vec3(size / 2, -size / 2, size / 2),
                                    glm::vec3(-size / 2, -size / 2, size / 2),
                                    glm::vec3(-size / 2, -size / 2, -size /2),
                                    glm::vec3(size / 2, -size / 2, -size / 2),

                                    glm::vec3(size / 2, size / 2, size / 2),
                                    glm::vec3(-size / 2, size / 2, size / 2),
                                    glm::vec3(-size / 2, -size / 2, size / 2),
                                    glm::vec3(size / 2, -size / 2, size / 2),

                                    glm::vec3(size / 2, size / 2, -size / 2),
                                    glm::vec3(-size / 2, size / 2, -size / 2),
                                    glm::vec3(-size / 2, -size / 2, -size /2),
                                    glm::vec3(size / 2, -size / 2, -size / 2),

                                    glm::vec3(size / 2, size / 2, size / 2),
                                    glm::vec3(size / 2, size / 2, -size / 2),
                                    glm::vec3(size / 2, -size / 2, -size / 2),
                                    glm::vec3(size / 2, -size / 2, size / 2),

                                    glm::vec3(-size / 2, size / 2, size / 2),
                                    glm::vec3(-size / 2, size / 2, -size / 2),
                                    glm::vec3(-size / 2, -size / 2, -size /2),
                                    glm::vec3(-size / 2, -size / 2, size /2)};

  // six faces, each face has 4 vertices
  std::vector<glm::vec2> uvs = {
      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),

      glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f),
      glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f),
  };

  std::vector<glm::vec3> normals = {
      glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),

      glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),

      glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),
      glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),

      glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
      glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),

      glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),

      glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
      glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f)};

  std::vector<unsigned int> indices = {
      0,  1,  2,  0,  2,  3,  4,  6,  5,  4,  7,  6,  8,  9,  10, 8,  10, 11,
      12, 14, 13, 12, 15, 14, 16, 18, 17, 16, 19, 18, 20, 21, 22, 20, 22, 23};

  Geometry *_geometry = new Geometry(vertices, uvs, normals, indices);

  _geometryList.push_back(_geometry);

  return _geometry;
}
```

然后准备天空盒着色器

#SH(title: "assets/shaders/skybox/skybox.vert")

```vert
#version 410 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUv;
layout(location = 2) in vec3 aNormal;

out vec3 uvw;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
  gl_Position = gl_Position.xyww;
  uvw = aPosition;
}
```
- 为了保证天空盒始终显示在画布最后方，使用`gl_Position = gl_Position.xyww`将天空盒深度变为1，即始终位于最后方；同时，设置默认深度测试模式为`GL_LEQUAL`，防止画布底色遮掩天空盒

#SH(title: "assets/shaders/skybox/skybox.frag")

```frag
#version 410 core
out vec4 FragColor;

in vec3 uvw;

uniform samplerCube sampler;

void main() { FragColor = texture(sampler, uvw); }
```

封装相应的材质类

#H(title: "include/game/material/skyboxMaterial.h")

```cpp
class SkyboxMaterial : public Material {
public:
  SkyboxMaterial();
  SkyboxMaterial(const SkyboxMaterial &other) : Material(other) {
    _diffuse = other._diffuse;
  }

  SkyboxMaterial* clone() const override { return new SkyboxMaterial(*this); }
  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override { _diffuse = diffuse; }

private:
  Texture *_diffuse{nullptr};
};
```

#H(title: "game/material/skyboxMaterial.cpp")

```cpp
SkyboxMaterial::SkyboxMaterial() {
  ResourceManager* resourceManager = Game::getGame()->getEngine()-> getResourceManager();
  _shader = resourceManager->loadShader(
      "assets/shaders/skybox/skybox.vert", "assets/shaders/skybox/skybox.frag");

  _depthWrite = false;
}

void SkyboxMaterial::apply(const RenderInfo &info) {
  _shader->begin();

  glm::mat4 model = info.modelInfo.model;
  model[0] = glm::vec4(1.0f, 0.0f, 0.0f, model[0].w);
  model[1] = glm::vec4(0.0f, 1.0f, 0.0f, model[1].w);
  model[2] = glm::vec4(0.0f, 0.0f, 1.0f, model[2].w);
  _shader->setUniform("model", model);
  _shader->setUniform("view",info.cameraInfo.view);
  _shader->setUniform("projection",info.cameraInfo.project);

  _diffuse->bind();
  _shader->setUniform("sampler", (int)_diffuse->getUnit());
}
```

由于天空盒要跟随相机，在游戏实现层封装天空盒对象

#H(title: "include/game/entity/skybox.h")

```cpp
class Skybox {
public:
  Skybox(const std::vector<std::string> &paths);

  void bind(GameObject *camera);

  GameObject *getSkybox() const { return _skybox; }

private:
  GameObject *_skybox{nullptr};
};
```

#H(title: "game/entity/skybox.cpp")
```cpp
Skybox::Skybox(const std::vector<std::string> &paths) {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  _skybox = new GameObject();
  Geometry *box = resourceManager->createBoxGeometry(1.0f);
  SkyboxMaterial *material = new SkyboxMaterial();
  material->setDiffuse(resourceManager->loadTexture(paths));
  _skybox->addComponent(std::make_shared<MeshComponent>(box, material));
}

void Skybox::bind(GameObject *camera) {
  if (_skybox->getParent())
    _skybox->getParent()->removeChild(_skybox);
  camera->addChild(_skybox);
}
```

=== 透明材质

由于透明材质和冯氏材质共用着色器，直接继承`PhongMaterial`类即可，实现如下

#H(title: "include/game/material/transparentMaterial.h")

```cpp
class TransparentMaterial : public PhongMaterial {
public:
  TransparentMaterial() : PhongMaterial() {
    _blend = true;
    _depthWrite = false;
  };
  TransparentMaterial(const TransparentMaterial &other) : PhongMaterial(other) {
    _alphaMap = other._alphaMap;
  }
  TransparentMaterial(const PhongMaterial &other) : PhongMaterial(other) {
    _blend = true;
    _depthWrite = false;
    _alphaMap = nullptr;
  }
  TransparentMaterial(const Material &other) : PhongMaterial(other) {
    _blend = true;
    _depthWrite = false;
    _alphaMap = nullptr;
  }

  ~TransparentMaterial() override {}

  TransparentMaterial *clone() const override {
    return new TransparentMaterial(*this);
  }

  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override {
    PhongMaterial::setDiffuse(diffuse);
  }
  void setAlphaMap(Texture *alphaMap) {
    _alphaMap = alphaMap;
    _alphaMap->setUnit(1);
  }

private:
  Texture *_alphaMap{nullptr};
};
```
- 将透明蒙版放在1号纹理单元
- 透明材质需开启颜色混合功能，并关闭深度检测，从而实现“透明”

```cpp
void TransparentMaterial::apply(const RenderInfo &info) {
  PhongMaterial::apply(info);

   _alphaMap->bind();
  _shader->setUniform("alphaMap", (int)_alphaMap->getUnit());
  _shader->setUniform("useAlphaMap", 1.0f);
}
```
- 直接调用`PhongMaterial`的`apply`方法，然后再开启透明功能，并绑定透明蒙版

=== 地形

参考@learnopengl_tessellation 中实现，使用镶嵌着色器生成地形，实现如下

#SH(title: "assets/shaders/terrain/terrain.vert")
```vert
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

void main() {
  gl_Position = vec4(aPos, 1.0);
  TexCoord = aTex;
}
```

#SH(title: "assets/shaders/terrain/terrain.tesc")
```tesc
#version 410 core

layout(vertices = 4) out;

in vec2 TexCoord[];
out vec2 TextureCoord[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

  if (gl_InvocationID == 0) {
    const int MIN_TESS_LEVEL = 4;
    const int MAX_TESS_LEVEL = 64;
    const float MIN_DISTANCE = 20;
    const float MAX_DISTANCE = 800;

    vec4 eyeSpacePos00 = view * model * gl_in[0].gl_Position;
    vec4 eyeSpacePos01 = view * model * gl_in[1].gl_Position;
    vec4 eyeSpacePos10 = view * model * gl_in[2].gl_Position;
    vec4 eyeSpacePos11 = view * model * gl_in[3].gl_Position;

    float distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

    float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
    float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
    float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
    float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

    gl_TessLevelOuter[0] = tessLevel0;
    gl_TessLevelOuter[1] = tessLevel1;
    gl_TessLevelOuter[2] = tessLevel2;
    gl_TessLevelOuter[3] = tessLevel3;

    gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
    gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
  }
}
```
- 通过计算补丁到相机的距离，动态确定细分级数，从而实现类似Mipmap的效果，在减轻GPU压力的同时避免过采样问题

#colbreak()

#SH(title: "assets/shaders/terrain/terrain.tese")
```tese
#version 410 core

layout(quads, fractional_odd_spacing, ccw) in;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix; // unused

in vec2 TextureCoord[];

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  vec2 t00 = TextureCoord[0];
  vec2 t01 = TextureCoord[1];
  vec2 t10 = TextureCoord[2];
  vec2 t11 = TextureCoord[3];
  vec2 t0 = (t01 - t00) * u + t00;
  vec2 t1 = (t11 - t10) * u + t10;
  vec2 texCoord = (t1 - t0) * v + t0;
  float height = texture(heightMap, texCoord).y * 64.0 - 32.0;

  vec4 p00 = gl_in[0].gl_Position;
  vec4 p01 = gl_in[1].gl_Position;
  vec4 p10 = gl_in[2].gl_Position;
  vec4 p11 = gl_in[3].gl_Position;
  vec4 uVec = p01 - p00;
  vec4 vVec = p10 - p00;
  vec4 normalN = normalize(vec4(cross(vVec.xyz, uVec.xyz), 0));
  vec4 p0 = (p01 - p00) * u + p00;
  vec4 p1 = (p11 - p10) * u + p10;
  vec4 p = (p1 - p0) * v + p0;
  p += normalN * height;

  uv = texCoord;
  normal = normalN.xyz;
  worldPosition = (model * p).xyz;
  gl_Position = projection * view * model * p;
}
```
- 手动对纹理坐标、高度进行双线性插值，得到当前采样点的属性数据
- 通过对高度图进行采样，生成地形；由于高度图像素值范围$[0, 1]$，需对计算结果进行放缩处理
- 为了能与冯氏模型片元着色器进行对接，声明Uniform变量`normalMatrix`，防止`SetUniform`时出错

为了使地形响应光照，直接将这些着色器与冯氏模型片元着色器@l3 进行链接，故实现材质时可直接继承`PhongMaterial`

#H(title: "include/game/material/terrainMaterial.h")

```cpp
class TerrainMaterial : public PhongMaterial {
public:
  TerrainMaterial() : PhongMaterial() { prepareShader(); }
  TerrainMaterial(const TerrainMaterial &other) : PhongMaterial(other) {
    _heightMap = other._heightMap;
  }
  TerrainMaterial(const PhongMaterial &other) : PhongMaterial(other) {
    prepareShader();
    _heightMap = nullptr;
  }
  TerrainMaterial(const Material &other) : PhongMaterial(other) {
    prepareShader();
    _heightMap = nullptr;
  }

  ~TerrainMaterial() override {}

  TerrainMaterial *clone() const override { return new TerrainMaterial(*this); }
  void apply(const RenderInfo &info) override;

  void setDiffuse(Texture *diffuse) override {
    PhongMaterial::setDiffuse(diffuse);
  }
  void setHeightMap(Texture *heightMap) {
    _heightMap = heightMap;
    _heightMap->setUnit(1);
  }

private:
  Texture *_heightMap{nullptr};

  void prepareShader();
};
```
- 将高度图放在1号纹理单元

#colbreak()

#H(title: "game/material/terrainMaterial.cpp")

```cpp
void TerrainMaterial::apply(const RenderInfo &info) {
  PhongMaterial::apply(info);

  _heightMap->bind();
  _shader->setUniform("heightMap", (int)_heightMap->getUnit());
}

void TerrainMaterial::prepareShader() {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader("assets/shaders/terrain/terrain.vert",
                                        "assets/shaders/terrain/terrain.tesc",
                                        "assets/shaders/terrain/terrain.tese",
                                        "assets/shaders/phong/phong.frag");
}
```

由于地形初始化时，需根据分辨率准备网格数据，在游戏实现层对地形对象进行封装

#H(title: "include/game/entity/terrain.h")

```cpp
class Terrain {
public:
  Terrain(float width, float height, int rez, int repeat, TerrainMaterial *material);
  ~Terrain();

  GameObject *getTerrain() const { return _terrain; }

private:
  GameObject *_terrain{nullptr};
};
```

#H(title: "game/entity/terrain.cpp")

```cpp
Terrain::Terrain(float width, float height, int rez, int repeat,
                 TerrainMaterial *material) {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  for (int i = 0; i <= rez - 1; i++) {
    for (int j = 0; j <= rez - 1; j++) {
      vertices.push_back(glm::vec3(-width / 2.0f + width * i / (float)rez, 0.0f, -height / 2.0f + height * j / (float)rez));
      uvs.push_back(glm::vec2(repeat * i / (float)rez, repeat * j / (float)rez));
      vertices.push_back(glm::vec3(-width / 2.0f + width * (i + 1) / (float)rez, 0.0f, -height / 2.0f + height * j / (float)rez));
      uvs.push_back(glm::vec2(repeat * (i + 1) / (float)rez, repeat * j / (float)rez));
      vertices.push_back(
          glm::vec3(-width / 2.0f + width * i / (float)rez, 0.0f,
                    -height / 2.0f + height * (j + 1) / (float)rez));
      uvs.push_back(glm::vec2(repeat * i / (float)rez, repeat * (j + 1) / (float)rez));
      vertices.push_back(
          glm::vec3(-width / 2.0f + width * (i + 1) / (float)rez, 0.0f,
                    -height / 2.0f + height * (j + 1) / (float)rez));
      uvs.push_back(glm::vec2(repeat * (i + 1) / (float)rez, repeat * (j + 1) / (float)rez));
    }
  }

  _terrain = new GameObject();
  Geometry *terrainGeometry =
      resourceManager->loadGeometry(std::move(vertices), std::move(uvs));
  std::shared_ptr<MeshComponent> meshComp =
      std::make_shared<MeshComponent>(terrainGeometry, material);
  std::shared_ptr<TerrainComponent> terrainComp =
      std::make_shared<TerrainComponent>(rez);
  _terrain->addComponent(meshComp);
  _terrain->addComponent(terrainComp);
}

Terrain::~Terrain() {
  if (_terrain) {
    delete _terrain;
    _terrain = nullptr;
  }
}
```
- 根据地形分辨率，从$[-"width"/2, "width"/2]times[-"height"/2, "height"/2]$生成地形
- 当地形过大时，若仍选取四角uv为$(0,0), (0, 1), (1, 0), (1, 1)$会导致地形变得过于平坦失真，通过修改纹理坐标实现地形重复密铺

=== 爆炸效果

使用几何着色器实现爆炸效果，参考@learnopengl_geometry_shader, 实现如下

#SH(title: "shaders/effect/explode.geom")

```geom
#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
  vec2 uv;
  vec3 normal;
  vec3 worldPosition;
} gs_in[];

out vec2 uv;
out vec3 normal;
out vec3 worldPosition;

uniform float time;

vec3 getNormal() {
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
  vec3 direction = normal * time;
  return position + vec4(direction, 0.0);
}

void main() {
  vec3 normalN = getNormal();

  gl_Position = explode(gl_in[0].gl_Position, normalN);
  uv = gs_in[0].uv;
  normal = gs_in[0].normal;
  worldPosition = gs_in[0].worldPosition;
  EmitVertex();
  gl_Position = explode(gl_in[1].gl_Position, normalN);
  uv = gs_in[1].uv;
  normal = gs_in[1].normal;
  worldPosition = gs_in[1].worldPosition;
  EmitVertex();
  gl_Position = explode(gl_in[2].gl_Position, normalN);
  uv = gs_in[2].uv;
  normal = gs_in[2].normal;
  worldPosition = gs_in[2].worldPosition;
  EmitVertex();
  EndPrimitive();
}
```
- 通过叉积得到每个三角面片的法向，让每个三角面的顶点朝着法向量方向偏移，从而实现爆炸效果

顶点和片元着色器都复用冯氏模型。材质继承`PhongMaterial`, 实现如下

#H(title: "include/game/material/explosionMaterial.h")

```cpp
class ExplosionMaterial : public PhongMaterial {
public:
  ExplosionMaterial() : PhongMaterial() { prepareShader(); }
  ExplosionMaterial(const ExplosionMaterial &other) : PhongMaterial(other) {
    _time = other._time;
  }
  ExplosionMaterial(const PhongMaterial &other) : PhongMaterial(other) {
    prepareShader();
    _time = .0f;
  }
  ExplosionMaterial(const Material &other) : PhongMaterial(other) {
    prepareShader();
    _time = .0f;
  }
  ~ExplosionMaterial() override {}

  ExplosionMaterial *clone() const override {
    return new ExplosionMaterial(*this);
  }
  void apply(const RenderInfo &info) override;

  void setTime(float time) { _time = time; }

private:
  float _time{0.0f};

  void prepareShader();
};
```

#H(title: "game/material/explosionMaterial.cpp")

```cpp
void ExplosionMaterial::apply(const RenderInfo &info) {
  PhongMaterial::apply(info);

  _shader->setUniform("time", _time);
}

void ExplosionMaterial::prepareShader() {
  ResourceManager *resourceManager =
      Game::getGame()->getEngine()->getResourceManager();
  _shader = resourceManager->loadShader("assets/shaders/phong/phong.vert",
                                        "assets/shaders/effect/explode.geom",
                                        "assets/shaders/phong/phong.frag");
}
```
这样，只需设置着色器的时间变量便可实现爆炸效果。