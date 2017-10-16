#ifndef IMAGEMANIP_H
#define IMAGEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>

#include "function2d.h"
#include "structpile.h"
#include "kernel.h"

class Imagemanip
{
private:
    image screen, filterScreen;
    Kernel kern;
    glm::vec3 background, foreground;
    Function2D* functions[MAX_FUNCTIONS];
    int funcNum;
public:
    Imagemanip();
    ~Imagemanip();
    Imagemanip(int, int);
    Imagemanip(image*);
    void setScreen(image*);
    void setKernel(int, int);
    void setKernelValues(float);
    void setKernelValuesF(Function2D*);
    void initScreen(image*);
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
    void simpleBlur();
    void motionBlur(Function2D*);
    void emboss();
    void dilation();
    void erosion();
    glm::vec3 rgbtohsv(glm::vec3);
    glm::vec3 hsvtorgb(glm::vec3);
};

#endif // IMAGEMANIP_H
