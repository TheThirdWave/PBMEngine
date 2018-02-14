#ifndef SHADERS_H
#define SHADERS_H

#include "structpile.h"

class Shaders
{
public:
    Shaders();
    glm::vec4 flat(glm::vec3 nH, glm::vec3 nLH, glm::vec4 cH, glm::vec4 cL);
};

#endif // SHADERS_H
