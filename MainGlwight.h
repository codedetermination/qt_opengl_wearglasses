#ifndef MAINGLWIGHT_H
#define MAINGLWIGHT_H


#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QTimer>
#include <opencv4/opencv2/opencv.hpp>
#include <QMouseEvent>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <QProgressDialog>

#include "points_68.h"
#include "Shader.h"
#include "Camera.h"

#include "Model.h"

#include <filesystem>





class MainGlwight : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    MainGlwight(QWidget* parent = nullptr)
        : QOpenGLWidget(parent)
    {
       //memset(texture,0,sizeof(texture));
       //setSurfaceType( OpenGLSurface );

       makeCurrent();

    }
~MainGlwight()
    {
        makeCurrent();
        //connect(this, SIGNAL(destroyed()), timer, SLOT(stop()));  // 窗口关闭时触发停止定时器
        delete shader;
        //delete m_texture;
    }
public :
    std::vector<Mesh> mesh2print;
    points_68* keypointface;
    QOpenGLVertexArrayObject VAO;
    unsigned int VBO, EBO;
    void update_scence(){
        update();
    }
    void setupMesh(std::vector<Vertex> vertices,std::vector<unsigned int>indices)
            //std::vector<Vertex> vertices,std::vector<unsigned int>indices)
    {
        VAO.bind();

        // create buffers/arrays

        //glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        //std::cout << vertices << std::endl;
        //glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


        std::cout << "size" << sizeof(float) << "sizeof(vertices)" <<sizeof(vertices[0])  <<"sizeof(indices)" <<sizeof(indices[0])<< std::endl;
        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // color attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        VAO.bind();
        //VAO.vertexData->vertexDeclaration = glm::vertex_declaration(glm::vertex_size(0), glm::format(glm::format_type::format_float_vector, 3), glm::elements_count(position), glm::format_type::format_float_vector);


    }
    void Draw(std::vector<Texture> textures,std::vector<unsigned int>indices)
    {

        for(int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            std::cout<< "draw " << std::endl;
            // now set the sampler to the correct texture unit
            shader->SetUniformValue<int>("texture1",i);
            //glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            shader->bind();
        }



        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);

    }



