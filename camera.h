#ifndef CAMERA_H
#define CAMERA_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include <../glm-0.9.9.1/glm/gtc/matrix_transform.hpp>
#include "structpile.h"

class camera
{
public:
    camera();
    camera(glm::vec3 p, glm::vec3 ld, glm::vec3 ud, float fov, float r, float n, float f);

    void setPos(glm::vec3 p);
    void setLookDir(glm::vec3 ld);
    void setUpDir(glm::vec3 ud);
    void setFOV(float fov);
    void setRenderDistances(float n, float f);

    glm::vec3 getPos();
    glm::vec3 getLookDir();
    glm::vec3 getUpDir();
    float getFOV();
    float getNear();
    float getFar();

private:
    glm::vec3 pos;
    glm::vec3 lookDir;
    glm::vec3 upDir;
    float FOV; //FOV in radians.
    float zNear;
    float zFar;
};

#endif // CAMERA_H
