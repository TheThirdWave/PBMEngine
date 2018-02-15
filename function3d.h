#ifndef FUNCTION3D_H
#define FUNCTION3D_H

#include "structpile.h"
#include "algorithm"
#include "shaders.h"

typedef glm::vec4 (Shaders::*ShaderPtr)(glm::vec3, glm::vec3, glm::vec4, glm::vec4, float);

class Function3D
{
    friend class Imagemanip;
protected:
    glm::vec3 normal, normal2, normal3;
    glm::vec3 origPoint, point2;
    glm::vec3* points[MAX_POINTS];
    glm::vec4 color;
    int a02, a12, a22, a21, a00, pointIdx;
    float s0, s1, s2;
    geometry geo;
public:
    Function3D();
    Function3D(glm::vec3, glm::vec3);
    Function3D(glm::vec3, glm::vec3, glm::vec3, glm::vec3);
    void setNormal(glm::vec3);
    void setNormal(glm::vec3, glm::vec3);
    void setNormal(glm::vec3, glm::vec3, glm::vec3);
    void setPoint(glm::vec3);
    void setColor(glm::vec4);
    void setQParams(int, int, int, int, int);
    void setQReals(float, float, float);
    void setGeometry(geometry);
    virtual float getRelativePoint(glm::vec3) = 0;
    virtual int getRelativeLine(glm::vec3, glm::vec3, intercept*, int) = 0;
    virtual glm::vec3 getSurfaceNormal(glm::vec3) = 0;
    ShaderPtr shader;
};

#endif // FUNCTION3D_H
