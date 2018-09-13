#ifndef SPHMODEL_H
#define SPHMODEL_H

#include <vector>
#include <../glm-0.9.9.1/glm/glm.hpp>
#include "particlegrid.h"
#include "solidframe.h"

class Particle;
//class SolidFrame;

class SPHModel
{
    std::vector<Particle> particles;
    ParticleGrid grid;
    std::vector<SolidFrame> frames;
    std::vector<Particle> circles;
    float* pointDispBuf;
    float* colDispBuf;
    glm::vec3 initColor;
    int numParticles;
    int width, height;
    int sph_state = 0;
    float gravity, wFriction, radius;
    //pressure coefficients
    float Pconst;
    float Dconst;
    float Rho;
    //viscosity coefficients
    float Vconst;
    float Epsilon;

public:
    SPHModel();
    SPHModel(int w, int h, int parts, float g, float f, float r);
    void init(int w, int h, int parts, float g, float f, float r);

    void update(float timeStep);

    void eulerTS(float timeStep);
    void leapFrogTS(float timeStep);
    void sixesTS(float timeStep);

    void forces(float timeStep);
    void pvF(float timeStep);
    void sFrameF(float timeStep);
    void gravityF(float timeStep);

    void circPartCollide(float timeStep);

    void updateFrameForces(float timeStep);
    void updateFrameChilds(float timeStep);

    void calcDensities();
    float calcPressElement(Particle* a, Particle* b);
    float calcViscElement(Particle* a, Particle* b);

    void enforceBounds();

    void addParts(int parts);
    int addPart(Particle part);
    int addFrame(SolidFrame frame);
    int addCircle(Particle c);
    void passToDisplay(int max);

    void setInitColor(glm::vec3 col);
    void setpointDispBuf(float* points);
    void setcolDispBuf(float* cols);
    void setGravity(float g);
    void setWFriction(float f);
    void setPCoefficients(float P, float D, float R);
    void setPconst(float P);
    void setDconst(float D);
    void setRho(float R);
    void setVCoefficients(float V, float E);
    void setVconst(float V);
    void setEpsilon(float E);
    void setState(int s);

    int getNumParts();
    int getNumVerts();
    int getHeight();
    int getWidth();
    int getState();
    float getGravity();
    float getWFriction();
    float getRadius();
    Particle* getPart(int i);
    SolidFrame* getFrame(int i);
    Particle* getCircle(int i);


};

#endif // SPHMODEL_H
