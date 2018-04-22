#ifndef SPHMODEL_H
#define SPHMODEL_H

#include <vector>
#include <../glm-0.9.8.5/glm/glm.hpp>
#include "particlegrid.h"

class Particle;

class SPHModel
{
    std::vector<Particle> particles;
    ParticleGrid grid;
    float* pointDispBuf;
    float* colDispBuf;
    glm::vec3 initColor;
    int numParticles;
    int width, height;
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

    void forces(float timeStep);
    void pvF(float timeStep);
    void gravityF(float timeStep);

    void calcDensities();
    float calcPressElement(Particle* a, Particle* b);
    float calcViscElement(Particle* a, Particle* b);

    void enforceBounds();

    void addParts(int parts);
    void addPart(Particle part);
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

    int getNumParts();
    int getHeight();
    int getWidth();
    float getGravity();
    float getWFriction();

};

#endif // SPHMODEL_H
