#include "camera.h"

Camera::Camera()
{
}

void Camera::setViewMatrix(glm::mat4* v)
{
    view = v;
}

void Camera::updateViewMatrix()
{
    pos = glm::translate(glm::mat4(1.0f), position);
    rot = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
    *view = rot * pos;
}

void Camera::addVelocity(glm::vec3 nv)
{
    glm::vec4 b = glm::transpose(rot) * glm::vec4(nv, 1.0f);
    velocity = velocity + glm::vec3(b.x, b.y, b.z);
}

glm::mat4 Camera::getViewMatrix()
{

    return *view;
}