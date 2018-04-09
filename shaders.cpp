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
            else if(ret[i].obj == ret[j].obj || ret[i].obj->getParent() == ret[j].obj->getParent())
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
        else if(ret[i].obj == closest.obj || ret[i].obj->getParent() == closest.obj->getParent())
        {
            if(closest.obj != &obj) r += ret[i].t - ret[(int)closest.t].t;
            else r += ret[i].t;
            counted = true;
        }
        else if(ret[i].obj != closest.obj && indicies[i + 1] > i)
        {
            if(indicies[(int)closest.t + 1] < idx)
            {
                r += (ret[indicies[(int)closest.t] + 1].t - ret[(int)closest.t + 1].t) - (ret[indicies[(int)closest.t + 1]].t - ret[i].t);
                closest.obj = ret[i].obj;
                closest.t = (float)i;
            }
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

//The second simplest shader, we get the cosign of the surface normal and the normal pointing from the point hit back to the camera.
//If the cosign is less than some value, we set that pixel to white, getting the outline of the object hit (or something close to that).
glm::vec4 Shaders::outline(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    glm::vec4 cPe(0.0f);
    if(glm::dot(nH, nPe) < obj.getGeo().width)cPe = (glm::vec4(100.0f, 100.0f, 100.0f, 100.0f));
    return cPe;
}

//The simplest shader, we just return the color of the object hit.
glm::vec4 Shaders::flat(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    return obj.getCD();
}

//This changes the color of the point hit based on whether or not that point is facing towards the light.
glm::vec4 Shaders::diffuse(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    //get the diffuse color of the object.
    glm::vec4 cD = obj.getCD();
    //get the ambient light color of the object.
    glm::vec4 cA = obj.getCA();
    //The color of the light being shined on theobject.
    glm::vec4 cL;
    //the final color of the object, we set it to cA since that's always there.
    glm::vec4 cPe = cA;
    //t takes on a different connotaton in this method, here it is a float from 0-1 that says how bright the diffuse light is
    //at the point on the object being shaded, 0 means no diffuse light, 1 means the maximum amount of diffuse light.
    float t = 0;
    //normally, the way you'd get t is by getting the cosign of the surface normal nH with the normal pointing towards the light
    //nL.  However, here I implement Dr. Ackleman's variation on that idea by casting a ray from some point below the surface
    //of the object being shaded to the light we're shading, and then dividing the distance underneath the surface of the point
    //d, by the total length of the cast ray that passes through an occluding object r, to get a pseudo cosign that gets larger
    //the greater the distance beneath the surface, d, is.  This gives a cheap, rough approximation of subsurface scattering.
    float d = obj.getGeo().depth;
    float r;
    //pDH = the actual point beneath the surface of the object.
    glm::vec3 pDH = pH - nH * d;
    //curLight = the current light we're shading for.
    LightBase* curLight;
    //nL = the normal pointing from pDH to the light.
    glm::vec3 nL;
    //The hits recorded by a cast ray.
    intercept hits[MAX_LINE_INTERCEPTS];
    //the number of hits recorded by a cast ray.
    int numHits = 0;
    //we loop through the lights in the scene to calcuate the final color.
    for(int i = 0; i < renderer->lightNum; i++)
    {
        //reset the number of hits.
        numHits = 0;
        //set the current light.
        curLight = renderer->lights[i];
        //we get the current normal from the light.
        nL = -curLight->getRelativeNorm(pH);
        //we get the color of thelight.
        cL = curLight->getColor();

        //we actually cast the ray from pDH along nL.
        numHits = castRay(pDH, nL, hits, numHits);
        //We sort the hits from closest to farthest.
        sortByT(hits, numHits);
        //We get rid of any hits that are beyond the actual light (objects behind a light cannot cast shadows.)
        numHits = cullForPLight(hits, numHits, pH, curLight);
        //the t value of the closest hit is also the distance from the point underneath the surface of the object pDH to
        //the point on the surface of the object closest to the light along the ray.  If we're not doing shadows, this is
        //all we need to do with r.
        r = hits[0].t;
        //if r == 0  we set t to 0, otherwise we divide d by r to get the pseudo cosin.
        if(r == 0) t = 0;
        else t = d / r;
        //a debug statement, ignore.
        if(pH.x >= 97.0 && pH.x < 98 && pH.y == 100 && pH.z <= -16 && pH.z > -17)
        {
            int x = getTotalR(hits, numHits, obj);
            numHits = 0;
            numHits = castRay(pDH, nL, hits, numHits);
        }
        //get the diffuse value for this specific light.
        glm::vec4 cDD = cD * cL;
        //does nothing, I should delete.
        if(curLight->getType() != DIRECTIONAL)
        {
            //float dnom = glm::dot(curLight->getPos() - pH, curLight->getPos() - pH);
            //t = t / dnom * 10000;
        }
        //if the light is a spotlight, we don't cast any light outside of the cone defined by the light.
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            t *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        //set t to somewhere between 0 and 1.
        t = clamp(t, 1.0, 0.0);
        //multiply the temporary diffuse value by t
        cDD.r *= t;
        cDD.g *= t;
        cDD.b *= t;

        //add the temporary diffuse value to the final color of the object.
        cPe += cDD;
    }

    return cPe;

}

glm::vec4 Shaders::phong(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
        nL = -curLight->getRelativeNorm(pH);
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


glm::vec4 Shaders::diffuseShadow(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
        nL = -curLight->getRelativeNorm(pH);
        cL = curLight->getColor();

        numHits = castRay(pDH, nL, hits, numHits);
        if(numHits > 1)
        {
            int x = 0;
        }
        sortByT(hits, numHits);
        if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
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

glm::vec4 Shaders::phongShadow(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
        nL = -curLight->getRelativeNorm(pH);
        cL = curLight->getColor();

        //get angle for diffuse light, raycast can catch occluders as well.
        numHits = castRay(pDH, nL, hits, numHits);
        sortByT(hits, numHits);
        if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
        r = getTotalR(hits, numHits, obj);
        if(r == 0) t = 0;
        else t = d / r;

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
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        //if(cos > 0.98) s = 1;
        //else s = 0;
        cPe += cS * cL * s;

    }

    return cPe;

}

glm::vec4 Shaders::mirror(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    if(numDeep < MAX_REFLECTIONS)
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
            nL = -curLight->getRelativeNorm(pH);
            cL = curLight->getColor();

            //get angle for diffuse light, raycast can catch occluders as well.
            numHits = castRay(pDH, nL, hits, numHits);
            sortByT(hits, numHits);
            if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
            r = getTotalR(hits, numHits, obj);
            if(r == 0) t = 0;
            else t = d / r;

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

            //calculate angle for reflecton
            glm::vec3 ref1 = -nPe + (2 *glm::dot(nH, nPe) * nH);
            numHits = 0;
            //cast ray along the reflection angle.
            numHits = castRay(pH, ref1, hits, numHits);
            sortByT(hits, numHits);
            float s = 0;
            for(int i = 0; i < numHits; i++)
            {
                if(hits[i].obj != &obj)
                {
                    //get the color of the closest object hit by the reflection ray.
                    glm::vec3 hitPoint = pH + ref1 * hits[i].t;
                    cS = (*this.*(hits[i].obj->shader))(hits[i].obj->getSurfaceNormal(hitPoint), -ref1, hitPoint, pE, *hits[i].obj, numDeep + 1);
                    s = 1;
                    break;
                }
            }
            if(s == 0)
            {
                cS = glm::vec4(renderer->background * 1000.0f, 1000.0f);
                s = 1;
            }
            //calculate angle for specular highlight
            /*glm::vec3 ref = -nL + (2 * glm::dot(nH, nL) * nH);
            float cos = glm::dot(nPe, ref);
            s = clamp(cos, obj.getGeo().radius, obj.getGeo().width);
            if(curLight->getType() == SPOTLIGHT)
            {
                float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
                s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
            }*/
            //if(cos > 0.98) s = 1;
            //else s = 0;
            cPe += cS * cL * s;

        }

        return cPe;
    }
    return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

}

