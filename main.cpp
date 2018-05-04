#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sys/time.h>

#include "../glm-0.9.8.5/glm/glm.hpp"
#include "../glm-0.9.8.5/glm/gtc/matrix_transform.hpp"
#include "../glm-0.9.8.5/glm/gtc/type_ptr.hpp"
#include "../glm-0.9.8.5/glm/gtx/transform.hpp"
#include "../glm-0.9.8.5/glm/gtx/rotate_vector.hpp"

#include "CJRW.h"
#include "shadermanager.h"
#include "modelmanager.h"
#include "imagemanip.h"
#include "shaders.h"
#include "function2d.h"
#include "function3d.h"
#include "lsegfunction.h"
#include "linefunction.h"
#include "planefunction.h"
#include "spherefunction.h"
#include "spherefunction3d.h"
#include "trigfunction.h"
#include "quadraticfunction.h"
#include "quadraticfunction3d.h"
#include "trianglefunction.h"
#include "trianglemesh.h"
#include "infinitysphere.h"
#include "directionallight.h"
#include "pointlight.h"
#include "spotlight.h"
#include "arealight.h"
#include "directionalprojection.h"
#include "structpile.h"
#include "renderobject.h"
#include "physicsobject.h"
#include "planeobject.h"
#include "polygonobject.h"



using namespace std;

image* flatImageRWStuff(int, char**);
void loadShades(void);
void initSphere(void);
void initParticle(float*, float*, unsigned int*);
void initPlane(float*, float*, unsigned int*);
void initShade(void);
void initMatricies(int, int);
void initTexture();
void initTexture(image*);
void configureAttributes(void);
void testRender(void);
void testTexture(void);
void update(float);
void handleKeyStates(float);
void display();
void reshape(GLsizei width, GLsizei height);
void Timer(int value);
void KeyHandler(unsigned char key, int x, int y);
void MouseMoveHandler(int x, int y);
void MouseHandler(int button, int state, int x, int y);
void KeyUpHandler(unsigned char key, int x, int y);
unsigned long getTickCount();

ShaderManager shaderManager;
ReaderWriter imageManager;
ModelManager modelManager;
Imagemanip Screen, vecMask, layer1, outerMask, innerMask, alphaMask, bumpMap, bumpMap1, normMap, normMap1;

PolygonObject plane2;
RenderObject sModel, pModel, poModel;

unsigned int kState = 0;

unsigned long prev_time, cur_time;
glm::mat4 modelViewProj, Proj, View, Model;
GLuint buf, idx, tex, posAttrib, vao, vao1, vao2;
GLfloat angle = 0.0f;
int refreshMills = 30;
int vShade, fShade, cShade, sModelIndex, tCount, capModel;
float lHeight, rCoeff, reflHeight, refrHeight, fresnel;
model* hold1;

Function2D* functions[MAX_FUNCTIONS];

LSegFunction* outer, inner;

int numFuncs = 0;
int numSFuncs = 0;
bool mouseDown = false;
glm::vec2 mDownPos;
glm::vec2 mUpPos;
int progState = CONVEX;

const float timeStep = 1000 / (60.0f);

int rotation = 15;


const char* vertShade;

const char* fragShade;


int main(int argc, char *argv[])
{
    int width = 1920;
    int height = 1080;

    image* img = flatImageRWStuff(argc, argv);

    glutInit(&argc, argv);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(img->width, img->height);

    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    glutCreateWindow("TestWindow");

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
        fprintf(stderr,"Error: %s\n", glewGetErrorString(err));
    }


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    initTexture(img);
    initMatricies(width, height);

    initShade();
    capModel = modelManager.readObjLoader("../Cube1.obj");

    float vertices[] = {
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };
    float colors[] = {
        0.0f, 0.0f, 0.5f,
        1.0f, 0.0f, 0.5f,
        0.0f, 1.0f, 0.5f,
        1.0f, 1.0f, 0.5f
    };
    unsigned int indicies[] = {
        0, 1, 2,
        3, 2, 1
    };

    //create plane vao
    initPlane(vertices, colors, indicies);
    initMatricies(width, height);


    plane2.setGeometry(glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0f, -1.0f));
    plane2.setRenderObject(&pModel);
    plane2.setPosition(glm::vec3(0.0f, 0.0f, -0.0f));
    plane2.setScale(glm::vec3(1.0f, 1.0f, 1.0f));

    glutDisplayFunc(testTexture);
    glutKeyboardFunc(KeyHandler);
    glutKeyboardUpFunc(KeyUpHandler);
    glutPassiveMotionFunc(MouseMoveHandler);
    glutMouseFunc(MouseHandler);

    glutTimerFunc(0, Timer, 0);

    cur_time = getTickCount();
    prev_time = cur_time;
    glutMainLoop();

    return 0;
}

