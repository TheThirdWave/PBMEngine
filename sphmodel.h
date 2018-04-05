#ifndef SPHMODEL_H
#define SPHMODEL_H

class Particle;

class SPHModel
{
    Particle* particles;
    float* pointDispBuf;
    float* colDispBuf;
    int numParticles;
public:
    SPHModel();
};

#endif // SPHMODEL_H