glm::vec4 Shaders::nMapMirror(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    if(numDeep < MAX_REFLECTIONS)
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
            nL = -curLight->getRelativeNorm(pH);
            cL = curLight->getColor();

            //get angle for diffuse light, raycast can catch occluders as well.
            numHits = castRay(pDH, nL, hits, numHits);
            sortByT(hits, numHits);
            if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
            r = getTotalR(hits, numHits, obj);
            if(r == 0) t = 0;
            else t = d / r;

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

            //get normal from normal map.
            glm::vec4 mapNorm = obj.getTexCol(pH);
            //combine mapNorm with the natural normal of the object
            glm::vec3 combinedNorm = glm::normalize(glm::vec3(mapNorm.x, mapNorm.y, mapNorm.z));
            combinedNorm -= 0.5f;
            combinedNorm += nH;
            //calculate angle for reflecton
            glm::vec3 ref1 = -nPe + (2 *glm::dot(combinedNorm, nPe) * nH);
            numHits = 0;
            //cast ray along the reflection angle.
            numHits = castRay(pH, ref1, hits, numHits);
            sortByT(hits, numHits);
            float s = 0;
            for(int i = 0; i < numHits; i++)
            {
                if(hits[i].obj != &obj)
                {
                    //get the color of the closest object hit by the reflection ray.
                    glm::vec3 hitPoint = pH + ref1 * hits[i].t;
                    cS = (*this.*(hits[i].obj->shader))(hits[i].obj->getSurfaceNormal(hitPoint), -ref1, hitPoint, pE, *hits[i].obj, numDeep + 1);
                    s = 1;
                    break;
                }
            }
            if(s == 0)
            {
                cS = glm::vec4(renderer->background * 1000.0f, 1000.0f);
                s = 1;
            }
            //calculate angle for specular highlight
            /*glm::vec3 ref = -nL + (2 * glm::dot(nH, nL) * nH);
            float cos = glm::dot(nPe, ref);
            s = clamp(cos, obj.getGeo().radius, obj.getGeo().width);
            if(curLight->getType() == SPOTLIGHT)
            {
                float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
                s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
            }*/
            //if(cos > 0.98) s = 1;
            //else s = 0;
            cPe += cS * cL * s;

        }

        return cPe;
    }
    return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

}

