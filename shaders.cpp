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

void Shaders::sortByT(intercept *ret, int idx)
{
    std::sort(ret, ret + idx,
                [] (intercept a, intercept b){
                    return (a.t < b.t);
                }
    );
}

float Shaders::getTotalR(intercept *ret, int idx, Function3D& obj)
{
    sortByT(ret, idx);
    float r = 0;
    intercept closest;
    closest.obj = &obj;
    closest.t = 0;
    bool counted = false;
    int* indicies = new int[MAX_LINE_INTERCEPTS];
    for(int i = 0; i < idx; i++)
    {
        indicies[i] = -1;
        for(int j = 0; j <= i; j++)
        {
            if(ret[i].obj == ret[j].obj)
            {
                indicies[j] = i;
                indicies[i] = j;
                break;
            }
            else if(ret[i].obj == &obj)
            {
                indicies[i] = -1;
                break;
            }
        }
    }
    counted = false;
    for(int i = 0; i < idx; i++)
    {
        if(counted)
        {
            closest.obj = ret[i].obj;
            closest.t = (float)i;
            counted = false;
        }
        if(ret[i].obj == closest.obj)
        {
            if(closest.obj != &obj) r += ret[i].t - ret[(int)closest.t].t;
            else r += ret[i].t;
            counted = true;
        }
        else if(ret[i].obj != closest.obj && indicies[i + 1] > i)
        {
            r += (ret[indicies[(int)closest.t] + 1].t - ret[(int)closest.t + 1].t) - (ret[indicies[(int)closest.t + 1]].t - ret[i].t);
            closest.obj = ret[i].obj;
            closest.t = (float)i;
        }
    }

    delete [] indicies;
    return r;
}

glm::vec4 Shaders::flat(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj)
{
    return obj.getCD();
}

glm::vec4 Shaders::diffuse(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj)
{
    glm::vec4 cD = obj.getCD();
    glm::vec4 cA = obj.getCA();
    glm::vec4 cL;
    glm::vec4 cPe = cA;
    float t = 0;
    float d = obj.getGeo().depth;
    float r;
    glm::vec3 pDH = pH - nH * d;
    LightBase* curLight;
    glm::vec3 nL;
    intercept hits[MAX_LINE_INTERCEPTS];
    int numHits = 0;
    for(int i = 0; i < renderer->lightNum; i++)
    {
        numHits = 0;
        curLight = renderer->lights[i];
        if(curLight->getType() == DIRECTIONAL)
        {
            nL = -curLight->getGeo().normal;
        }
        else if(curLight->getType() == POINT)
        {
            nL = glm::normalize(curLight->getPos() - pH);
        }
        else if(curLight->getType() == SPOTLIGHT)
        {
            nL = glm::normalize(curLight->getPos() - pH);
            if(glm::dot(nL, -(curLight->getGeo().normal)) > curLight->getGeo().radius) break;
        }
        cL = curLight->getColor();

        numHits = castRay(pDH, nL, hits, numHits);
        r = getTotalR(hits, numHits, obj);
        t = d / r;
        if(t > 0.9)
        {
            int x = getTotalR(hits, numHits, obj);
            numHits = 0;
            numHits = castRay(pDH, nL, hits, numHits);
        }
        glm::vec4 cDD = cD * cL;
        if(curLight->getType() != DIRECTIONAL)
        {
            cDD /= glm::dot(curLight->getPos() - pH, curLight->getPos() - pH);
        }

        cDD.r *= t;
        cDD.g *= t;
        cDD.b *= t;

        cPe += cDD;
    }

    return cPe;

}
