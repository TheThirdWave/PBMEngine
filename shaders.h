#ifndef SHADERS_H
#define SHADERS_H

#include "structpile.h"
#include "algorithm"
#include "vector"
#include "lightbase.h"

class Imagemanip;

class Shaders
{
    Imagemanip* renderer;
public:
    Shaders();
    void setRenderer(Imagemanip*);
    float clamp(float cos, float max, float min);
    int castRay(glm::vec3 pE, glm::vec3 nPE, intercept* ret, int idx);
    void sortByT(intercept* ret, int idx);
    float getTotalR(intercept* ret, int idx, Function3D &obj);
    float cullForPLight(intercept* ret, int idx, glm::vec3 pH, LightBase* L);
    glm::vec4 outline(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 flat(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 diffuse(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 phong(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 diffuseShadow(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 phongShadow(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 aLight(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 texMap(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
    glm::vec4 skySphere(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj);
};

#endif // SHADERS_H