glm::vec4 Shaders::refractor(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    if(numDeep < MAX_REFLECTIONS)
    {
        glm::vec4 cD = obj.getCD();
        glm::vec4 cA = obj.getCA();
        glm::vec4 cS = obj.getCS();
        glm::vec4 cL;
        glm::vec4 cPe = cA;
        float shnell = obj.getShnell();
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
            nL = -curLight->getRelativeNorm(pH);
            cL = curLight->getColor();

            //get angle for diffuse light, raycast can catch occluders as well.
            numHits = castRay(pDH, nL, hits, numHits);
            sortByT(hits, numHits);
            if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
            r = getTotalR(hits, numHits, obj);
            if(r == 0) t = 0;
            else t = d / r;

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

            //calculate angle for refraction
            glm::vec3 ref1;
            float C = glm::dot(nH, nPe);
            float sqrtTerm = (C * C - 1)/(shnell * shnell) + 1;
            if(sqrtTerm >= 0)
            {
                float b = (C / shnell) - sqrt(sqrtTerm);
                ref1 = (-1 / shnell) * nPe + b * nH;
            }
            else ref1 = -nPe + (2 *glm::dot(nH, nPe) * nH);
            numHits = 0;
            //cast ray along the reflection angle.
            numHits = castRay(pH, ref1, hits, numHits);
            sortByT(hits, numHits);
            float s = 0;
            for(int i = 0; i < numHits; i++)
            {
                if(hits[i].obj != &obj && (hits[i].obj->getParent() != obj.getParent() || hits[i].obj->getParent() == NULL))
                {
                    //get the color of the closest object hit by the reflection ray.
                    glm::vec3 hitPoint = pH + ref1 * hits[i].t;
                    cS = (*this.*(hits[i].obj->shader))(hits[i].obj->getSurfaceNormal(hitPoint), -ref1, hitPoint, pE, *hits[i].obj, numDeep + 1);
                    s = 1;
                    break;
                }
            }
            if(s == 0)
            {
                cS = glm::vec4(renderer->background * 1000.0f, 1000.0f);
                s = 1;
            }
            //calculate angle for specular highlight
            /*glm::vec3 ref = -nL + (2 * glm::dot(nH, nL) * nH);
            float cos = glm::dot(nPe, ref);
            s = clamp(cos, obj.getGeo().radius, obj.getGeo().width);
            if(curLight->getType() == SPOTLIGHT)
            {
                float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
                s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
            }*/
            //if(cos > 0.98) s = 1;
            //else s = 0;
            cPe += cS * cL * s;

        }

        return cPe;
    }
    return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

}

