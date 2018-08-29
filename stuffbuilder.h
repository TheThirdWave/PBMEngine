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
    void MakeSFCube(SPHModel* model, glm::vec2 llc, int numParts, float sidelength, float elasticity);
};

#endif // STUFFBUILDER_H
