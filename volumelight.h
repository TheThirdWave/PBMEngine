#ifndef VOLUMELIGHT_H
#define VOLUMELIGHT_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "scalarfields.h"

class VolumeLight
{
public:
    VolumeLight();
    VolumeLight(light l);
    VolumeLight(light l, ScalarGrid* g);
    VolumeLight(glm::vec3 p, color c);
    VolumeLight(glm::vec3 p, color c, ScalarGrid* g);

    void setDSM(ScalarGrid* g);
    void setPos(glm::vec3 p);
    void setColor(color c);

    glm::vec3 getPos();
    color getColor();
    float evalDSM(const glm::vec3 &P);

private:
    ScalarGrid* grid;
    light li;
};

#endif // VOLUMELIGHT_H