glm::vec4 Shaders::refractorMaps(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    if(numDeep < MAX_REFLECTIONS)
    {
        glm::vec4 cD = obj.getCD();
        glm::vec4 cA = obj.getCA();
        glm::vec4 cS = obj.getCS();
        glm::vec4 cL;
        glm::vec4 cPe = cA;
        float shnell = obj.getShnell();
        //get normal map normal.
        glm::vec3 mapNorm = obj.getNMapAt(pH);
        //combine mapNorm with the natural normal of the object
        mapNorm -= 0.5f;
        mapNorm += nH;
        shnell = sqrt((1 - pow(glm::dot(nH, nPe), 2)) / (1 - pow(glm::dot(-mapNorm, nPe), 2)));
        float t = 0;
        float d = obj.getGeo().depth;
        float r;
        float displacement = (obj.getBMapAt(pH) - 0.5) * obj.getDisp();
        glm::vec3 pHnew = pH + displacement;
        glm::vec3 pDH = pHnew - nH * d;
        LightBase* curLight;
        glm::vec3 nL;
        intercept hits[MAX_LINE_INTERCEPTS];
        int numHits = 0;
        for(int i = 0; i < renderer->lightNum; i++)
        {
            numHits = 0;
            curLight = renderer->lights[i];
            nL = -curLight->getRelativeNorm(pH);
            cL = curLight->getColor();

            //get angle for diffuse light, raycast can catch occluders as well.
            numHits = castRay(pDH, nL, hits, numHits);
            sortByT(hits, numHits);
            if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
            r = getTotalR(hits, numHits, obj);
            if(r == 0) t = 0;
            else t = d / r;

            glm::vec4 cDD = cD * cL;
            if(curLight->getType() != DIRECTIONAL)
            {
                //float dnom = glm::dot(curLight->getPos() - pH, curLight->getPos() - pH);
                //t = t / dnom * 10000;
            }
            if(curLight->getType() == SPOTLIGHT)
            {
                float cos = glm::dot(glm::normalize((pHnew - curLight->getPos())), curLight->getGeo().normal);
                t *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
            }
            t = clamp(t, 1.0, 0.0);
            cDD.r *= t;
            cDD.g *= t;
            cDD.b *= t;

            cPe += cDD;

            //calculate angle for refraction
            glm::vec3 ref1;
            float C = glm::dot(nH, nPe);
            float sqrtTerm = (C * C - 1)/(shnell * shnell) + 1;
            if(sqrtTerm >= 0)
            {
                float b = (C / shnell) - sqrt(sqrtTerm);
                ref1 = (-1 / shnell) * nPe + b * nH;
            }
            else ref1 = -nPe + (2 *glm::dot(nH, nPe) * nH);
            numHits = 0;
            //cast ray along the reflection angle.
            numHits = castRay(pHnew, ref1, hits, numHits);
            sortByT(hits, numHits);
            float s = 0;
            if(numHits > 2)
            {
                int x = 1;
            }
            for(int i = 0; i < numHits; i++)
            {
                if(hits[i].obj != &obj && (hits[i].obj->getParent() != obj.getParent() || hits[i].obj->getParent() == NULL))
                {
                    //get the color of the closest object hit by the reflection ray.
                    glm::vec3 hitPoint = pHnew + ref1 * hits[i].t;
                    cS = (*this.*(hits[i].obj->shader))(hits[i].obj->getSurfaceNormal(hitPoint), -ref1, hitPoint, pE, *hits[i].obj, numDeep + 1);
                    s = 1;
                    break;
                }
            }
            if(s == 0)
            {
                cS = glm::vec4(renderer->background * 1000.0f, 1000.0f);
                s = 1;
            }
            //calculate angle for specular highlight
            /*glm::vec3 ref = -nL + (2 * glm::dot(nH, nL) * nH);
            float cos = glm::dot(nPe, ref);
            s = clamp(cos, obj.getGeo().radius, obj.getGeo().width);
            if(curLight->getType() == SPOTLIGHT)
            {
                float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
                s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
            }*/
            //if(cos > 0.98) s = 1;
            //else s = 0;
            cPe += cS * cL * s;

        }

        return cPe;
    }
    return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

}

