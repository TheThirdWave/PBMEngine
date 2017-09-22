#ifndef IMAGEMANIP_H
#define IMAGEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "structpile.h"

#define MAX_IMAGES 10

class Imagemanip
{
private:
    image screen;
public:
    Imagemanip();
    ~Imagemanip();
    Imagemanip(int, int);
    void initScreen(int, int);
    image getScreen();
    image* getPtr();
    void psychedelic(int);
};

#endif // IMAGEMANIP_H
