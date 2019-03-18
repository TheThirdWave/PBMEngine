#ifndef WISPPART_H
#define WISPPART_H

#include <random>
#include <../glm-0.9.9.1/glm/glm.hpp>
#include "grid.h"
#include "structpile.h"
#include "scalarfields.h"
#include "field.h"

class WispPart
{
public:
    WispPart(ScalarFSPN* fspn, ScalarFSPN* fspn2, int nx, int ny, int nz);

    Grid<float>* generateGrid(wparticle* p);

    void restampGrid(Grid<float>*g, wparticle* p);

    void setOctaves(int o);
    void setFrequency(float f);
    void setFjump(float f);

    ScalarFSPN* FSPN;
    ScalarFSPN* FSPN2;
    std::random_device rd1;
    std::mt19937 gen1;
    std::uniform_real_distribution<double> prn{-1.0, 1.0};
private:
    int Nx, Ny, Nz;
};

#endif // WISPPART_H
