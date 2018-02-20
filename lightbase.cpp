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

void LightBase::setDirectional(glm::vec3 n)
{
    geo.normal = n;
    type = DIRECTIONAL;
}

void LightBase::setPoint(glm::vec3 pos)
{
    position = pos;
    type = POINT;
}

void LightBase::setSpotLight(glm::vec3 pos, glm::vec3 n, float  rMin, float rMax)
{
    position = pos;
    geo.normal = n;
    geo.radius = rMin;
    geo.width = rMax;
    type = SPOTLIGHT;
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
