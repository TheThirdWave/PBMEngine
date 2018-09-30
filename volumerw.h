#ifndef VOLUMERW_H
#define VOLUMERW_H

#include <string>
#include <fstream>
#include <iostream>

#include "grid.h"

class VolumeRW
{
public:
    VolumeRW();

    Grid<float>* readScalarGrid(char* fname);
    bool writeScalarGrid(Grid<float>* dat, char* fname);
};

#endif // VOLUMERW_H
