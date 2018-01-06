#include "camera.h"

Camera::Camera()
{
}

void Camera::setViewMatrix(glm::mat4* v)
{
    view = v;
}

void Camera::updateViewMatrix(float ts)
{
    position += velocity * ts;
    glm::vec3 hold = rotation;
    pos = glm::translate(glm::mat4(1.0f), getPosition());
    rot = glm::rotate(glm::mat4(1.0f), hold.z, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::rotate(glm::mat4(1.0f), hold.y, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), hold.x, glm::vec3(0.0f, 1.0f, 0.0f));
    *view = rot * pos;
}

void Camera::addVelocity(glm::vec3 nv)
{
    glm::vec4 b = glm::transpose(rot) * glm::vec4(nv, 1.0f);
    setVelocity(velocity + glm::vec3(b.x, b.y, b.z));
}

void Camera::addRotation(glm::vec3 nr)
{
    rotation += nr;
}

void Camera::setPosition(glm::vec3 pos)
{
    position = pos;
}

void Camera::setRotation(glm::vec3 rot)
{
    rotation = rot;
}

void Camera::setVelocity(glm::vec3 vel)
{
    velocity = vel;
}

glm::vec3 Camera::getPosition()
{
    return position;
}

glm::mat4 Camera::getViewMatrix()
{

    return *view;
}
