#ifndef IMAGEMANIP_H
#define IMAGEMANIP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>
#include <algorithm>

#include "function2d.h"
#include "linefunction.h"
#include "spherefunction.h"
#include "function3d.h"
#include "structpile.h"
#include "kernel.h"
#include "shaders.h"
#include "lightbase.h"

using namespace std;

class Imagemanip
{
    friend class Shaders;

private:
    image screen, filterScreen;
    Kernel kern;
    glm::vec3 background, foreground;
    Function2D* functions[MAX_FUNCTIONS];
    Function3D* functions3D[MAX_FUNCTIONS];
    LightBase* lights[MAX_LIGHTS];
    glm::vec3* voronoiPts;
    int funcNum, func3DNum, lightNum, vslen;
    Shaders shades;
public:
    Imagemanip();
    ~Imagemanip();
    Imagemanip(int, int);
    Imagemanip(image*);
    int getWidth();
    int getHeight();
    int getNumChannels();
    float getVSL();
    glm::vec3* getVoronoiPts();
    void getDataAt(int, int, int*);
    void setScreen(image*);
    void setKernel(int, int);
    void setKernelValues(float);
    void setKernelValuesF(Function2D*);
    void setKernelValuesEF(Function2D*);
    void setVoronoiPts(glm::vec3*);
    void initScreen(image*);
    void initScreen(int, int);
    void switchScreenPtrs();
    void setBackground(char, char, char);
    void setForeground(char, char, char);
    void addFunction(Function2D*);
    void addFunction3D(Function3D*);
    void addLight(LightBase*);
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
    void draw3D(glm::vec3, float, glm::vec3, glm::vec3, float, int);
    void draw3DPaint(glm::vec3, float, glm::vec3, glm::vec3, float, int, image*, glm::vec3);
    void draw3DFocus(glm::vec3, float, float, glm::vec3, glm::vec3, float, float, int);
    void draw3DFocusMBlur(glm::vec3, float, float, glm::vec3, glm::vec3, float, float, int);
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
    void diffLight(Imagemanip*, Imagemanip*, glm::vec3);
    void specLight(Imagemanip*, Imagemanip*, glm::vec3, float);
    void reflection(Imagemanip*, Imagemanip*, Imagemanip*, Imagemanip*, Imagemanip*, glm::vec3, float, float, float, float);
    int** beyesMat(int);
    void bdlpf();
    void flipScreens();
    glm::vec3 rgbtohsv(glm::vec3);
    glm::vec3 hsvtorgb(glm::vec3);
};

#endif // IMAGEMANIP_H
