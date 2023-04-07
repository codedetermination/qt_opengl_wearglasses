#ifndef SHADER_H
#define SHADER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <QtOpenGLWidgets>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>

class Shader : public QOpenGLWidget, public QOpenGLFunctions{
    Q_OBJECT
public:
    QOpenGLShaderProgram* shader;

    Shader(const char* vertexPath,const char* fragmentPath,const char* geometryPath = nullptr){
        initializeOpenGLFunctions();
        makeCurrent();
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if(geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char * fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        shader = new QOpenGLShaderProgram(this);
        QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
        vshader->compileSourceCode(vShaderCode);
        QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
        fshader->compileSourceCode(fShaderCode);
        shader->addShader(vshader);
        shader->addShader(fshader);

    }
    void Use(){
        shader->link();
    }
    //set many uniform value use one template function.
    template<typename T>
    void SetUniformValue(const char* name,T& value){
        shader->setUniformValue(name,value);
    }
    void bind(){
        shader->bind();
    }
};



#endif // SHADER_H
