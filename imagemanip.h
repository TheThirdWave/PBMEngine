#ifndef IMAGEMANIP_H
#define IMAGEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>

#include "function2d.h"
#include "structpile.h"

class Imagemanip
{
private:
    image screen;
    glm::vec3 background, foreground;
    Function2D* functions[MAX_FUNCTIONS];
    int funcNum;
public:
    Imagemanip();
    ~Imagemanip();
    Imagemanip(int, int);
    void initScreen(int, int);
    void setBackground(char, char, char);
    void setForeground(char, char, char);
    void addFunction(Function2D*);
    void emptyFunctions();
    image getScreen();
    image* getPtr();
    void psychedelic(int);
    void clearScreen();
    void fillScreen(int, int, int);
    void drawConvex();
    void drawStar();
    void drawMod(int);
    void drawBlobby();
    void drawConvexAA(int);
    void drawStarAA(int);
    void drawModAA(int, int);
    void drawBlobbyAA(int);
    void drawShaded();
};

#endif // IMAGEMANIP_H
