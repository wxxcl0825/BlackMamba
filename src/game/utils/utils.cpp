#include "game/utils/utils.h"

#include "common/macro.h"
#include "runtime/framework/component/transform/transform.h"
#include "runtime/resource/resourceManager.h"

GameObject *loadModel(const std::string& path, Material &material){
    GameObject *root = new GameObject();

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        Err("Assimp error: %s", importer.GetErrorString());
        return nullptr;
    }

    std::size_t lastIndex = path.find_last_of('/');
    auto rootPath = path.substr(0, lastIndex + 1);

    parse(scene->mRootNode, scene, root, rootPath, material);

    return root;
}

void parse(aiNode *ainode, const aiScene *scene, GameObject *parent, const std::string& rootPath, Material &material){
    GameObject *gameObject = new GameObject();
    
    glm::mat4 localMatrix = getMat4f(ainode->mTransformation);
    glm::vec3 position, enlerAngle, scale;
    decompose(localMatrix, position, enlerAngle, scale);

    gameObject->getComponent<TransformComponent>()->setPosition(position);
    gameObject->getComponent<TransformComponent>()->setAngle(enlerAngle);
    gameObject->getComponent<TransformComponent>()->setScale(scale);

    parent->addChild(gameObject);

    for(std::size_t i = 0; i < ainode->mNumMeshes; i++){
        aiMesh *aimesh = scene->mMeshes[ainode->mMeshes[i]];
        auto mesh = parseMesh(aimesh, scene, rootPath,material);
        auto _object = new GameObject();
        _object->addComponent(mesh);
        gameObject->addChild(_object);
    }

    for(std::size_t i = 0; i < ainode->mNumChildren; i++){
        parse(ainode->mChildren[i], scene, gameObject, rootPath, material);
    }
}

MeshComponent *parseMesh(aiMesh *aimesh, const aiScene *scene, const std::string& rootPath, Material &material){

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    for(std::size_t i = 0; i < aimesh->mNumVertices; i++){
        vertices.push_back(glm::vec3(aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z));
        normals.push_back(glm::vec3(aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z));

        if(aimesh->mTextureCoords[0]){
            uvs.push_back(glm::vec2(aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y));
        }else{
            uvs.push_back(glm::vec2(0.0f, 0.0f));
        }
    }

    for(std::size_t i = 0; i < aimesh->mNumFaces; i++){
        aiFace face = aimesh->mFaces[i];
        for(std::size_t j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }


    auto geometry = ResourceManager::getResourceManager()->loadGeometry(vertices, uvs, normals, indices);

    // if (aimesh->mMaterialIndex >= 0) {
    //     aiMaterial* aiMat = scene->mMaterials[aimesh->mMaterialIndex];
    //     material->setDiffuse(parseTexture(aiMat, aiTextureType_DIFFUSE, scene, rootPath));
    // } else {
    //     material->setDiffuse(ResourceManager::getResourceManager()->loadTexture("/assets/textures/default.png"));
    // }

    return new MeshComponent(geometry,&material);
}

Texture *parseTexture(aiMaterial *aimat, aiTextureType type, const aiScene *scene, const std::string& rootPath){
    aiString aipath;
    aimat->Get(AI_MATKEY_TEXTURE(type, 0), aipath);
    if(aipath.length == 0){
        return ResourceManager::getResourceManager()->loadTexture("/assets/textures/default.png");
    }
    const aiTexture* aitexture = scene->GetEmbeddedTexture(aipath.C_Str());
    if(aitexture){
        unsigned char* dataIn = reinterpret_cast<unsigned char*>(aitexture->pcData);
        uint32_t widthIn = aitexture->mWidth;
        uint32_t heightIn = aitexture->mHeight;
        return ResourceManager::getResourceManager()->loadTexture(aipath.C_Str(), dataIn, widthIn, heightIn);
    } else {
        std::string path = rootPath + aipath.C_Str();
        return ResourceManager::getResourceManager()->loadTexture(path);
    }
}

glm::mat4 getMat4f(aiMatrix4x4 value){
    return glm::mat4(
        value.a1, value.a2, value.a3, value.a4,
        value.b1, value.b2, value.b3, value.b4,
        value.c1, value.c2, value.c3, value.c4,
        value.d1, value.d2, value.d3, value.d4
    );
}

void decompose(glm::mat4 matrix, glm::vec3& position, glm::vec3& eulerAngle, glm::vec3& scale){
    glm::quat quaternion;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, quaternion, position, skew, perspective);

    glm::mat4 rotation = glm::toMat4(quaternion);
    glm::extractEulerAngleXYZ(rotation, eulerAngle.x, eulerAngle.y, eulerAngle.z);

    eulerAngle.x = glm::degrees(eulerAngle.x);
    eulerAngle.y = glm::degrees(eulerAngle.y);
    eulerAngle.z = glm::degrees(eulerAngle.z);
}