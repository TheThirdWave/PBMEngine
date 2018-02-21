#include "lightbase.h"

LightBase::LightBase()
{
}

void LightBase::setGeo(geometry g)
{
    geo = g;
}

void LightBase::setPos(glm::vec3 pos)
{
    position = pos;
}

void LightBase::setType(lightType t)
{
    type = t;
}

void LightBase::setColor(glm::vec4 col)
{
    cL = col;
}

geometry LightBase::getGeo()
{
    return geo;
}

glm::vec3 LightBase::getPos()
{
    return position;
}

lightType LightBase::getType()
{
    return type;
}

glm::vec4 LightBase::getColor()
{
    return cL;
}
