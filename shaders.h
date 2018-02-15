#ifndef SHADERS_H
#define SHADERS_H

#include "structpile.h"

class Imagemanip;

class Shaders
{
    Imagemanip* renderer;
public:
    Shaders();
    void setRenderer(Imagemanip*);
    int castRay(glm::vec3 pE, glm::vec3 nPE, intercept* ret, int idx);
    glm::vec4 flat(glm::vec3 nH, glm::vec3 nLH, glm::vec4 cH, glm::vec4 cL, float d);
    glm::vec4 diffuse(glm::vec3 nH, glm::vec3 nLH, glm::vec4 cH, glm::vec4 cL, float d);
};

#endif // SHADERS_H