image* flatImageRWStuff(int argc, char** argv)
{
    int holdImage = imageManager.openPNG("../Skull.png");
    if(holdImage < 0) fprintf(stderr,"Error, couldn't read PPM file.\n");
    image* img = imageManager.getImgPtr(holdImage);
    //int count = stoi(argv[2], NULL, 10);

    //int count = stoi(argv[2], NULL, 10);
    vecMask.initScreen(img);

    holdImage = imageManager.openPNG("../Close-Up-Snail-Wallpaper.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    layer1.initScreen(img);

    holdImage = imageManager.openPNG("../serratedMetalBMap.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    outerMask.initScreen(img);

    holdImage = imageManager.openPNG("../woodTex.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    innerMask.initScreen(img);

    holdImage = imageManager.openPNG("../SkullAlpha.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    alphaMask.initScreen(img);

    holdImage = imageManager.openPNG("../serratedMetalBMap.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    bumpMap.initScreen(img);

    holdImage = imageManager.openPNG("../flatBump.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    bumpMap1.initScreen(img);

    holdImage = imageManager.openPNG("../RocksNormal.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    normMap.initScreen(img);

    holdImage = imageManager.openPNG("../Skull.png");
    img = imageManager.getImgPtr(holdImage);
    Screen.initScreen(img);
    Screen.setBackground(0, 0, 0);
    //Screen.initScreen(800, 800);
    //Screen.clearScreen();
    //Screen.psychedelic(1);
    Screen.setKernel(7, 7);

    img = Screen.getPtr();

    return img;
}


void initShade()
{
    vShade = shaderManager.loadVertexShader("../PBMEngine/vertshade_2d");
    fShade = shaderManager.loadFragmentShader("../PBMEngine/FragShade_2d");
    cShade = shaderManager.combineShaders(vShade, fShade);
    shaderManager.set3dShaderProgram(cShade);
}

void initMatricies(int width, int height)
{
    Proj = glm::mat4(1.0f);

    Model = glm::mat4(1.0f);
    modelViewProj = Proj * View * Model;

    int shader = shaderManager.getCombinedShader(cShade);
    glUniform1i(tex, 0);
    GLuint mvpID = glGetUniformLocation(shader, "MVPMat");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));
}

void initSphere()
{
    sModelIndex = modelManager.readObj("../testObj.obj");
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    sModel.setModel(modelManager.getModel(sModelIndex));


    GLuint tough = shaderManager.getCombinedShader(cShade);
    shaderManager.configure3DShaders(tough, &sModel);
}

void initParticle(float* vertices, float* colors, unsigned int* indicies)
{
    glGenVertexArrays(1, &vao2);
    glBindVertexArray(vao2);

    poModel.setModel(modelManager.getModel(sModelIndex));
    //poModel.setVertexBuffer(vertices, 9);
    //poModel.setColorBuffer(colors, 9);
    //poModel.setIndexBuffer(indicies, 3);




    GLuint tough = shaderManager.getCombinedShader(cShade);
    shaderManager.configure3DShaders(tough, &sModel);
}

void initPlane(float* vertices, float* colors, unsigned int* indicies)
{
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);


    pModel.setVertexBuffer(vertices, 12);
    pModel.setColorBuffer(colors, 12);
    pModel.setIndexBuffer(indicies, 6);

    GLuint tough = shaderManager.getCombinedShader(cShade);
    shaderManager.configure3DShaders(tough, &pModel);
}

void initTexture()
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char pixels[] = {
        (unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255, 	(unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255,
        (unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255,       (unsigned char)255, (unsigned char)255, (unsigned char)0, (unsigned char)255
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

}

void initTexture(image* texture)
{
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char pixels[] = {
        (unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255, 	(unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255,
        (unsigned char)255, (unsigned char)255, (unsigned char)255, (unsigned char)255,       (unsigned char)255, (unsigned char)255, (unsigned char)0, (unsigned char)255
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);

}

void testTexture()
{
    display();
}


void display()
{
    int shader = shaderManager.getCombinedShader(cShade);
    GLint mvpID = glGetUniformLocation(shader, "MVPMat");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    //draw box faces
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vao1);

    //draw plane2
    plane2.updateRenderObject();
    Model = *(plane2.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    RenderObject* puts = plane2.getRenderObj();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    model* hold  = plane2.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);

    glutSwapBuffers();
}

void Timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(refreshMills, Timer, 0);
}

unsigned long getTickCount()
{
    struct timespec tv;
    if(clock_gettime(CLOCK_MONOTONIC, &tv) == 0)
    {
        return ((long)tv.tv_sec * 1000) + (tv.tv_nsec / 1000000);
    }
}

void KeyHandler(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'z':
    {
        if(progState != FUNC3D) progState = FUNC3D;
        else
        {
            geometry gTest;
            gTest.depth = 10.0f;
            gTest.radius = 1.0f;
            gTest.width = 0.9f;
            SphereFunction3D hold;
            hold.setPoint(glm::vec3(0.0f, -50.0f, -10.0f));
            hold.setPoint2(glm::vec3(0.0f, -100.0f, 0.0f));
            //hold.setPoint2(glm::vec3(0.0f, -100.0f, 200.0f));
            hold.setRadius(100.0f);
            hold.setShnell(1.1);
            hold.setBlur(0.0);
            hold.setTexProj(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 0.7f), 1);
            hold.setColor(glm::vec4(10.0f, 10.0f, 10.0f, 10.0f), glm::vec4(10.0f, 10.0f, 10.0f, 10.0f), glm::vec4(10.0f, 10.0f, 10.0f, 10.0f));
            hold.setGeometry(gTest);
            hold.setTexNorms(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::normalize(glm::vec3(-0.0f, 0.0f, 1.0f)));
            hold.setTexture(&outerMask);
            hold.setNormalMap(&normMap);
            hold.setBumpMap(&bumpMap1);
            hold.setDisp(0);
            hold.shader = &Shaders::refractor;
            Screen.addFunction3D(&hold);
            SphereFunction3D hold9;
            hold9.setPoint(glm::vec3(-110.0f, -200.0f, 0.0f));
            hold9.setPoint2(glm::vec3(0.0f, -100.0f, 0.0f));
            //hold.setPoint2(glm::vec3(0.0f, -100.0f, 200.0f));
            hold9.setRadius(80.0f);
            hold9.setShnell(1.3);
            hold9.setBlur(0.0);
            hold9.setTexProj(glm::vec3(0.0f, 0.0f, -200.0f), glm::vec3(0.5f, 0.0f, 0.7f), 100);
            hold9.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(10.0f, 10.0f, 10.0f, 10.0f));
            hold9.setGeometry(gTest);
            hold9.setTexNorms(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::normalize(glm::vec3(-0.0f, 0.0f, 1.0f)));
            hold9.setTexture(&outerMask);
            hold9.setNormalMap(&normMap);
            hold9.setBumpMap(&bumpMap1);
            hold9.setDisp(0);
            hold9.shader = &Shaders::voronoi;
            //Screen.addFunction3D(&hold9);
            gTest.depth = 1.0f;
            TriangleMesh hold6;
            hold6.setShnell(1.25);
            hold6.setPoint(glm::vec3(-175.0f, -50.0f, -350.0f));
            hold6.setPoint2(glm::vec3(0.0f, -200.0f, -600.0f));
            hold6.setNormal(glm::vec3(0.0f, -1.0f, 0.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f));
            hold6.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 100.0f, 100.0f), glm::vec4(0.0f, 0.0f, 10.0f, 10.0f));
            hold6.setGeometry(gTest);
            hold6.setOCoeff(2.0f);
            hold6.setcBCoeff(2.0f);
            hold6.setTexture(&outerMask);
            hold6.shader = &Shaders::caustics;
            //hold6.createTetrahedron(300);
            //hold6.createCube(500);
            hold6.loadFromModel(modelManager.getModel(capModel), 1.5);
            Screen.addFunction3D(&hold6);
            //TriangleFunction hold5(glm::vec3(100.0f, 50.0f, -200.0f), glm::vec3(150.0f, -150.0f, 0.0f), glm::vec3(0.0f, 50.0f, 200.0f));
            //hold5.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 7000.0f, 7000.0f));
            //hold5.setGeometry(gTest);
            //hold5.setT0exture(&innerMask);
            //hold5.shader = &Shaders::phongShadow;
            //Screen.addFunction3D(&hold5);
            gTest.depth = 0.1f;
            SphereFunction3D hold2;
            hold2.setPoint(glm::vec3(-150.0f, 50.0f, 50.0f));
            hold2.setRadius(50.0f);
            hold2.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(0.0f, 100.0f, 0.0f, 100.0f), glm::vec4(0.0f, 10.0f, 0.0f, 10.0f));
            hold2.setGeometry(gTest);
            hold2.setOCoeff(2.0f);
            hold2.setcBCoeff(2.0f);
            hold2.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold2);
            SphereFunction3D hold7;
            hold7.setPoint(glm::vec3(150.0f, -50.0f, -50.0f));
            hold7.setRadius(50.0f);
            hold7.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(10.0f, 10.0f, 10.0f, 10.0f));
            hold7.setGeometry(gTest);
            hold7.setOCoeff(2.0f);
            hold7.shader = &Shaders::mirror;
            Screen.addFunction3D(&hold7);
            SphereFunction3D hold8;
            hold8.setPoint(glm::vec3(150.0f, 50.0f, 50.0f));
            hold8.setRadius(50.0f);
            hold8.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(10.0f, 0.0f, 0.0f, 10.0f));
            hold8.setGeometry(gTest);
            hold8.setOCoeff(2.0f);
            hold8.setcBCoeff(2.0f);
            hold8.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold8);
            gTest.depth = 1.0f;
            gTest.radius = 1.0f;
            gTest.width = 0.99;
            PlaneFunction hold3;
            hold3.setPoint(glm::vec3(0.0f, 100.0f, -100.0f));
            hold3.setPoint2(glm::vec3(0.0f, 100.0f, -100.0f));
            hold3.setNormal(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)));
            hold3.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(100.0f, 100.0f, 0.0f, 100.0f), glm::vec4(10.0f, 10.0f, 0.0f, 10.0f));
            hold3.setGeometry(gTest);
            hold3.setTexture(&innerMask);
            hold3.setOCoeff(2.0f);
            hold3.setcBCoeff(2.0f);
            hold3.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold3);
            PlaneFunction hold10;
            hold10.setPoint(glm::vec3(400.0f, -400.0f, -600.0f));
            hold10.setPoint2(glm::vec3(0.0f, 100.0f, -100.0f));
            hold10.setNormal(glm::normalize(glm::vec3(-1.0f, -0.0f, 0.0f)));
            hold10.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(10.0f, 0.0f, 0.0f, 10.0f));
            hold10.setGeometry(gTest);
            hold10.setTexture(&innerMask);
            hold10.setOCoeff(2.0f);
            hold10.setcBCoeff(2.0f);
            hold10.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold10);
            PlaneFunction hold11;
            hold11.setPoint(glm::vec3(400.0f, -400.0f, -600.0f));
            hold11.setPoint2(glm::vec3(0.0f, 100.0f, -100.0f));
            hold11.setNormal(glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
            hold11.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(100.0f, 100.0f, 100.0f, 100.0f), glm::vec4(10.0f, 10.0f, 10.0f, 10.0f));
            hold11.setGeometry(gTest);
            hold11.setTexture(&innerMask);
            hold11.setOCoeff(2.0f);
            hold11.setcBCoeff(2.0f);
            hold11.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold11);
            PlaneFunction hold12;
            hold12.setPoint(glm::vec3(400.0f, -400.0f, -600.0f));
            hold12.setPoint2(glm::vec3(0.0f, 100.0f, -100.0f));
            hold12.setNormal(glm::normalize(glm::vec3(0.0f, -0.0f, 1.0f)));
            hold12.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(100.0f, 100.0f, 100.0f, 100.0f), glm::vec4(10.0f, 10.0f, 10.0f, 10.0f));
            hold12.setGeometry(gTest);
            hold12.setTexture(&innerMask);
            hold12.setOCoeff(2.0f);
            hold12.setcBCoeff(2.0f);
            hold12.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold12);
            PlaneFunction hold13;
            hold13.setPoint(glm::vec3(-400.0f, -400.0f, -600.0f));
            hold13.setPoint2(glm::vec3(0.0f, 100.0f, -100.0f));
            hold13.setNormal(glm::normalize(glm::vec3(1.0f, -0.0f, 0.0f)));
            hold13.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 100.0f, 100.0f), glm::vec4(0.0f, 0.0f, 10.0f, 10.0f));
            hold13.setGeometry(gTest);
            hold13.setTexture(&innerMask);
            hold13.setOCoeff(2.0f);
            hold13.setcBCoeff(2.0f);
            hold13.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold13);
            PlaneFunction hold14;
            hold14.setPoint(glm::vec3(400.0f, -400.0f, 701.0f));
            hold14.setPoint2(glm::vec3(0.0f, 100.0f, -100.0f));
            hold14.setNormal(glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)));
            hold14.setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(100.0f, 100.0f, 100.0f, 100.0f), glm::vec4(10.0f, 10.0f, 10.0f, 10.0f));
            hold14.setGeometry(gTest);
            hold14.setTexture(&innerMask);
            hold14.setOCoeff(2.0f);
            hold14.setcBCoeff(2.0f);
            hold14.shader = &Shaders::caustics;
            Screen.addFunction3D(&hold14);
            InfinitySphere hold4;
            hold4.setRadius(1000000.0f);
            hold4.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 7000.0f, 7000.0f));
            hold4.setTexture(&outerMask);
            hold4.setTexNorms(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::normalize(glm::vec3(0.5f, 0.0f, -1.0f)));
            hold4.shader = &Shaders::skySphere;
            //Screen.addFunction3D(&hold4);

            DirectionalLight light;
            light.initialize(glm::normalize(glm::vec3(0.0f, 0.5f, -0.5f)));
            light.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f));
            //Screen.addLight(&light);
            PointLight light2;
            light2.initialize(glm::vec3(-0.0f, -350.0f, -50.0f));
            light2.setColor(glm::vec4(10.0f, 10.0f, 10.0f, 10.0f));
            Screen.addLight(&light2);
            SpotLight light3;
            light3.initialize(glm::vec3(400.0f, -400.0f, 0.0f), glm::normalize(glm::vec3(-0.5f, 0.5f, 0.0f)), 0.6, 0.5);
            light3.setColor(glm::vec4(500.0f, 500.0f, 5000.0f, 5000.0f));
            //Screen.addLight(&light3);
            SpotLight light5;
            light5.initialize(glm::vec3(-0.0f, -350.0f, -50.0f), glm::normalize(glm::vec3(0.0f, 1.0f, -0.0f)), 0.6, 0.5);
            light5.setColor(glm::vec4(5000.0f, 5000.0f, 5000.0f, 5000.0f));
            //Screen.addLight(&light5);
            AreaLight light4;
            light4.initialize(glm::vec3(-0.0f, -350.0f, -50.0f), glm::normalize(glm::vec3(0.0f, 0.00f, -1.0f)), glm::vec3(0.0f, 1.0f, 0.0f), 100, 100, 1);
            light4.setColor(glm::vec4(5000.0f, 5000.0f, 5000.0f, 5000.0f));
            //Screen.addLight(&light4);
            DirectionalProjection light6;
            light6.initialize(glm::normalize(glm::vec3(0.0f, 0.5f, -0.5f)), glm::vec3(300.0f, -200.0f, -200.0f));
            light6.setTex(&normMap);
            light6.setColor(glm::vec4(6000.0f, 6000.0f, 6000.0f, 6000.0f));
            //Screen.addLight(&light6);
            PerspectiveProjection light7;
            light7.initialize(glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(0.0f, -100.0f, 200.0f), 200);
            light7.setTex(&normMap);
            light7.setColor(glm::vec4(6000.0f, 6000.0f, 6000.0f, 6000.0f));
            //Screen.addLight(&light7);


            Screen.setAmbRad(100);
            Screen.setOccFall(10.0f);
            //Screen.draw3D(glm::vec3(600.0f, -700.0f, 200.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-0.6f, 0.7f, -0.2f)), (float)Screen.getHeight(), 1);
            Screen.draw3DFocus(glm::vec3(0.0f, -100.0f, 700.0f), 800.0f, 800.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-0.0f, 0.0f, -1.0f)), (float)Screen.getHeight(), 0.0f, 4);
            //Screen.draw3DPaint(glm::vec3(00.0f, -0.0f, 350.0f), 900.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-0.0f, 0.0f, -0.2f)), (float)Screen.getHeight(), 1, layer1.getPtr(), glm::vec3(1.0f));
            //Screen.draw3DFocusMBlur(glm::vec3(600.0f, -700.0f, 200.0f), 500.0f, 800.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-0.6f, 0.7f, -0.2f)), (float)Screen.getHeight(), 0.0f, 16);
            //Screen.draw3D(glm::vec3(780.0f, 88.0f, -100.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-1.0f, -0.1f, -0.0f)), (float)Screen.getHeight(), 1);
            //Screen.draw3DFocus(glm::vec3(780.0f, 88.0f, -100.0f), 1000.0f, 500.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-1.0f, 0.1f, -0.0f)), (float)Screen.getHeight(), 0.0f, 1);
            //Screen.draw3DPaint(glm::vec3(780.0f, 88.0f, -100.0f), 500.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-1.0f, -0.1f, -0.0f)), (float)Screen.getHeight(), 1, layer1.getPtr(), glm::vec3(1.0f));
            //Screen.draw3DFocusMBlur(glm::vec3(780.0f, 88.0f, -100.0f), 500.0f, 500.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-1.0f, -0.1f, -0.0f)), (float)Screen.getHeight(), 0.0f, 1);
            /*int count = 50;
            int idx = imageManager.addImage(*Screen.getPtr());
            string nameBuf;// = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
            //imageManager.writePNG((char*)nameBuf.c_str(), idx);
            float sH = 1.2;
            sH -= 50 * (1.2/(24.0f * 3.0f));
            for(int i = 50; i < 24 * 3; i++)
            {
                hold.setShnell(sH);
                glm::vec3 cPos2 = glm::vec3(780.0f, 88.0f, -100.0f);
                glm::vec3 cPos1 = glm::vec3(500.0f, -500.0f, 700.0f);
                glm::vec3 cAng2 = glm::normalize(glm::vec3(-1.0f, 0.1f, -0.0f));
                glm::vec3 cAng1 = glm::normalize(glm::vec3(-0.5f, 0.5f, -0.7f));
                Screen.draw3DFocus(cPos2 * ((float)i/(float)(24.0*3.0)) + cPos1 * (1 - ((float)i/(float)(24.0*3.0))), 1000.0f, 500.0f, glm::vec3(0.0f, 1.0f, 0.0f), cAng2 * ((float)i/(float)(24.0*3.0)) + cAng1 * (1 - ((float)i/(float)(24.0*3.0))), (float)Screen.getHeight(), 0.0f, 8);
                //Screen.draw3DFocus(glm::vec3(-400.0f, -500.0f, 1000.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(0.35f, 0.5f, -1.0f)), (float)Screen.getHeight(), 1);
                //Screen.draw3DFocus(glm::vec3(0.0f, 0.0f, 1000.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), (float)Screen.getHeight(), 1);
                nameBuf = std::string("../Mov1/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
                sH -= 1.2/(24.0f * 3.0f);
            }*/
            /*int count = 67;
            int idx = imageManager.addImage(*Screen.getPtr());
            string nameBuf;// = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
            //imageManager.writePNG((char*)nameBuf.c_str(), idx);
            float sH = 0.0;
            sH = 0.05*67;
            for(int i = 67; i < 24 * 3; i++)
            {
                Screen.draw3DPaint(glm::vec3(00.0f, -0.0f, 350.0f), 900.0f, glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(-0.0f, 0.0f, -0.2f)), (float)Screen.getHeight(), 8, layer1.getPtr(), glm::vec3(sH/1, -sH/2, sH*2));
                //Screen.draw3DFocus(glm::vec3(-400.0f, -500.0f, 1000.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(0.35f, 0.5f, -1.0f)), (float)Screen.getHeight(), 1);
                //Screen.draw3DFocus(glm::vec3(0.0f, 0.0f, 1000.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), (float)Screen.getHeight(), 1);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
                sH += 0.05;
            }*/
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
        break;
    case'x':
    {
        progState = CONVEX;
        Screen.clearScreen();
        image* img = Screen.getPtr();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        numFuncs = 0;
        Screen.emptyFunctions();
    }
        break;
    case 'c':
    {
        if(progState != EMBOSS)
        {
            progState = EMBOSS;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.emboss();
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
        break;
    case 'v':
    {
        if(progState != DILATION)
        {
            progState = DILATION;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.dilation();
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
        break;
    case 'b':
    {
        if(progState != EROSION)
        {
            progState = EROSION;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.erosion();
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
        break;
    case 'n':
    {
        if(progState != MBLUR)
        {
            progState = MBLUR;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.maskedMBlur(&vecMask);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
        break;
    case 'm':
    {
        if(progState != MDILATION)
        {
            progState = MDILATION;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.maskedDilation(&vecMask);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
        break;
    case 'l':
    {
        if(progState != MEROSION)
        {
            progState = MEROSION;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.maskedErosion(&vecMask);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
        break;
    case 'k':
    {
        if(progState != BDLPF)
        {
            progState = BDLPF;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.bdlpf();
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
    }
    case 'p':
    {
        if(progState != ALPHA)
        {
            progState = ALPHA;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.alphaLayer(&layer1);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'o':
    {
        if(progState != MULT)
        {
            progState = MULT;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.multiplyLayer(&layer1);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'i':
    {
        if(progState != SUBTRACT)
        {
            progState = SUBTRACT;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.subtractionLayer(&layer1);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'u':
    {
        if(progState != MIN)
        {
            progState = MIN;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.minLayer(&layer1);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'y':
    {
        if(progState != MAX)
        {
            progState = MAX;
            Screen.setKernelValues(1.0f);
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.maxLayer(&layer1);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'w':
    {
        if(progState != COMPOSITE)
        {
            progState = COMPOSITE;
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            layer1.composite(&outerMask, &innerMask);
            Screen.alphaLayer(&layer1);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'e':
    {
        if(progState != DITHER)
        {
            progState = DITHER;
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.oDither(5);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'r':
    {
        if(progState != FSDITHER)
        {
            progState = FSDITHER;
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.fsDither();
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 't':
    {
        if(progState != CDITHER)
        {
            progState = CDITHER;
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.cDither(2);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'a':
    {
        if(progState != DIFFUSE)
        {
            progState = DIFFUSE;
            layer1.makeNormal(&vecMask);
            tCount = 0;
            lHeight = 256.0f;
        }
        else
        {
            /*
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::vec3 l = glm::vec3(100.0f, 50.0f, 255.0f);
            int idx = imageManager.addImage(*Screen.getPtr());
            Screen.flipScreens();
            Screen.makeNormal(&vecMask);

            int count = 0;
            string nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
            imageManager.writePNG((char*)nameBuf.c_str(), idx);
            for(int j = 0; j < 20; j++)
            {
                l.x += j * 3;
                Screen.flipScreens();
                Screen.specLight(&layer1, &vecMask, l, PI/20);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
            }
            for(int j = 0; j < 20; j++)
            {
                l.y += j * 3;
                Screen.flipScreens();
                Screen.specLight(&layer1, &vecMask, l, PI/20);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
            }
            for(int j = 0; j < 20; j++)
            {
                l.x -= j * 3;
                Screen.flipScreens();
                Screen.specLight(&layer1, &vecMask, l, PI/20);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
            }
            for(int j = 0; j < 20; j++)
            {
                l.y -= j * 3;
                Screen.flipScreens();
                Screen.specLight(&layer1, &vecMask, l, PI/20);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
            }
            for(int j = 0; j < 20; j++)
            {
                l.x += j * 2;
                l.y += j * 2;
                l.z -= j * 1;
                Screen.flipScreens();
                Screen.specLight(&layer1, &vecMask, l, PI/20);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
            }
            for(int j = 0; j < 20; j++)
            {
                l.x += j * 2;
                l.y += j * 2;
                l.z += j * 1;
                Screen.flipScreens();
                Screen.specLight(&layer1, &vecMask, l, PI/20);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
            }
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);*/
        }
        break;
    }
    case 's':
    {
        if(progState != REFLECTION)
        {
            progState = REFLECTION;
            layer1.makeNormal(&vecMask);
            image* img = layer1.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
            tCount = 0;
            rCoeff = 1;
            refrHeight = -1.0f;
            reflHeight = 256.0f;
            fresnel = 10.0f;
        }
        else
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
        break;
    }
    case '-':
    {
        if(progState == DIFFUSE)
        {
            lHeight -= 10.0f;
        }
        if(progState == REFLECTION)
        {
            rCoeff -= 1.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case '=':
    {
        if(progState == DIFFUSE)
        {
            lHeight += 10.0f;
        }
        if(progState == REFLECTION)
        {
            rCoeff += 1.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case '[':
    {
        if(progState == REFLECTION)
        {
            refrHeight -= 10.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case ']':
    {
        if(progState == REFLECTION)
        {
            refrHeight += 10.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case ';':
    {
        if(progState == REFLECTION)
        {
            reflHeight -= 10.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case '\'':
    {
        if(progState == REFLECTION)
        {
            reflHeight += 10.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case ',':
    {
        if(progState == REFLECTION)
        {
            fresnel -= 1.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case '.':
    {
        if(progState == REFLECTION)
        {
            fresnel += 1.0f;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Screen.reflection(&vecMask, &layer1, &outerMask, &innerMask, &alphaMask, glm::vec3(0.0f), rCoeff, fresnel, reflHeight, refrHeight);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        }
        break;
    }
    case 'q':
    {
        int idx = imageManager.addImage(*Screen.getPtr());
        imageManager.writePNG("../filtered.png", idx);
    }
        break;
    default:
        break;
    }
}

void KeyUpHandler(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'w':
        kState = kState & (~FORWARD);

        break;
    case 'a':
        kState = kState & (~SLEFT);

        break;
    case 's':
        kState = kState & (~BACK);

        break;
    case 'd':
        kState = kState & (~SRIGHT);

        break;
    case 'i':
        kState = kState & (~LUP);
        break;
    case 'j':
        kState = kState & (~LLEFT);
        break;
    case 'k':
        kState = kState & (~LDOWN);
        break;
    case 'l':
        kState = kState & (~LRIGHT);
        break;
    case 'r':
    default:
        break;
    }
}

void MouseMoveHandler(int x, int y)
{
    if(progState == DIFFUSE)
    {
        if(tCount == 0)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glm::vec3 l = glm::vec3(1.0f);
            //l.x = rand() % Screen.getWidth();
            //l.y = rand() % Screen.getHeight();
            l.x = Screen.getWidth() - (float)x + 0.5;
            l.y = (float)y + 0.5;
            l.z = lHeight;

            Screen.specLight(&layer1, &vecMask, l, PI/50);
            image* img = Screen.getPtr();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
            tCount = 20;
        }
        else tCount--;
    }
}

void MouseHandler(int button, int state, int x, int y)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
    {
        if(state == GLUT_DOWN)
        {
            mouseDown = !mouseDown;
            if(mouseDown)
            {
                mDownPos = glm::vec2((float)x, (float)y);
            }
            else if(numFuncs < MAX_FUNCTIONS)
            {
                mUpPos = glm::vec2((float)x, (float)y);
                glm::vec2 lineDir = mUpPos - mDownPos;
                glm::vec2 normal = glm::vec2(0.0f, 0.0f);
                switch(progState)
                {
                case CONVEX:
                case STAR:
                    lineDir = glm::normalize(lineDir);
                    normal = glm::vec2(lineDir.y, lineDir.x);
                    functions[numFuncs] = new LineFunction(normal, mDownPos);
                    break;
                case BLOBBY:
                case SHADED:
                case MODULUS:
                    normal = -lineDir;
                    functions[numFuncs] = new SphereFunction(normal, mDownPos);
                    break;
                case BLUR:
                {
                    lineDir = glm::normalize(lineDir);
                    LineFunction l = LineFunction(lineDir, glm::vec2(0.0f, 0.0f));
                    Screen.setKernelValuesEF(&l);
                }
                    break;
                case COMPOSITE:
                    break;
                default:
                    break;
                }

                if(progState == CONVEX || progState == STAR || progState == BLOBBY || progState == SHADED || progState == MODULUS)
                {
                    Screen.addFunction(functions[numFuncs]);
                    numFuncs++;
                }

                switch(progState)
                {
                case CONVEX:
                    Screen.clearScreen();
                    Screen.drawConvexAA(3);
                    break;
                case STAR:
                    Screen.clearScreen();
                    Screen.drawStarAA(3);
                    break;
                case BLOBBY:
                    Screen.clearScreen();
                    Screen.drawBlobbyAA(3);
                    break;
                case SHADED:
                    Screen.drawShaded();
                    break;
                case MODULUS:
                    Screen.drawModAA(3, 2);
                    break;
                case COMPOSITE:
                    break;
                default:
                    break;
                }
                image* img = Screen.getPtr();
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
            }
            else fprintf(stderr, "ERROR: MAX_FUNCTIONS reached\n");
        }
        break;
    }
    default:
        break;
    }
}

