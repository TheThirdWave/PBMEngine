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

void LightBase::setTex(Imagemanip *tex)
{
    texture = tex;
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

Imagemanip* LightBase::getTex()
{
    return texture;
}
