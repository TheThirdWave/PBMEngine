#include "particleobject.h"

ParticleObject::ParticleObject()
{
    id = PARTICLE;
}

void ParticleObject::initRenderObj(model* mdl)
{
    rendrPtr = new RenderObject();
    rendrPtr->setModel(mdl);
}

void ParticleObject::updateRenderObject()
{
    glm::mat4 rot;
    if(glm::dot(geoDescription.normal, geoDescription.upVec) != -1) rot = glm::orientation(geoDescription.normal, geoDescription.upVec);
    else rot = glm::rotate(glm::mat4(1.0f), (float)PI, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pos = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), scale);

    rendrPtr->setPosMatrix(pos * rot * sca);
}

void ParticleObject::setGeometry(glm::vec3 v)
{
    geoDescription.normal = glm::vec3(-v.x, -v.y, v.z);
}

void ParticleObject::setGeometry(glm::vec3 v, glm::vec3 uv)
{
    geoDescription.normal = v;
    geoDescription.upVec = uv;
}
