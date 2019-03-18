#include "stampednoise.h"

StampedNoise::StampedNoise(ScalarFSPN* fspn, int nx, int ny, int nz)
{
    FSPN = fspn;
    Nx = nx;
    Ny = ny;
    Nz = nz;
}

Grid<float>* StampedNoise::generateGrid(mparticle *p)
{
    Grid<float>* hold = new Grid<float>(p->pos - glm::vec3(p->pscale), p->pos + glm::vec3(p->pscale), Nx, Ny, Nz);
    hold->stampNoise(FSPN, p);
    return hold;
}

void StampedNoise::restampGrid(Grid<float> *g, mparticle *p)
{
    g->stampNoise(FSPN, p);
}

void StampedNoise::setOctaves(int o)
{
    FSPN->setOctaveCount(o);
}

void StampedNoise::setFrequency(float f)
{
    FSPN->setFrequency(f);
}

void StampedNoise::setFjump(float f)
{
    FSPN->setLacunarity(f);
}
