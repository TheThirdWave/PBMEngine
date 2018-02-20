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

float Shaders::clamp(float cos, float max, float min)
{
    float x = (cos - min) / (max - min);
    if(x > 1) return 1;
    if(x < 0) return 0;
    else return x;
}

void Shaders::sortByT(intercept *ret, int idx)
{
    std::sort(ret, ret + idx,
                [] (intercept a, intercept b){
                    return (a.t < b.t);
                }
    );
}

//requires array sorted by T
float Shaders::getTotalR(intercept *ret, int idx, Function3D& obj)
{
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
            if(ret[i].obj == &obj)
            {
                indicies[i] = -1;
                break;
            }
            else if(ret[i].obj == ret[j].obj)
            {
                indicies[j] = i;
                indicies[i] = j;
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
        else if(ret[i].obj == closest.obj)
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

//requires array sorted by T
float Shaders::cullForPLight(intercept *ret, int idx, glm::vec3 pH, LightBase* L)
{
    glm::vec3 pLH = L->getPos() - pH;
    float length = glm::length(pLH);
    for(int i = 0; i < idx; i++)
    {
        if(ret[i].t > length)
        {
            return i;
        }
    }
    return idx;
}

glm::vec4 Shaders::outline(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj)
{
    glm::vec4 cPe(0.0f);
    if(glm::dot(nH, nPe) < obj.getGeo().width)cPe = (glm::vec4(100.0f, 100.0f, 100.0f, 100.0f));
    return cPe;
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
        }
        cL = curLight->getColor();

        numHits = castRay(pDH, nL, hits, numHits);
        sortByT(hits, numHits);
        numHits = cullForPLight(hits, numHits, pH, curLight);
        r = hits[0].t;
        if(r == 0) t = 0;
        else t = d / r;
        if(pH.x >= 97.0 && pH.x < 98 && pH.y == 100 && pH.z <= -16 && pH.z > -17)
        {
            int x = getTotalR(hits, numHits, obj);
            numHits = 0;
            numHits = castRay(pDH, nL, hits, numHits);
        }
        glm::vec4 cDD = cD * cL;
        if(curLight->getType() != DIRECTIONAL)
        {
            //float dnom = glm::dot(curLight->getPos() - pH, curLight->getPos() - pH);
            //t = t / dnom * 10000;
        }
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            t *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        t = clamp(t, 1.0, 0.0);
        cDD.r *= t;
        cDD.g *= t;
        cDD.b *= t;

        cPe += cDD;
    }

    return cPe;

}

glm::vec4 Shaders::phong(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj)
{
    glm::vec4 cD = obj.getCD();
    glm::vec4 cA = obj.getCA();
    glm::vec4 cS = obj.getCS();
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
        }
        cL = curLight->getColor();

        //get angle for diffuse light, raycast can catch occluders as well.
        numHits = castRay(pDH, nL, hits, numHits);
        sortByT(hits, numHits);
        numHits = cullForPLight(hits, numHits, pH, curLight);
        r = hits[0].t;
        if(r == 0) t = 0;
        else t = d / r;
        if(pH.x >= 97.0 && pH.x < 98 && pH.y == 100 && pH.z <= -16 && pH.z > -17)
        {
            int x = getTotalR(hits, numHits, obj);
            numHits = 0;
            numHits = castRay(pDH, nL, hits, numHits);
        }
        glm::vec4 cDD = cD * cL;
        if(curLight->getType() != DIRECTIONAL)
        {
            //float dnom = glm::dot(curLight->getPos() - pH, curLight->getPos() - pH);
            //t = t / dnom * 10000;
        }
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            t *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        t = clamp(t, 1.0, 0.0);
        cDD.r *= t;
        cDD.g *= t;
        cDD.b *= t;

        cPe += cDD;

        //calculate angle for specular highlight
        glm::vec3 ref = -nL + (2 * glm::dot(nH, nL) * nH);
        float cos = glm::dot(nPe, ref);
        float s = clamp(cos, obj.getGeo().radius, obj.getGeo().width);
        //if(cos > 0.98) s = 1;
        //else s = 0;
        cPe += cS * cL * s;

    }

    return cPe;

}


glm::vec4 Shaders::diffuseShadow(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj)
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
        }
        cL = curLight->getColor();

        numHits = castRay(pDH, nL, hits, numHits);
        sortByT(hits, numHits);
        numHits = cullForPLight(hits, numHits, pH, curLight);
        r = getTotalR(hits, numHits, obj);
        if(r == 0) t = 0;
        else t = d / r;
        if(pH.x >= 97.0 && pH.x < 98 && pH.y == 100 && pH.z <= -16 && pH.z > -17)
        {
            int x = getTotalR(hits, numHits, obj);
            numHits = 0;
            numHits = castRay(pDH, nL, hits, numHits);
        }
        glm::vec4 cDD = cD * cL;
        if(curLight->getType() != DIRECTIONAL)
        {
            //float dnom = glm::dot(curLight->getPos() - pH, curLight->getPos() - pH);
            //t = t / dnom * 10000;
        }
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            t *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        t = clamp(t, 1.0, 0.0);
        cDD.r *= t;
        cDD.g *= t;
        cDD.b *= t;

        cPe += cDD;
    }

    return cPe;

}
