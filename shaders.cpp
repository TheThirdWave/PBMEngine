#include "shaders.h"
#include "imagemanip.h"

Shaders::Shaders()
{
}

void Shaders::setRenderer(Imagemanip *ptr)
{
    renderer = ptr;
}

void Shaders::genvoronoi(float l)
{
    renderer->setVoronoiPts(new glm::vec3[1000]);
    glm::vec3* voronoiPts = renderer->getVoronoiPts();
    float sidelen = l;
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            for(int k = 0; k < 10; k++)
            {
                int index = i + j * 10 + k * 10 * 10;
                //to get the bottom corner of the cell we divide by the length of the cells and take the floor of that.
                voronoiPts[index] = glm::vec3(i * sidelen, j * sidelen, k * sidelen);
                //then, to get a random spot inside the cell we add some random number betwee 0 and the length of the cell to the corner position,
                //srand() is just like rand but it takes in a seed value, so you can get a random number that's determined by the position of the
                //cell's corner position (I think).
                float jitterX = sidelen * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float jitterY = sidelen * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float jitterZ = sidelen * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                voronoiPts[index].x += jitterX;
                voronoiPts[index].y += jitterY;
                voronoiPts[index].z += jitterZ;
            }
        }
    }
}

void Shaders::genSphere(int wSeg, int hSeg)
{
    int numPts = (hSeg - 2) * wSeg + 2;
    renderer->setOccSpherePts(new glm::vec3[numPts]);
    glm::vec3* pts = renderer->getOccSpherePts();
    renderer->setNumOSPts(numPts);
    for(int i = 0; i < hSeg; i++)
    {
        if(i != 0 && i != hSeg - 1)
        {
            for(int j = 0; j < wSeg; j++)
            {
                int index = j + ((i - 1) * wSeg) + 1;
                pts[index] = glm::rotate(glm::vec3(0.0f, 1.0f, 0.0f), (float)(PI / (float)hSeg) * i, glm::vec3(1.0f, 0.0f, 0.0f));
                pts[index] = glm::rotate(pts[index], (float)((PI * 2) / (wSeg)) * j, glm::vec3(0.0f, 1.0f, 0.0f));
                int x = 1;
            }
        }
        else if(i == 0)
        {
            pts[0] = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else if(i == hSeg - 1)
        {
            pts[numPts - 1] = glm::vec3(0.0f, -1.0f, 0.0f);
        }
    }
    renderer->setOccVals((PI / (2 * wSeg)), (PI / hSeg));
}

int Shaders::castRay(glm::vec3 pE, glm::vec3 nPE, intercept ret[], int idx)
{
    for(int i = 0; i < renderer->func3DNum; i++)
    {
        idx = renderer->functions3D[i]->getRelativeLine(pE, nPE, ret, idx);
    }
    return idx;
}

int Shaders::castRayMBlur(glm::vec3 pE, glm::vec3 nPE, intercept ret[], int idx)
{
    for(int i = 0; i < renderer->func3DNum; i++)
    {
        idx = renderer->functions3D[i]->getRelativeLineMBlur(pE, nPE, ret, idx);
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

glm::vec4 Shaders::voronoi(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    //first get the position of the cell nH is in. (we use the modulus to wrap around the grid, I don't wanna deal with out of bounds stuff)
    int fi, fj, fk;
    float sidelen = renderer->getVSL();
    glm::vec3* vpts = renderer->getVoronoiPts();
    fi = floor(((int)pH.x % (int)(10 * sidelen)) / sidelen);
    if(fi < 0) fi = -fi;
    fj = floor(((int)pH.y % (int)(10 * sidelen)) / sidelen);
    if(fj < 0) fj = -fj;
    fk = floor(((int)pH.z % (int)(10 * sidelen) ) / sidelen);
    if(fk < 0) fk = -fk;
    int cidx = (fi + 1) + (fj + 1) * 10 + (fk + 1) * 10 * 10;
    float closest = glm::length(vpts[cidx] - pH);
    float dist;
    for(int i = -1; i <= 1; i++)
    {
        for(int j = -1; j <= 1; j++)
        {
            for(int k = -1; k <= 1; k++)
            {
                //now we check the surrounding cells to find the closest voronoi point.
                int ni = fi + i;
                if(ni < 0) ni = sidelen - 1;
                else if(ni >= sidelen) ni = 0;
                int nj = fj + j;
                if(nj < 0) nj = sidelen - 1;
                else if(nj >= sidelen) nj = 0;
                int nk = fk + k;
                if(nk < 0) nk = sidelen - 1;
                else if(nk >= sidelen) nk = 0;
                int index = ni + nj * 10 + nk * 10 * 10;
                //if the checked point is closer than the current closest point,
                //we switch with that.
                dist = glm::length(vpts[index] - pH);
                if(dist < closest)
                {
                    cidx = index;
                    closest = dist;
                }
            }
        }
    }
    //now we use the position of the nearest point to get the color at pH using srand.
    glm::vec4 cD = obj.getCD();
    unsigned int ux = (unsigned int)vpts[cidx].x;
    unsigned int uy = (unsigned int)vpts[cidx].y;
    unsigned int uz = (unsigned int)vpts[cidx].z;
    cD.r = cD.a * static_cast <float> (rand_r(&ux)) / static_cast <float> (RAND_MAX);
    cD.g = cD.a * static_cast <float> (rand_r(&uy)) / static_cast <float> (RAND_MAX);
    cD.b = cD.a * static_cast <float> (rand_r(&uz)) / static_cast <float> (RAND_MAX);
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
        cL = curLight->getColor(pH);

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
        cL = curLight->getColor(pH);

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
        cL = curLight->getColor(pH);

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
        cL = curLight->getColor(pH);

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
        cL = curLight->getColor(pH);

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

glm::vec4 Shaders::phongShadowMBlur(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
        cL = curLight->getColor(pH);

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
            cL = curLight->getColor(pH);

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

glm::vec4 Shaders::ambientOcclusion(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
        cL = curLight->getColor(pH);

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

        int numOR = renderer->getNumOSPts();
        float hold = numOR;
        for(int i = 0; i < numOR; i++)
        {
            glm::vec3* oPts = renderer->getOccSpherePts();
            float oW = renderer->getOccU();
            float oH = renderer->getOccV();
            float rotH = (oH * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - (oH / 2);
            float rotW = (oW *static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - (oW / 2);
            glm::vec3 h = glm::rotate(oPts[i], rotH, glm::vec3(1.0f, 0.0f, 0.0f));
            h = glm::rotate(h, rotW, glm::vec3(0.0f, 1.0f, 0.0f));
            numHits = 0;
            numHits = castRay(pH + nH * 0.001f, h, hits, numHits);//(pH, h, hits, numHits);
            sortByT(hits, numHits);
            float z = hits[0].t / renderer->getAmbRad();
            if(numHits > 0 && hits[0].t <= renderer->getAmbRad()) hold -= 1 - (pow(renderer->getOccFall(), z) - 1.0f) / (renderer->getOccFall() - 1.0f);//z / (z + renderer->getOccFall());
        }
        glm::vec4 cDA = cD;
        cDA.r *= (hold / (float)numOR);
        cDA.b *= (hold / (float)numOR);
        cDA.g *= (hold / (float)numOR);
        cDA *= obj.getOCoeff();
        cPe += cDA;// * (hold / 10.0f);

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

glm::vec4 Shaders::colorBleed(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
            cL = curLight->getColor(pH);

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

            int numOR = renderer->getNumOSPts();
            float hold = numOR;
            glm::vec4 cDA = glm::vec4(0.0f);
            for(int i = 0; i < numOR; i++)
            {
                glm::vec3* oPts = renderer->getOccSpherePts();
                float oW = renderer->getOccU();
                float oH = renderer->getOccV();
                float rotH = (oH * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - (oH / 2);
                float rotW = (oW *static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - (oW / 2);
                glm::vec3 h = glm::rotate(oPts[i], rotH, glm::vec3(1.0f, 0.0f, 0.0f));
                h = glm::rotate(h, rotW, glm::vec3(0.0f, 1.0f, 0.0f));
                numHits = 0;
                numHits = castRay(pH + nH * 0.001f, h, hits, numHits);//(pH, h, hits, numHits);
                sortByT(hits, numHits);
                float z = hits[0].t / renderer->getAmbRad();
                if(numHits > 0 && hits[0].t <= renderer->getAmbRad())
                {
                    hold -= 1 - (pow(renderer->getOccFall(), z) - 1.0f) / (renderer->getOccFall() - 1.0f);//z / (z + renderer->getOccFall());
                    glm::vec3 hitPoint = (pH + nH * 0.001f) + h * hits[0].t;
                    cDA += hits[0].obj->getCA() * obj.getCBCoeff();
                }
            }
            cDA.r *= (hold / (float)numOR);
            cDA.b *= (hold / (float)numOR);
            cDA.g *= (hold / (float)numOR);
            cDA *= obj.getOCoeff();
            cPe += cDA;// * (hold / 10.0f);

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
    return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

}

glm::vec4 Shaders::caustics(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
            cL = curLight->getColor(pH);

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

            int numOR = renderer->getNumOSPts();
            float hold = numOR;
            glm::vec4 cDA = glm::vec4(0.0f);
            for(int i = 0; i < numOR; i++)
            {
                glm::vec3* oPts = renderer->getOccSpherePts();
                float oW = renderer->getOccU();
                float oH = renderer->getOccV();
                float rotH = (oH * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - (oH / 2);
                float rotW = (oW *static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - (oW / 2);
                glm::vec3 h = glm::rotate(oPts[i], rotH, glm::vec3(1.0f, 0.0f, 0.0f));
                h = glm::rotate(h, rotW, glm::vec3(0.0f, 1.0f, 0.0f));
                numHits = 0;
                numHits = castRay(pH + nH * 0.001f, h, hits, numHits);//(pH, h, hits, numHits);
                sortByT(hits, numHits);
                float z = hits[0].t / renderer->getAmbRad();
                if(numHits > 0 && hits[0].t <= renderer->getAmbRad())
                {
                    hold -= 1 - (pow(renderer->getOccFall(), z) - 1.0f) / (renderer->getOccFall() - 1.0f);//z / (z + renderer->getOccFall());
                    glm::vec3 hitPoint = (pH + nH * 0.001f) + h * hits[0].t;
                    if(hits[0].obj->shader != &Shaders::mirror && hits[0].obj->shader != &Shaders::refractor) cDA += hits[0].obj->getCA() * obj.getCBCoeff();
                    else if(hits[0].obj->shader == &Shaders::mirror)
                    {
                        glm::vec3 rNorm = hits[0].obj->getSurfaceNormal(hitPoint);
                        glm::vec3 rPe = glm::normalize(pH - hitPoint);
                        //calculate angle for reflecton
                        glm::vec3 ref1 = -rPe + (2 *glm::dot(rNorm, rPe) * rNorm);
                        numHits = 0;
                        //cast ray along the reflection angle.
                        numHits = castRay(hitPoint, ref1, hits, numHits);
                        sortByT(hits, numHits);
                        cDD += cD * hits[0].obj->getCD();
                    }
                    else if(hits[0].obj->shader == &Shaders::refractor)
                    {
                        glm::vec3 rNorm = hits[0].obj->getSurfaceNormal(hitPoint);
                        glm::vec3 rPe = glm::normalize(pH - hitPoint);
                        float shnell = hits[0].obj->getShnell();
                        //calculate angle for refraction
                        glm::vec3 ref1;
                        float C = glm::dot(rNorm, rPe);
                        float sqrtTerm = (C * C - 1)/(shnell * shnell) + 1;
                        if(sqrtTerm >= 0)
                        {
                            float b = (C / shnell) - sqrt(sqrtTerm);
                            ref1 = (-1 / shnell) * rPe + b * rNorm;
                        }
                        else ref1 = -rPe + (2 *glm::dot(rNorm, rPe) * rNorm);
                        numHits = 0;
                        //cast ray along the reflection angle.
                        numHits = castRay(hitPoint, ref1, hits, numHits);
                        sortByT(hits, numHits);
                        cDD += cD * hits[0].obj->getCD();
                    }
                }
            }
            cDA.r *= (hold / (float)numOR);
            cDA.b *= (hold / (float)numOR);
            cDA.g *= (hold / (float)numOR);
            cDA *= obj.getOCoeff();
            cPe += cDA;// * (hold / 10.0f);

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
    return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

}

glm::vec4 Shaders::mirrorBlur(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
            cL = curLight->getColor(pH);

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
            glm::vec3 n0 = glm::normalize(glm::cross(glm::vec3(0.0f, -1.0f, 0.0f), ref1));
            glm::vec3 n1 = glm::normalize(glm::cross(ref1, n0));
            ref1 = glm::normalize(ref1 + (n0 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + n1 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * obj.getBlur());

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
            cL = curLight->getColor(pH);

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
            cL = curLight->getColor(pH);

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

glm::vec4 Shaders::refractorBlur(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
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
            cL = curLight->getColor(pH);

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
            glm::vec3 n0 = glm::normalize(glm::cross(glm::vec3(0.0f, -1.0f, 0.0f), ref1));
            glm::vec3 n1 = glm::normalize(glm::cross(ref1, n0));
            ref1 = glm::normalize(ref1 + (n0 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + n1 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * obj.getBlur());

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
            cL = curLight->getColor(pH);

            //get normal map normal.
            glm::vec3 mapNorm = obj.getNMapAt(pH);
            //combine mapNorm with the natural normal of the object
            mapNorm -= 0.5f;
            mapNorm += nH;

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
            float C = glm::dot(mapNorm, nPe);
            float sqrtTerm = (C * C - 1)/(shnell * shnell) + 1;
            if(sqrtTerm >= 0)
            {
                float b = (C / shnell) - sqrt(sqrtTerm);
                ref1 = (-1 / shnell) * nPe + b * mapNorm;
            }
            else ref1 = -nPe + (2 *glm::dot(mapNorm, nPe) * mapNorm);
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
        cL = curLight->getColor(pH);

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
        cL = curLight->getColor(pH);

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
        cL = curLight->getColor(pH);

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

glm::vec4 Shaders::texMapDProj(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    glm::vec3 p0 = obj.getTexPos();
    glm::vec3 n2 = obj.getTexNorm();
    glm::vec3 up;
    Imagemanip* tex = obj.getTexture();
    if(n2 != glm::vec3(0.0f, 1.0f, 0.0f) && n2 != glm::vec3(0.0f, -1.0f, 0.0f)) up = glm::vec3(0.0f, -1.0f, 0.0f);
    else up = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 n0, n1;
    n0 = glm::normalize(glm::cross(n2, up));
    n1 = glm::normalize(glm::cross(n2, n0));
    int texWidth = tex->getWidth();
    int texHeight = tex->getHeight();
    int ptx = glm::dot((pH - p0), n0) + (texWidth / 2.0f);
    int pty = glm::dot((pH - p0), n1) + (texHeight / 2.0f);
    int buf[4];
    ptx = ptx * 2 % texWidth;
    pty = pty * 2 % texHeight;
    if(ptx < 0 || ptx > texWidth) return glm::vec4(0.0f);
    if(pty < 0 || pty > texHeight) return glm::vec4(0.0f);
    tex->getDataAt(ptx, pty, buf);
    glm::vec4 h = obj.getCD();
    glm::vec4 cD =  glm::vec4((buf[0] / 255.0f) * h.a, (buf[1] / 255.0f) * h.a, (buf[2] / 255.0f) * h.a, h.a);
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
        cL = curLight->getColor(pH);

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

glm::vec4 Shaders::texMapPProj(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D& obj, int numDeep)
{
    glm::vec3 p0 = obj.getTexPos();
    glm::vec3 n2 = obj.getTexNorm();
    glm::vec3 up;
    Imagemanip* tex = obj.getTexture();
    if(n2 != glm::vec3(0.0f, 1.0f, 0.0f) && n2 != glm::vec3(0.0f, -1.0f, 0.0f)) up = glm::vec3(0.0f, -1.0f, 0.0f);
    else up = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 n0, n1;
    n0 = glm::normalize(glm::cross(n2, up));
    n1 = glm::normalize(glm::cross(n2, n0));
    float dis = glm::dot((pH - p0), n2);
    float texWidth = tex->getWidth() / obj.getTexDist() * dis;
    float texHeight = tex->getHeight() / obj.getTexDist() * dis;
    float ptx = glm::dot((pH - p0), n0) + (texWidth / 2.0f);
    float pty = glm::dot((pH - p0), n1) + (texHeight / 2.0f);
    ptx /= texWidth;
    pty /= texHeight;
    int buf[4];
    ptx *= tex->getWidth();
    pty *= tex->getHeight();
    ptx = (int)ptx * 2 % tex->getWidth();
    pty = (int)pty * 2 % tex->getHeight();
    tex->getDataAt(ptx, pty, buf);
    glm::vec4 h = obj.getCD();
    glm::vec4 cD =  glm::vec4((buf[0] / 255.0f) * h.a, (buf[1] / 255.0f) * h.a, (buf[2] / 255.0f) * h.a, h.a);
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
        cL = curLight->getColor(pH);

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
        cL = curLight->getColor(pH);

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