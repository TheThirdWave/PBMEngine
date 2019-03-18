#include "volumelight.h"

VolumeLight::VolumeLight()
{

}

VolumeLight::VolumeLight(light l)
{
    li = l;
}

VolumeLight::VolumeLight(light l, ScalarGrid *g)
{
    li = l;
    grid = g;
}

VolumeLight::VolumeLight(glm::vec3 p, color c)
{
    li.pos = p;
    li.col = c;
}

VolumeLight::VolumeLight(glm::vec3 p, color c, ScalarGrid *g)
{
    li.pos = p;
    li.col = c;
    grid = g;
}

void VolumeLight::setPos(glm::vec3 pos)
{
    li.pos = pos;
}

void VolumeLight::setColor(color c)
{
    li.col = c;
}

void VolumeLight::setDSM(ScalarGrid *g)
{
    grid = g;
}

glm::vec3 VolumeLight::getPos()
{
    return li.pos;
}

color VolumeLight::getColor()
{
    return li.col;
}

float VolumeLight::evalDSM(const glm::vec3 &P)
{
    return grid->eval(P);
}
