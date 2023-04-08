#ifndef MODEL_H
#define MODEL_H

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include <Mesh.h>
#include <Shader.h>
#include <opencv4/opencv2/opencv.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
//#include <vector>

using namespace std;
#define MAX_BONE_INFLUENCE 4

/// @brief  顶点结构体
/// @param  Position    顶点位置
/// @param  TexCoords   顶点纹理坐标
struct Vertex {
    // position
    float Position[3];
    // normal
    //glm::vec3 Normal;
    // texCoords
    float TexCoords[2];
    // tangent
//    glm::vec3 Tangent;
//    // bitangent
//    glm::vec3 Bitangent;
//    //bone indexes which will influence this vertex
//    int m_BoneIDs[MAX_BONE_INFLUENCE];
//    //weights from each bone
//    float m_Weights[MAX_BONE_INFLUENCE];
};
/// @brief 纹理结构体
/// @param id           纹理id
/// @param type         纹理类型
/// @param path         纹理路径
struct Texture {
    unsigned int id;
    string type;
    string path;
};

/// @brief  网格结构体
/// @param  vertices    顶点数组
/// @param  indices     顶点索引数组
/// @param  textures    纹理数组

struct Mesh{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
};

/// @brief  从png文件中加载纹理
/// 从文件中加载纹理，支持png格式，返回纹理id，纹理类型，纹理路径，纹理宽度，纹理高度，纹理通道数，纹理数据，纹理格式，纹理过滤方式，纹理环绕方式等
/// @param  path        纹理路径
/// @param  directory   纹理所在目录

inline
GLuint TextureFromFile1(const char *path, const string &directory)
{
    string filename = string(path);
    filename = directory + '/' + filename;

    //unsigned int textureID;
    //glGenTextures(1, &textureID);

    //int width, height, nrComponents;
    //QOpenGLTexture* texture;
    //unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    cv::Mat image = cv::imread(filename);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
    QImage imgQ((uchar*) image.data, image.cols, image.rows, QImage::Format_RGB888);
    QOpenGLTexture* texture = new QOpenGLTexture(imgQ);
    if (!image.empty())
    {
        std::cout << !image.empty() << std::endl;
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;

        texture->setWrapMode(QOpenGLTexture::Repeat);
        texture->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);
        texture->setWrapMode(QOpenGLTexture::ClampToEdge);

        texture->bind();

    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;

    }

    return  texture->textureId();
}

/// @brief  模型类
/// 模型类，用于加载模型，绘制模型，加载纹理等
/// @param  path        模型路径
/// @param  directory   模型所在目录
/// @param  gamma       是否开启gamma校正

class Model
{
public:
    std::vector<Texture> textures_loaded;	///> 已加载的纹理
    string directory;                      ///> 模型所在目录
    Shader* shader;                       ///> 着色器
    std::vector<Mesh> meshvector;         ///> 网格数组
    bool gammaCorrection;                ///> 是否开启gamma校正

    /// @brief  模型构造函数
    /// 模型构造函数，用于加载模型,并初始化模型,加载纹理等,并将模型的网格数组返回,用于绘制模型。
    /// @param path       模型路径
    /// @param sha      着色器
    /// @param gamma    是否开启gamma校正
    Model(string const &path,Shader* sha,bool gamma = false) : gammaCorrection(gamma) ,shader(sha)
    {
        //makeCurrent();
        loadModel(path);

    }
    /// @brief 返回模型的网格数组
    /// @return  网格数组
    std::vector<Mesh> returnmesh(){
        return this->meshvector;
    }



private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    /// @brief  加载模型
    /// 从路径加载模型,并初始化模型,加载纹理等,并将模型的网格数组返回,用于绘制模型。
    /// @param path       模型路径
    void loadModel(string const &path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    /// @brief  处理节点
    /// 处理节点,递归处理节点,并将节点的网格数组返回,用于绘制模型。
    /// @param node       节点
    /// @param scene      场景

    void processNode(aiNode *node, const aiScene *scene)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene.
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);

        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }
   /// @brief 处理网格
   /// 处理网格,并将网格的顶点数组返回,用于绘制模型。
   /// @param mesh  网格
   /// @param scene 场景
   void processMesh(aiMesh *mesh, const aiScene *scene)
    {
        // data to fill
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        // walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
             // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vertex.Position[0] = mesh->mVertices[i].x/100;
            vertex.Position[1] = mesh->mVertices[i].y/100;
            vertex.Position[2] = mesh->mVertices[i].z/100;
            // QDebug << "has normal" << mesh->HasNormals() << std::endl;
//            // normals
//            if (mesh->HasNormals())
//            {
//                vector.x = mesh->mNormals[i].x;
//                vector.y = mesh->mNormals[i].y;
//                vector.z = mesh->mNormals[i].z;
//                vertex.Normal = vector;
//            }
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {

                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vertex.TexCoords[0] = mesh->mTextureCoords[0][i].x;
                vertex.TexCoords[1] = 1.0f - mesh->mTextureCoords[0][i].y;

            }
            else{
                vertex.TexCoords[0] = 0.0f;
                vertex.TexCoords[1] = 0.0f;
            }
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        //textures = textures_loaded(mesh,)
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        Mesh tmpmesh;
        tmpmesh.vertices = vertices;
        tmpmesh.indices = indices;
        tmpmesh.textures = textures;
        meshvector.push_back(tmpmesh);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    /// @brief  加载材质纹理
    /// 加载材质纹理,并将纹理的路径返回,用于绘制模型。
    /// @param mat 材质
    /// @param type 纹理类型
    /// @param typeName 纹理类型名称
    /// @return 纹理路径
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile1(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
            }
        }
        return textures;
    }
};



#endif
