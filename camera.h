#ifndef CAMERA_H
#define CAMERA_H

#include "physicsobject.h"

class Camera:public PhysicsObject
{
private:
    glm::mat4* view;
    glm::mat4 rot;
    glm::mat4 pos;
public:
    Camera();
    void setViewMatrix(glm::mat4*);
    void updateViewMatrix();
    void addVelocity(glm::vec3);
    glm::mat4 getViewMatrix();
};

#endif // CAMERA_H
