#ifndef FLUIDMODEL_H
#define FLUIDMODEL_H

#include "buffer2d.h"
#include <../glm-0.9.8.5/glm/glm.hpp>

class FluidModel
{
private:
    Buffer2D density;
    Buffer2D velocity;
    Buffer2D charMap;
    Buffer2D pressure;
    Buffer2D obstruction;
    Buffer2D* source;
    Buffer2D* color;
    bool hasSource;
    int pLoops;
    float gravity;

public:
    FluidModel();
    FluidModel(Buffer2D* initBuf, Buffer2D* s);
    void init(Buffer2D* initBuf, Buffer2D* s);
    void runSLTimeStep(double timeStep);
    void advection(double timeStep);
    void forces(double timeStep);
    void sources(double timeStep);
    void calcPressure();
    void applyPressure();

    void cMapSLAdvect(double timeStep);

    float calcDivergence(int i, int j);

    float interpolateF(Buffer2D* buf, glm::vec2 vec);
    glm::vec2 interpolate2Vec(Buffer2D* buf, glm::vec2 vec);
    glm::vec3 interpolate3Vec(Buffer2D* buf, glm::vec2 vec);

    void setHasSource(bool);
    void setGravity(float);
    void reset();
    Buffer2D* getSource();
    Buffer2D* getObstruction();
};

#endif // FLUIDMODEL_H
