#ifndef SPHMODEL_H
#define SPHMODEL_H

#include <vector>
#include <../glm-0.9.8.5/glm/glm.hpp>

class Particle;

class SPHModel
{
    std::vector<Particle> particles;
    float* pointDispBuf;
    float* colDispBuf;
    glm::vec3 initColor;
    int numParticles;
    int width, height;
public:
    SPHModel();
    SPHModel(int w, int h, int parts);

    void init(int w, int h, int parts);

    void addParts(int parts);
    void addPart(Particle part);
    void passToDisplay(int max);

    void setInitColor(glm::vec3 col);
    void setpointDispBuf(float* points);
    void setcolDispBuf(float* cols);

    int getNumParts();

};

#endif // SPHMODEL_H
