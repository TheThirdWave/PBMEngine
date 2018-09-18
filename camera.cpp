#include "camera.h"

camera::camera()
{

}

camera::camera(glm::vec3 p, glm::vec3 ld, glm::vec3 ud, float fov, float r, float n, float f)
{
    pos = p;
    lookDir = ld;
    upDir = ud;
    FOV = fov;
    zNear = n;
    zFar = f;
}

void camera::setPos(glm::vec3 p)
{
    pos = p;
}

void camera::setLookDir(glm::vec3 ld)
{
    lookDir = ld;
}

void camera::setUpDir(glm::vec3 ud)
{
    upDir = ud;
}

void camera::setRenderDistances(float n, float f)
{
    zNear = n;
    zFar = f;
}

void camera::setFOV(float fov)
{
    FOV = fov;
}

glm::vec3 camera::getPos()
{
    return pos;
}

glm::vec3 camera::getLookDir()
{
    return lookDir;
}

glm::vec3 camera::getUpDir()
{
    return upDir;
}

float camera::getFOV()
{
    return FOV;
}

float camera::getNear()
{
    return zNear;
}

float camera::getFar()
{
    return zFar;
}
