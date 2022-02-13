#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
    CAM_FORWARD = 1,
    CAM_BACKWARD = 2,
    CAM_RIGHT = 4,
    CAM_LEFT = 8,
    CAM_UP = 16,
    CAM_DOWN = 32
};

const float YAW = 90.0f; // поворот влево/вправо в горизонтальной плоскости
const float PITCH = 0.0f; // угол наклона вперёд/назад

const float SPEED = 2.5f; 
const float SENSITIVITY = 0.05f; // Чувствительность к повороту

const float FOV = 45.0f; 
const float ZNEAR = 0.1f;
const float ZFAR = 1000.f;
const float ASPECTRATIO = 16.f / 9.f;


class Camera
{
public:
    glm::vec3 Position; // Вектор позиции камеры
    glm::vec3 Front; // Вектор куда смотрит камера
    glm::vec3 Up; // Где находится верх у камеры
    glm::vec3 Right;
    glm::vec3 WorldUp; // Вверх мировой

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float Sensitivity;

    float Fov;
    float zNear;
    float zFar;
    float AspectRatio;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);

    Camera(float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch);

    glm::mat4 GetViewMatrix(); // Матрица взгляда

    glm::mat4 GetProjectionMatrix(); // Матрица перспективы

    void Move(int32_t direction, float deltaTime);

    void Rotate(float xoffset, float yoffset, GLboolean constrainPitch = true);

    void ChangeFOV(double value);

    void UpdateCameraVectors();
};
#endif