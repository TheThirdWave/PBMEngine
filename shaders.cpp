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
    for(int i = 0; i < idx; i++)
    {
        //assumes that all objects are convex (i.e. there will be a max of 2 intercepts per object)
        if(ret[i].obj != closest.obj)
        {
            closest.obj = ret[i].obj;
            if(closest.t == -1) closest.t = ret[i].t;
        }
        else
        {
            r += closest.t + ret[i].t;
            closest.t = -1;
        }
    }
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
        glm::vec4 cDD = cD * cL;
        if(curLight->getType() != DIRECTIONAL)
        {
            cDD /= glm::dot(curLight->getPos() - pH, curLight->getPos() - pH);
        }
        cPe += cDD * t;
    }

    return cPe;

}
