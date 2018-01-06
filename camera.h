#ifndef CAMERA_H
#define CAMERA_H

#include "physicsobject.h"

class Camera
{
private:
    glm::mat4* view;
    glm::mat4 rot;
    glm::mat4 pos;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 velocity;
    glm::vec3 newPositon;
    glm::vec3 newRotation;
    glm::vec3 newVelocity;
public:
    Camera();
    void setViewMatrix(glm::mat4*);
    void updateViewMatrix(float);
    void setPosition(glm::vec3);
    void setRotation(glm::vec3);
    void setVelocity(glm::vec3);
    void addVelocity(glm::vec3);
    void addRotation(glm::vec3);
    glm::vec3 getPosition();
    glm::mat4 getViewMatrix();
};

#endif // CAMERA_H
