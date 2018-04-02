#ifndef FLUIDMODEL_H
#define FLUIDMODEL_H

#include "buffer2d.h"
#include <../glm-0.9.8.5/glm/glm.hpp>

class FluidModel
{
private:
    Buffer2D density;
    Buffer2D sDensity;
    Buffer2D tDensity;
    Buffer2D stDensity;
    Buffer2D velocity;
    Buffer2D charMap;
    Buffer2D forwardsMap;
    Buffer2D backwardsMap;
    Buffer2D err;
    Buffer2D pressure;
    Buffer2D obstruction;
    Buffer2D* source;
    Buffer2D* color;
    bool hasSource, hasTSource, macCormack;
    int pLoops, iopLoops, logLoops;
    float gravity;

public:
    FluidModel();
    FluidModel(Buffer2D* initBuf, Buffer2D* s);
    void init(Buffer2D* initBuf, Buffer2D* s);
    void runTimeStep(double timeStep);
    void advection(double timeStep);
    void forces(double timeStep);
    void sources(double timeStep);
    void targetSource();
    void calcPressure();
    void applyPressure();
    void enforceBounds();

    void cMapSLAdvect(double timeStep);
    void cMapMCAdvect(double timeStep);
    void setObsBoundary();
    void fastBlur(Buffer2D* in, Buffer2D* out);

    float calcDivergence(int i, int j);

    float interpolateF(Buffer2D* buf, glm::vec2 vec);
    glm::vec2 interpolate2Vec(Buffer2D* buf, glm::vec2 vec);
    glm::vec3 interpolate3Vec(Buffer2D* buf, glm::vec2 vec);

    float getGravity();
    int getPLoops();
    int getIOPLoops();
    int getLogLoops();
    bool usingMacCormack();

    void setHasSource(bool);
    void setHasTSource(bool);
    void setGravity(float);
    void setPLoops(int);
    void setIOPLoops(int);
    void setLogLoops(int);
    void setUsingMacCormack(bool);
    void reset();
    Buffer2D* getSource();
    Buffer2D* getDensity();
    Buffer2D* getPressure();
    Buffer2D* getObstruction();
    Buffer2D* getError();
};

#endif // FLUIDMODEL_H
