#ifndef IMAGEMANIP_H
#define IMAGEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "function2d.h"
#include "structpile.h"

class Imagemanip
{
private:
    image screen;
    glm::vec3 background, foreground;
    Function2D* functions;
public:
    Imagemanip();
    ~Imagemanip();
    Imagemanip(int, int);
    void initScreen(int, int);
    void setBackground(char, char, char);
    void setForeground(char, char, char);
    void addFunction(Function2D*);
    image getScreen();
    image* getPtr();
    void psychedelic(int);
    void clearScreen();
    void fillScreen(int, int, int);
    void drawConvex();
};

#endif // IMAGEMANIP_H
