#ifndef FUNCTION3D_H
#define FUNCTION3D_H

#include "structpile.h"
#include "algorithm"
#include "shaders.h"

typedef glm::vec4 (Shaders::*ShaderPtr)(glm::vec3, glm::vec3, glm::vec3, glm::vec3, Function3D&obj, int numDeep);

class Function3D
{
    friend class Imagemanip;
protected:
    glm::vec3 normal, normal2, normal3;
    glm::vec3 origPoint, point2;
    glm::vec3* points[MAX_POINTS];
    glm::vec4 cS, cD, cA;
    int a02, a12, a22, a21, a00, pointIdx, numChildren;
    float s0, s1, s2, disp, shnell;
    geometry geo;
    Imagemanip* texture;
    Imagemanip* bumpMap;
    Imagemanip* normMap;
    Function3D* children;
    Function3D* parent;
    triangle tri, nTri;
    triangle2d uvs;

public:
    Function3D();
    Function3D(glm::vec3, glm::vec3);
    Function3D(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
    void setNormal(glm::vec3);
    void setNormal(glm::vec3, glm::vec3);
    void setNormal(glm::vec3, glm::vec3, glm::vec3);
    void setPoint(glm::vec3);
    void setColor(glm::vec4, glm::vec4, glm::vec4);
    void setQParams(int, int, int, int, int);
    void setQReals(float, float, float);
    void setDisp(float);
    void setShnell(float);
    void setGeometry(geometry);
    void setTexture(Imagemanip*);
    void setBumpMap(Imagemanip*);
    void setNormalMap(Imagemanip*);
    void setTriangle(glm::vec3, glm::vec3, glm::vec3);
    void setTriangle(triangle);
    void setTriNorms(glm::vec3, glm::vec3, glm::vec3);
    void setTriNorms(triangle);
    void setUVTriangle(glm::vec2, glm::vec2, glm::vec2);
    void setUVTriangle(triangle2d);
    void setChildren(Function3D* ch, int num);
    void setParent(Function3D* p);
    geometry getGeo();
    glm::vec4 getCS();
    glm::vec4 getCD();
    glm::vec4 getCA();
    float getShnell();
    float getDisp();
    Function3D* getParent();
    virtual glm::vec4 getTexCol(glm::vec3) = 0;
    virtual glm::vec3 getNMapAt(glm::vec3) = 0;
    virtual float getBMapAt(glm::vec3) = 0;
    virtual float getRelativePoint(glm::vec3) = 0;
    virtual int getRelativeLine(glm::vec3, glm::vec3, intercept*, int) = 0;
    virtual glm::vec3 getSurfaceNormal(glm::vec3) = 0;
    ShaderPtr shader;
};

#endif // FUNCTION3D_H
