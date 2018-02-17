#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include "structpile.h"

class LightBase
{
private:
    lightType type;
    glm::vec3 position;
    glm::vec4 cL;
    geometry geo;
public:
    LightBase();
    void setType(lightType);
    void setPos(glm::vec3 pos);
    void setGeo(geometry g);
    void setColor(glm::vec4 col);
    void setDirectional(glm::vec3 n);
    void setPoint(glm::vec3 pos);
    void setSpotLight(glm::vec3 pos, glm::vec3 n, float rMax, float rMin);
    glm::vec3 getPos();
    geometry getGeo();
    lightType getType();
    glm::vec4 getColor();
};

#endif // LIGHTBASE_H
