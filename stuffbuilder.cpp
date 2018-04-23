#include "stuffbuilder.h"
#include "sphmodel.h"
#include "particle.h"
#include "solidframe.h"

StuffBuilder::StuffBuilder()
{

}

void StuffBuilder::MakeSFCube(SPHModel *model, glm::vec2 llc, float radius, float sidelength)
{
    int idx = model->addFrame(SolidFrame());
    SolidFrame* frame = model->getFrame(idx);
    for(int i = 0; i < sidelength; i++)
    {
        for(int j = 0; j < sidelength; j++)
        {
            int idx1 = frame->addParticle(Particle(llc + glm::vec2(i * radius, j * radius), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, radius));
            Particle* p = frame->getParticle(idx);
            int idx2 = model->addPart(Particle(llc + glm::vec2(i * radius, j * radius), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, radius));
            Particle* p1 = model->getPart(idx);
            p->setParentPtr(frame);
            p->setSolidPtr(idx2);
            p1->setParentPtr(frame);
            p1->setSolidPtr(idx1);
        }
    }
}
