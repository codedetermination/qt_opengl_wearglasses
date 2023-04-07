#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader.h>
#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh :public QOpenGLWidget, public QOpenGLFunctions{
    Q_OBJECT
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    QOpenGLVertexArrayObject VAO;
    Shader* shader;
    // constructor
    Mesh(Shader* sha,std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        makeCurrent();
        this->shader = sha;
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }
    ~Mesh(){

    }

    // render the mesh
    void Draw(Shader &shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
//            string number;
//            string name = textures[i].type;
//            if(name == "texture_diffuse")
//                number = std::to_string(diffuseNr++);
//            else if(name == "texture_specular")
//                number = std::to_string(specularNr++); // transfer unsigned int to string
//            else if(name == "texture_normal")
//                number = std::to_string(normalNr++); // transfer unsigned int to string
//             else if(name == "texture_height")
//                number = std::to_string(heightNr++); // transfer unsigned int to string
//             shader.SetUniformValue<unsigned int>((name + number).c_str(),i);
            // now set the sampler to the correct texture unit
            shader.SetUniformValue<unsigned int>("texture1",i);
            //glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        //glBindVertexArray(VAO);
        //shader.bind();
        VAO.bind();
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data
    unsigned int VBO, EBO;


    // initializes all the buffer objects/arrays
    void setupMesh()
    {


       VAO.create();

        // create buffers/arrays

        //glGenVertexArrays(1, &VAO);
        shader->glGenBuffers(1, &VBO);
        shader->glGenBuffers(1, &EBO);

        //glBindVertexArray(VAO);
        // load data into vertex buffers
        shader->glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.

        shader->glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        shader->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        shader->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        shader->glEnableVertexAttribArray(0);
        shader->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        shader->glEnableVertexAttribArray(1);
        shader->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        shader->glEnableVertexAttribArray(2);
        shader->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        shader->glEnableVertexAttribArray(3);
        shader->glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        shader->glEnableVertexAttribArray(4);
        shader->glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // ids
        shader->glEnableVertexAttribArray(5);
        shader->glVertexAttribPointer(5,4,GL_INT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, m_BoneIDs));

        //glVertexAttribIPointer(5, 4, GL_INT, GL_FALSE,sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
        //glVertexAttribIPointer(5, 4, GL_FLOAT,  GL_FALSE,sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

        // weights
        shader->glEnableVertexAttribArray(6);
        shader->glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

        //glBindVertexArray(0);
        //VAO.bind();
    }
};
#endif

