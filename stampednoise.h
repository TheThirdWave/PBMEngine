#ifndef STAMPEDNOISE_H
#define STAMPEDNOISE_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "grid.h"
#include "structpile.h"
#include "scalarfields.h"
#include "field.h"

class StampedNoise
{
public:
    StampedNoise(ScalarFSPN *fspn, int nx, int ny, int nz);

    Grid<float>* generateGrid(mparticle* p);

    void  restampGrid(Grid<float>* g, mparticle* p);

    void setOctaves(int o);
    void setFrequency(float f);
    void setFjump(float f);
    void setFade (float f);

    ScalarFSPN* FSPN;
private:
    int Nx;
    int Ny;
    int Nz;
};

#endif // STAMPEDNOISE_H