protected:
    void initializeGL() override
    {
        cap = cv::VideoCapture();
        cap.open(0);
        camera = new Camera();
        initializeOpenGLFunctions();

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glShadeModel(GL_SMOOTH);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);

        const char* shadervs_name = "./shader.vs";
        const char* shaderfs_name = "./shader.fs";

        shader = new Shader(shadervs_name,shaderfs_name);
        shader->Use();
        VAO.create();
        //Model ourModel("resources/objects/backpack/backpack.obj");
    }

    void paintGL() override
    {

        if(shoot){
            cap >> image;
        }


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        glActiveTexture(GL_TEXTURE0);



        if(show_cam){
            cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
            QImage imgQ((uchar*) image.data, image.cols, image.rows, QImage::Format_RGB888);
            QOpenGLTexture* texture = new QOpenGLTexture(imgQ);
            texture->setWrapMode(QOpenGLTexture::Repeat);
            texture->bind();

            int texture1_id = 0;
            shader->SetUniformValue<int>("texture1",texture1_id);
            shader->bind();
            //glUniform1i(glGetUniformLocation(, "texture1"), 0);
            float vertices[] = {
                    // positions                 // texture coords
                     1.0f,  1.0f, 0.0f,      1.0f, 1.0f, // top right
                     1.0f, -1.0f, 0.0f,      1.0f, 0.0f, // bottom right
                    -1.0f, -1.0f, 0.0f,      0.0f, 0.0f, // bottom left
                    -1.0f,  1.0f, 0.0f,      0.0f, 1.0f  // top left
                };
            unsigned int indices[] = {
                0, 1, 3, // first triangle
                1, 2, 3  // second triangle
            };



            //VAO.create();
            VAO.bind();

            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);


            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // color attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glm::mat4 model(1.0f);
            model = glm::rotate(model,glm::radians(0.0f),glm::vec3(0.0f,0.0f,1.0f));
            //QMatrix4x4 model_qt;
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(model,model_qt,4,true);
            shader->SetUniformValue<QMatrix4x4>("model",model_qt);

            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::rotate(model,glm::radians(0.0f),glm::vec3(0.0f,0.0f,1.0f));
            //QMatrix4x4 projection_qt;
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(projection,projection_qt,4,true);
            shader->SetUniformValue<QMatrix4x4>("projection",projection_qt);


            glm::mat4 view = glm::mat4(1.0f);
            view = glm::rotate(model,glm::radians(0.0f),glm::vec3(0.0f,0.0f,1.0f));
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(view,view_qt,4,true);

            //QMatrix4x4 view_qt ;
            shader->SetUniformValue<QMatrix4x4>("view",view_qt);


            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else{
            Model ourModel("./texture/out1.obj",shader);
            mesh2print = ourModel.returnmesh();

            //std::cout<< "hello" << std::endl;


            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            //model = glm::translate(model, cubePositions[i]);

            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            //QMatrix4x4 model_qt;
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(model,model_qt,4,true);
            shader->SetUniformValue<QMatrix4x4>("model",model_qt);

            glm::mat4 projection = glm::mat4(1.0f);
            //QMatrix4x4 projection_qt;
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(projection,projection_qt,4,true);
            shader->SetUniformValue<QMatrix4x4>("projection",projection_qt);


            glm::mat4 view = glm::mat4(1.0f);
            view = camera->GetViewMatrix();
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(view,view_qt,4,true);

            //QMatrix4x4 view_qt ;
            shader->SetUniformValue<QMatrix4x4>("view",view_qt);
            //ourModel.Draw(*shader);
            for(int i=0;i<mesh2print.size();i++){
                Mesh mesh_tmp = mesh2print[i];
                std::vector<Texture> textures;
                textures = mesh_tmp.textures;
                std::vector<unsigned int> indices;
                indices = mesh_tmp.indices;
                std::vector<Vertex> vertices;
                vertices = mesh_tmp.vertices;
                setupMesh(vertices,indices);
                Draw(textures,indices);
                std::cout << "vertices:" << vertices.size()<<"textures:" <<textures.size() << "indices:" << indices.size() << std::endl;
            }

            Model ourModel1("./texture/glasses1.obj",shader);
            mesh2print = ourModel1.returnmesh();
            for(int i=0;i<mesh2print.size();i++){
                Mesh mesh_tmp = mesh2print[i];
                std::vector<Texture> textures;
                textures = mesh_tmp.textures;
                std::vector<unsigned int> indices;
                indices = mesh_tmp.indices;
                std::vector<Vertex> vertices;
                vertices = mesh_tmp.vertices;
                setupMesh(vertices,indices);
                Draw(textures,indices);
                std::cout << "vertices:" << vertices.size()<<"textures:" <<textures.size() << "indices:" << indices.size() << std::endl;
            }

        }
        if(true){

        }
        else{
            float vertices[] = {
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

                    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

                    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
                };
                // world space positions of our cubes
            glm::vec3 cubePositions[] = {
                glm::vec3( 0.0f,  0.0f,  0.0f),
                glm::vec3( 2.0f,  5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3( 2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f,  3.0f, -7.5f),
                glm::vec3( 1.3f, -2.0f, -2.5f),
                glm::vec3( 1.5f,  2.0f, -2.5f),
                glm::vec3( 1.5f,  0.2f, -1.5f),
                glm::vec3(-1.3f,  1.0f, -1.5f)
            };

            unsigned int VBO;
            QOpenGLVertexArrayObject VAO;
            //VAO.create();
            VAO.bind();

            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // TEXTURE
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);


            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            //model = glm::translate(model, cubePositions[i]);

            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            //QMatrix4x4 model_qt;
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(model,model_qt,4,true);
            shader->SetUniformValue<QMatrix4x4>("model",model_qt);

            glm::mat4 projection = glm::mat4(1.0f);
            //QMatrix4x4 projection_qt;
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(projection,projection_qt,4,true);
            shader->SetUniformValue<QMatrix4x4>("projection",projection_qt);


            glm::mat4 view = glm::mat4(1.0f);
            view = camera->GetViewMatrix();
            camera->Transform2Qt<glm::mat4,QMatrix4x4>(view,view_qt,4,true);

            //QMatrix4x4 view_qt ;
            shader->SetUniformValue<QMatrix4x4>("view",view_qt);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }



//        float vertices[] = {
//                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//                 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
//                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

//                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
//                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

//                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//                -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//                -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

//                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//                 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//                 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//                 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

//                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//                 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
//                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//                -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

//                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//                -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
//                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
//            };
//            // world space positions of our cubes
//        glm::vec3 cubePositions[] = {
//            glm::vec3( 0.0f,  0.0f,  0.0f),
//            glm::vec3( 2.0f,  5.0f, -15.0f),
//            glm::vec3(-1.5f, -2.2f, -2.5f),
//            glm::vec3(-3.8f, -2.0f, -12.3f),
//            glm::vec3( 2.4f, -0.4f, -3.5f),
//            glm::vec3(-1.7f,  3.0f, -7.5f),
//            glm::vec3( 1.3f, -2.0f, -2.5f),
//            glm::vec3( 1.5f,  2.0f, -2.5f),
//            glm::vec3( 1.5f,  0.2f, -1.5f),
//            glm::vec3(-1.3f,  1.0f, -1.5f)
//        };

//        unsigned int VBO;
//        QOpenGLVertexArrayObject VAO;
//        VAO.create();
//        VAO.bind();

//        glGenBuffers(1, &VBO);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(0);
//        // TEXTURE
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//        glEnableVertexAttribArray(1);


//        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
//        //model = glm::translate(model, cubePositions[i]);

//        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//        //QMatrix4x4 model_qt;
//        camera->Transform2Qt<glm::mat4,QMatrix4x4>(model,model_qt,4,true);
//        shader->SetUniformValue<QMatrix4x4>("model",model_qt);

//        glm::mat4 projection = glm::mat4(1.0f);
//        //QMatrix4x4 projection_qt;
//        camera->Transform2Qt<glm::mat4,QMatrix4x4>(projection,projection_qt,4,true);
//        shader->SetUniformValue<QMatrix4x4>("projection",projection_qt);


//        glm::mat4 view = glm::mat4(1.0f);
//        view = camera->GetViewMatrix();
//        camera->Transform2Qt<glm::mat4,QMatrix4x4>(view,view_qt,4,true);

//        //QMatrix4x4 view_qt ;
//        shader->SetUniformValue<QMatrix4x4>("view",view_qt);

//        glDrawArrays(GL_TRIANGLES, 0, 36);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    }

    void resizeGL(int w, int h) override
    {
        glViewport(0, 0, w, h);
    }

    void mousePressEvent(QMouseEvent *event) override{
        if(event->button() == Qt::LeftButton && show_cam == false){

            //glm::mat4 view = glm::mat4(1.0f);
            //view = glm::rotate(view,glm::radians(event->angle()),glm::vec3(1.0f,0.3f,0.5f));
            lastxpos = event->globalPosition().x();
            lastypos = event->globalPosition().y();
        }
        std::cout<< "press" << std::endl;
    }

    void mouseMoveEvent(QMouseEvent *event) override{
        if(!show_cam){
            xpos = event->globalPosition().x() - lastxpos;
            ypos = event->globalPosition().y() - lastypos;

            camera->ProcessMouseMovement(xpos,ypos);

            //view = glm::rotate(view,glm::radians(event->globalPosition()),glm::vec3(1.0f,0.3f,0.5f));
            std::cout<< "move" << xpos/10 << ", y:"<< ypos/10 << std::endl;
        }
        //std::cout<< "move" << lastxpos << lastypos << std::endl;
    }

    void mouseReleaseEvent(QMouseEvent *event) override{
        if(event->button() == Qt::LeftButton && show_cam == false){
            camera->ProcessMouseMovement(0,0);
            std::cout<< "release"  << std::endl;
        }
    }
public:
    void shoot_image(){
        cap >> image;
        shoot = false;
    }

    void reshoot(){
        cap >> image;
        shoot = true;
    }

    void show_camera(){
        show_cam = false;
        image_cpy = image.clone();
    }

    void reshow_camera(){
        show_cam = true;
    }
    cv::Mat get_image(){
        return image_cpy;
    }
    bool setup_points68(){
        keypointface = new points_68(image);

        cv::Mat image_tmp = keypointface->return_image();

        return !image_tmp.empty();
    }


private:
    //QOpenGLTexture* texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
//    QOpenGLShaderProgram* shader;
    QMatrix4x4 model_qt;
    QMatrix4x4 projection_qt;
    QMatrix4x4 view_qt ;

    float xpos,ypos,lastxpos,lastypos,angle = 0.0f ;
    glm::mat4 view = glm::mat4(1.0f);

    Camera* camera;
    Shader* shader;
    QTimer* timer;
    cv::VideoCapture cap;
    GLuint m_texture;
    GLint m_posAttr = 0;
    GLint m_colAttr = 0;
    cv::Mat image;
    cv::Mat image_cpy;

    float test = 0;
    bool shoot = true;
    bool show_cam = true;

};



#endif // MainGlwight_H
