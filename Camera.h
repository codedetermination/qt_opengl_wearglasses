#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <QtOpenGLWidgets>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>

/// 定义了相机的移动选项
///
/// 定义相机移动的几个可能选项。 用作抽象以远离窗口系统特定的输入法
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

/// 相机类
///
/// 一个自由移动的相机类，可以在场景中自由移动

class Camera
{
public:
    glm::vec3 Position; ///< 相机位置
    glm::vec3 Front;    ///< 相机前方向
    glm::vec3 Up;       ///< 相机上方向
    glm::vec3 Right;    ///< 相机右方向
    glm::vec3 WorldUp;  ///< 相机世界坐标系中的上方向
    
    float Yaw;          ///< 相机偏航角
    float Pitch;        ///< 相机俯仰角
    
    float MovementSpeed;                ///< 相机移动速度
    float MouseSensitivity = 0.1;       ///< 相机鼠标灵敏度
    float Zoom;                         ///< 相机缩放

    /// @brief 相机类的构造函数
    /// @param position    相机位置
    /// @param up          相机上方向
    /// @param yaw         相机偏航角
    /// @param pitch       相机俯仰角
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    /// @brief 相机类的构造函数
    /// @param posX 相机位置x
    /// @param posY 相机位置y
    /// @param posZ 相机位置z 
    /// @param upX  相机上方向x
    /// @param upY  相机上方向y
    /// @param upZ  相机上方向z
    /// @param yaw  相机偏航角
    /// @param pitch  相机俯仰角
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    /// @brief 返回相机的观察矩阵
    ///
    /// @return  返回相机的观察矩阵
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    /// @brief  返回指定格式的相机的观察矩阵
    ///
    /// @return  返回指定格式的相机的观察矩阵
    QMatrix4x4 GetViewMatrix_Qt(){
        QMatrix4x4 matrix4_4;
        glm::mat4 tmp = glm::lookAt(Position, Position + Front, Up);
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                matrix4_4(i,j) = tmp[i][j];
            }
        }
        return matrix4_4;
    }
    template <typename T1,typename T2>
    void Transform2Qt(T1& glmdata,T2& qtdata,int length, bool isvector){

        for(int i=0;i<length;i++){
            for(int j=0;j<length;j++){
                qtdata(i,j) = glmdata[i][j];
            }
        }

    }
    template <typename T1,typename T2>
    void Transform2Qt(T1& glmdata,T2& qtdata,int length){

        for(int i=0;i<length;i++){

            qtdata(i) = glmdata[i];

        }

    }

    
    /// @brief 处理键盘输入
    /// @param direction  相机移动方向
    /// @param deltaTime  相机移动时间
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }



    /// @brief  处理鼠标输入
    /// @param xoffset 
    /// @param yoffset 
    /// @param constrainPitch 
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity * -1;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    /// @brief  处理鼠标滚轮输入
    /// @param yoffset 
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    /// @brief  更新相机的方向向量
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};



#endif // CAMERA_H