glm::vec4 Shaders::phongShadowClassic(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
        nL = -curLight->getRelativeNorm(pH);
        cL = curLight->getColor();

        //get angle for diffuse light, raycast can catch occluders as well.
        numHits = castRay(pH, nL, hits, numHits);
        sortByT(hits, numHits);
        if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
        if (numHits > 0) t = 0;
        else
        {
            t = glm::dot(nPe, -nL);
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
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        //if(cos > 0.98) s = 1;
        //else s = 0;
        cPe += cS * cL * s;

    }

    return cPe;

}

glm::vec4 Shaders::aLight(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
        nL = -curLight->getRelativeNorm(pH);
        cL = curLight->getColor();

        if(curLight->getType() != AREALIGHT)
        {
            //get angle for diffuse light, raycast can catch occluders as well.
            numHits = castRay(pDH, nL, hits, numHits);
            sortByT(hits, numHits);
            numHits = cullForPLight(hits, numHits, pH, curLight);
            r = getTotalR(hits, numHits, obj);
            if(r == 0) t = 0;
            else t = d / r;

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
        else
        {
            for(int i = 0; i < (int)curLight->getGeo().depth; i++)
            {
                //get angle for diffuse light, raycast can catch occluders as well.
                nL = -curLight->getRelativeNorm(pH);
                numHits = castRay(pDH, nL, hits, numHits);
                sortByT(hits, numHits);
                numHits = cullForPLight(hits, numHits, pH, curLight);
                r = getTotalR(hits, numHits, obj);
                if(r != 0) t += d / r;


                numHits = 0;
            }
            t /= curLight->getGeo().depth;
            glm::vec4 cDD = cD * cL;
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            if(cos < 0) t = 0;
            t = clamp(t, 1.0, 0.0);
            cDD.r *= t;
            cDD.g *= t;
            cDD.b *= t;

            cPe += cDD;
        }

        //calculate angle for specular highlight
        glm::vec3 ref = -nL + (2 * glm::dot(nH, nL) * nH);
        float cos = glm::dot(nPe, ref);
        float s = clamp(cos, obj.getGeo().radius, obj.getGeo().width);
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        //if(cos > 0.98) s = 1;
        //else s = 0;
        cPe += cS * cL * s;

    }

    return cPe;

}

glm::vec4 Shaders::texMap(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    glm::vec4 cD = obj.getTexCol(pH);
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
        nL = -curLight->getRelativeNorm(pH);
        cL = curLight->getColor();

        //get angle for diffuse light, raycast can catch occluders as well.
        numHits = castRay(pDH, nL, hits, numHits);
        sortByT(hits, numHits);
        if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
        r = getTotalR(hits, numHits, obj);
        if(r == 0) t = 0;
        else t = d / r;

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
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        //if(cos > 0.98) s = 1;
        //else s = 0;
        cPe += cS * cL * s;

    }

    return cPe;

}

glm::vec4 Shaders::displacementMap(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep)
{
    glm::vec4 cD = obj.getCD();
    glm::vec4 cA = obj.getCA();
    glm::vec4 cS = obj.getCS();
    glm::vec4 cL;
    glm::vec4 cPe = cA;
    glm::vec4 hColor = obj.getTexCol(pH);
    float displacement = glm::length(glm::vec3(hColor.x/hColor.a, hColor.y/hColor.a, hColor.z/hColor.a)) * obj.getDisp();
    pH = pH + (nH * displacement);
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
        nL = -curLight->getRelativeNorm(pH);
        cL = curLight->getColor();

        //get angle for diffuse light, raycast can catch occluders as well.
        numHits = castRay(pDH, nL, hits, numHits);
        sortByT(hits, numHits);
        if(curLight->getType() != DIRECTIONAL) numHits = cullForPLight(hits, numHits, pH, curLight);
        r = getTotalR(hits, numHits, obj);
        if(r == 0) t = 0;
        else t = d / r;

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
        if(curLight->getType() == SPOTLIGHT)
        {
            float cos = glm::dot(glm::normalize((pH - curLight->getPos())), curLight->getGeo().normal);
            s *= clamp(cos, curLight->getGeo().radius, curLight->getGeo().width);
        }
        //if(cos > 0.98) s = 1;
        //else s = 0;
        cPe += cS * cL * s;

    }

    return cPe;

}

glm::vec4 Shaders::skySphere(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    return obj.getTexCol(pH);
}
