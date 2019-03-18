#ifndef PPMRW_H
#define PPMRW_H

#include <iostream>
#include <fstream>
#include <string>

struct ppmStuff
{
    int width;
    int height;
    int sampleWidth;
    char* data;
};


class ppmrw
{
public:
    ppmrw();
    ppmStuff readPPM(const char* filename);
    bool writePPM(const char* filename, ppmStuff&);
};
#endif // PPMRW_H
