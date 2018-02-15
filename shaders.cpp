#include "shaders.h"
#include "imagemanip.h"

Shaders::Shaders()
{
}

void Shaders::setRenderer(Imagemanip *ptr)
{
    renderer = ptr;
}

int Shaders::castRay(glm::vec3 pE, glm::vec3 nPE, intercept ret[], int idx)
{
    for(int i = 0; i < renderer->func3DNum; i++)
    {
        idx = renderer->functions3D[i]->getRelativeLine(pE, nPE, ret, idx);
    }
    return idx;
}

glm::vec4 Shaders::flat(glm::vec3 nH, glm::vec3 nLH, glm::vec4 cH, glm::vec4 cL, float d)
{
    return cH;
}

glm::vec4 Shaders::diffuse(glm::vec3 nH, glm::vec3 nLH, glm::vec4 cH, glm::vec4 cL, float d)
{
    return cH;

}
