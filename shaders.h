#ifndef SHADERS_H
#define SHADERS_H

#include "structpile.h"
#include "algorithm"
#include "vector"
#include "lightbase.h"
#include "../glm-0.9.8.5/glm/glm.hpp"
#include "../glm-0.9.8.5/glm/gtc/matrix_transform.hpp"
#include "../glm-0.9.8.5/glm/gtc/type_ptr.hpp"
#include "../glm-0.9.8.5/glm/gtx/transform.hpp"
#include "../glm-0.9.8.5/glm/gtx/rotate_vector.hpp"

class Imagemanip;

class Shaders
{
    Imagemanip* renderer;
public:
    Shaders();
    void setRenderer(Imagemanip*);
    float clamp(float cos, float max, float min);
    void genvoronoi(float l);
    void genSphere(int wSeg, int hSeg);
    int castRay(glm::vec3 pE, glm::vec3 nPE, intercept* ret, int idx);
    int castRayMBlur(glm::vec3 pE, glm::vec3 nPE, intercept* ret, int idx);
    void sortByT(intercept* ret, int idx);
    float getTotalR(intercept* ret, int idx, Function3D &obj);
    float cullForPLight(intercept* ret, int idx, glm::vec3 pH, LightBase* L);
    glm::vec4 outline(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 flat(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 voronoi(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 diffuse(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 phong(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 diffuseShadow(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 phongShadow(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 phongShadowMBlur(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 phongShadowClassic(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 ambientOcclusion(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 colorBleed(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 caustics(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 mirror(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 mirrorBlur(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 nMapMirror(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 refractor(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 refractorBlur(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 refractorMaps(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 aLight(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 texMap(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 texMapDProj(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 texMapPProj(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 displacementMap(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
    glm::vec4 skySphere(glm::vec3 nH, glm::vec3 nPe, glm::vec3 pH, glm::vec3 pE, Function3D &obj, int numDeep);
};

#endif // SHADERS_H
