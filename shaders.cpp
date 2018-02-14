#include "shaders.h"

Shaders::Shaders()
{
}

glm::vec4 Shaders::flat(glm::vec3 nH, glm::vec3 nLH, glm::vec4 cH, glm::vec4 cL)
{
    return cH;
}
