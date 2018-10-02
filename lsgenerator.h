#ifndef LSGENERATOR_H
#define LSGENERATOR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <../glm-0.9.9.1/glm/glm.hpp>

#include "grid.h"

class LSGenerator
{
public:
    LSGenerator();

    void stampLS(Grid<float> &g);
    bool readObj(char* fname);

private:
    float findClosestPoint(glm::vec3 pos);
    void evaluateTri(glm::vec3 x, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 &p, glm::vec3 &n);
    std::vector<glm::vec3> vecs;
    std::vector<std::vector<int>> indicies;
};

#endif // LSGENERATOR_H
