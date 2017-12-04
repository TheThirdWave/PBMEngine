#ifndef IMAGEMANIP_H
#define IMAGEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>

#include "function2d.h"
#include "linefunction.h"
#include "spherefunction.h"
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
    int getWidth();
    int getHeight();
    void setScreen(image*);
    void setKernel(int, int);
    void setKernelValues(float);
    void setKernelValuesF(Function2D*);
    void setKernelValuesEF(Function2D*);
    void initScreen(image*);
    void initScreen(int, int);
    void switchScreenPtrs();
    void setBackground(char, char, char);
    void setForeground(char, char, char);
    void addFunction(Function2D*);
    void emptyFunctions();
    image getScreen();
    image* getPtr();
    void psychedelic(int);
    void clearScreen();
    void fillScreen(int, int, int);
    void fillScreen(int, int, int, int);
    void noise();
    void drawConvex();
    void drawUnion();
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
    void maskedMBlur(Imagemanip*);
    void emboss();
    void dilation();
    void erosion();
    void maskedDilation(Imagemanip*);
    void maskedErosion(Imagemanip*);
    void alphaLayer(Imagemanip*);
    void multiplyLayer(Imagemanip*);
    void subtractionLayer(Imagemanip*);
    void maxLayer(Imagemanip*);
    void minLayer(Imagemanip*);
    void composite(Imagemanip*, Imagemanip*);
    void oDither(int);
    void cDither(int);
    void fsDither();
    void makeNormal(Imagemanip*);
    void light(Imagemanip*, Imagemanip*, glm::vec3);
    int** beyesMat(int);
    void bdlpf();
    glm::vec3 rgbtohsv(glm::vec3);
    glm::vec3 hsvtorgb(glm::vec3);
};

#endif // IMAGEMANIP_H
