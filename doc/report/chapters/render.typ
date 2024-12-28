= 渲染系统
== 资源管理系统
=== 背景介绍
由于游戏中的资源种类繁多，且存在大量重复与需要写入GPU的资源(这些资源需要合理地从GPU上释放)，因此需要一个专门的系统————资源管理系统来管理这些资源。资源管理系统的主要功能是加载、管理和释放资源，以及提供资源的查询接口，其内部实现是很多个资源池，按照资源类型进行分类，每种资源类型都有一个对应的资源池。
=== 资源介绍
==== Texture
纹理资源是游戏中最常用的资源之一，用于渲染模型的表面，其在渲染时会绑定到一个纹理单元上参与运算，其主要属性有宽度，高度，纹理单元序号等，其定义如下：
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

其在初始化时，通过传入纹理的路径或者数据，来创建一个纹理对象，其内部会调用OpenGL的API来生成纹理对象，并将纹理数据写入GPU中。在渲染时，通过调用bind函数来绑定纹理到指定的纹理单元上。这些实现代码如下：
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
    // activate the texture unit first before binding texture
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _textureID));

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    stbi_image_free(data);

    Log("Texture created: %s,and wrapping/filtering options set here", path.c_str());
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    // set the texture filtering parameters
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

}

// for muitiple pictures in skybox
// right left top bottom back front
Texture::Texture(const std::vector<std::string>& paths, unsigned int unit) : _unit(unit){
    _textureTarget = GL_TEXTURE_CUBE_MAP;

    stbi_set_flip_vertically_on_load(false);

    GL_CALL(glGenTextures(1, &_textureID));
    // activate the texture unit first before binding texture
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

    Log("Texture created: %s,and wrapping/filtering options set here", paths[0].c_str());
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CALL(glTexParameteri(_textureTarget, GL_TEXTURE_WRAP_R, GL_REPEAT));

    // set the texture filtering parameters
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
    // activate the texture unit first before binding texture
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
==== Geometry
几何体资源是游戏中的另一种常用资源，用于渲染模型的形状，其在渲染时会将所需数据按照对应格式传入GPU参与运算，其主要属性有VAO（储存数据格式），VBO（储存顶线，uv等信息）和EBO（顶点索引信息）等，其定义如下：
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
==== Shader
着色器资源是管线中的一个重要组成部分，用于控制渲染的流程，其储存着代码片段，在渲染时传入GPU进行编译和链接，需要维护着色器的ID，在析构时，需要根据着色器ID释放着色器资源，其定义如下：
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

==== Audio


