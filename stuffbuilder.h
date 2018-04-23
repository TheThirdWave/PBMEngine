#ifndef STUFFBUILDER_H
#define STUFFBUILDER_H

#include <vector>
#include <../glm-0.9.8.5/glm/glm.hpp>

class SPHModel;
class Particle;
class SolidFrame;

class StuffBuilder
{
public:
    StuffBuilder();
    void MakeSFCube(SPHModel* model, glm::vec2 llc, float radius, float sidelength);
};

#endif // STUFFBUILDER_H
