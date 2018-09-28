#ifndef GRID_H
#define GRID_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"

template < typename T >
class Grid
{
public:
    Grid(glm::vec3 L, glm::vec3 U, int nx, int ny, int nz);

    setDataAt(int index);

    glm::vec3 getLLC();
    glm::vec3 getURC();
    void getDimensions(int& arr); //expects an int array of length 3 : int[3]

    trilerp(glm::vec3 x);
private:
    glm::vec3 LLC;
    glm::vec3 URC;
    int Nx, Ny, Nz;
    T* data;
};

#endif // GRID_H
