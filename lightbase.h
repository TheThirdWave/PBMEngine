#ifndef LIGHTBASE_H
#define LIGHTBASE_H

#include "structpile.h"

class LightBase
{
private:
    glm::vec3 position;
    geometry geo;
public:
    LightBase();
    void setPos(glm::vec3 pos);
    void setGeo(geometry geo);
    glm::vec3 getPos();
    geometry getGeo();
};

#endif // LIGHTBASE_H
