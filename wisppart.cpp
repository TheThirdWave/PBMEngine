#include "wisppart.h"

WispPart::WispPart(ScalarFSPN *fspn, ScalarFSPN *fspn2, int nx, int ny, int nz)
{
    gen1.seed(rd1());
    FSPN = fspn;
    FSPN2 = fspn2;
    Nx = nx;
    Ny = ny;
    Nz = nz;
}

Grid<float>* WispPart::generateGrid(wparticle *p)
{
    Grid<float>* hold = new Grid<float>(p->pos - glm::vec3(p->pscale + p->dscale), p->pos + glm::vec3(p->pscale + p->dscale), Nx, Ny, Nz);

    for(int i = 0 ; i < p->numDots; i++)
    {
        //get a random position in a u,v,w box.
        glm::vec3 rPos(prn(gen1), prn(gen1), prn(gen1));
        //move position to the unit sphere.
        glm::vec3 r1 = glm::normalize(rPos);
        //move r1 off the unit sphere radially.
        float q = std::pow(std::abs(FSPN->eval(rPos)), p->clump);
        glm::vec3 r2 = r1 * q;
        //put r2 in object space/environment space.
        glm::vec3 p2 = p->pos + p->pscale * r2;//(r2.x * p->T + r2.y * p->N + r2.z * p->BN);
        //Add displacement.
        glm::vec3 d(FSPN2->eval(p2), FSPN2->eval(p2 + p->O), FSPN2->eval(p2 - p->O));
        glm::vec3 p3 = p2 + p->dscale * d;
        //stamp wisp dot.
        wispdot wd;
        wd.pos = p3;
        wd.density = p->density;
        hold->revTrilerp(wd);
    }
    return hold;
}

void WispPart::restampGrid(Grid<float> * g, wparticle *p)
{
    g->clearData();

    for(int i = 0 ; i < p->numDots; i++)
    {
        //get a random position in a u,v,w box.
        glm::vec3 rPos(prn(gen1), prn(gen1), prn(gen1));
        //move position to the unit sphere.
        glm::vec3 r1 = glm::normalize(rPos);
        //move r1 off the unit sphere radially.
        float q = std::pow(std::abs(FSPN->eval(rPos)), p->clump);
        glm::vec3 r2 = r1 * q;
        //put r2 in object space/environment space.
        glm::vec3 p2 = p->pos + p->pscale * (r2.x * p->T + r2.y * p->N + r2.z * p->BN);
        //Add displacement.
        glm::vec3 d(FSPN2->eval(p2), FSPN2->eval(p2 + p->O), FSPN2->eval(p2 - p->O));
        glm::vec3 p3 = p2 + p->dscale * d;
        //stamp wisp dot.
        wispdot wd;
        wd.pos = p3;
        wd.density = p->density;
        g->revTrilerp(wd);
    }
}
