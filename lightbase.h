#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include "structpile.h"

class Imagemanip;

class LightBase
{
protected:
    lightType type;
    glm::vec3 position;
    glm::vec4 cL;
    geometry geo;
    Imagemanip* texture;
public:
    LightBase();
    void setType(lightType);
    void setPos(glm::vec3 pos);
    void setGeo(geometry g);
    void setColor(glm::vec4 col);
    void setTex(Imagemanip* tex);
    virtual glm::vec3 getRelativeNorm(glm::vec3 pH) = 0;
    virtual glm::vec4 getColor(glm::vec3 pH) = 0;
    glm::vec3 getPos();
    geometry getGeo();
    lightType getType();
    Imagemanip* getTex();
};

#endif // LIGHTBASE_H
