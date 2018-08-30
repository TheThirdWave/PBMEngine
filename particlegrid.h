#ifndef PARTICLEGRID_H
#define PARTICLEGRID_H

#include <vector>
#include <../glm-0.9.8.5/glm/glm.hpp>

class Particle;

class ParticleGrid
{
private:
    std::vector<std::vector<Particle*>> grid;
    glm::vec2 min, max;
    glm::vec2 LLC, URC;
    int width, height;
    float cellSize;
public:
    ParticleGrid();

    void buildGrid(std::vector<Particle> &parts);

    std::vector<Particle*>* getPartsAt(int x, int y);
    void printGridStatus();

    void setMin(glm::vec2 m);
    void setMax(glm::vec2 m);
    void setCellSize(float cs);
};

#endif // PARTICLEGRID_H
