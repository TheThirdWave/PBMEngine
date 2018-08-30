#include "stuffbuilder.h"
#include "sphmodel.h"
#include "particle.h"
#include "solidframe.h"

StuffBuilder::StuffBuilder()
{

}

void StuffBuilder::MakeSFCube(SPHModel *model, glm::vec2 llc, int numParts, float sidelength, float elasticity)
{
    int idx = model->addFrame(SolidFrame());
    SolidFrame* frame = model->getFrame(idx);
    frame->setElasticity(elasticity);
    int partsPerLevel = ceil(sqrt(numParts));
    for(int i = 0; i < partsPerLevel; i++)
    {
        for(int j = 0; j < partsPerLevel; j++)
        {
            if((i + j * sidelength) > numParts) break;
            int idx1 = frame->addParticle(Particle(llc + glm::vec2( i * (sidelength / partsPerLevel), j * (sidelength / partsPerLevel)), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, model->getRadius()));
            Particle* p = frame->getParticle(idx1);
            int idx2 = model->addPart(Particle(llc + glm::vec2(i * (sidelength / partsPerLevel), j * (sidelength / partsPerLevel)), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, model->getRadius()));
            Particle* p1 = model->getPart(idx2);
            p->setParentPtr(frame);
            p->setSolidPtr(idx2);
            p1->setParentPtr(frame);
            p1->setSolidPtr(idx1);
        }
    }
    frame->calcCOM();
    frame->calcRelPos();
}
