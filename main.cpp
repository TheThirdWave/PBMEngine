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
Imagemanip Screen, vecMask, layer1, outerMask, innerMask, alphaMask;

PolygonObject plane2;
RenderObject sModel, pModel, poModel;

unsigned int kState = 0;

unsigned long prev_time, cur_time;
glm::mat4 modelViewProj, Proj, View, Model;
GLuint buf, idx, tex, posAttrib, vao, vao1, vao2;
GLfloat angle = 0.0f;
int refreshMills = 30;
int vShade, fShade, cShade, sModelIndex, tCount;
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

    holdImage = imageManager.openPNG("../AlphaTest2.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    layer1.initScreen(img);

    holdImage = imageManager.openPNG("../Skybox.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    outerMask.initScreen(img);

    holdImage = imageManager.openPNG("../GWbackground.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    innerMask.initScreen(img);

    holdImage = imageManager.openPNG("../SkullAlpha.png");
    if(holdImage < 0) fprintf(stderr, "Error, couldn't read PPM file.\n");
    img = imageManager.getImgPtr(holdImage);

    alphaMask.initScreen(img);

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
            hold.setPoint(glm::vec3(300.0f, 0.0f, 0.0f));
            hold.setRadius(100.0f);
            hold.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 7000.0f, 7000.0f));
            hold.setGeometry(gTest);
            hold.setTexNorms(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::normalize(glm::vec3(-0.0f, 0.0f, 1.0f)));
            hold.setTexture(&vecMask);
            hold.setDisp(15);
            hold.shader = &Shaders::texMap;
            //Screen.addFunction3D(&hold);
            TriangleMesh hold6;
            hold6.setPoint(glm::vec3(50.0f, 100.0f, 0.0f));
            hold6.setNormal(glm::vec3(0.0f, -1.0f, 0.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.0f));
            hold6.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 7000.0f, 7000.0f));
            hold6.setGeometry(gTest);
            hold6.setTexture(&innerMask);
            hold6.shader = &Shaders::phongShadow;
            hold6.createTetrahedron(100);
            Screen.addFunction3D(&hold6);
            //TriangleFunction hold5(glm::vec3(100.0f, 50.0f, -200.0f), glm::vec3(150.0f, -150.0f, 0.0f), glm::vec3(0.0f, 50.0f, 200.0f));
            //hold5.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 7000.0f, 7000.0f));
            //hold5.setGeometry(gTest);
            //hold5.setTexture(&innerMask);
            //hold5.shader = &Shaders::phongShadow;
            //Screen.addFunction3D(&hold5);
            gTest.depth = 5.0f;
            SphereFunction3D hold2;
            hold2.setPoint(glm::vec3(-400.0f, -100.0f, 0.0f));
            hold2.setRadius(40.0f);
            hold2.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(0.0f, 100.0f, 0.0f, 100.0f), glm::vec4(5000.0f, 0.0f, 5000.0f, 5000.0f));
            hold2.setGeometry(gTest);
            hold2.shader = &Shaders::phongShadow;
            //Screen.addFunction3D(&hold2);
            gTest.depth = 40.0f;
            gTest.radius = 1.0f;
            gTest.width = 0.99;
            PlaneFunction hold3;
            hold3.setPoint(glm::vec3(0.0f, 100.0f, -100.0f));
            hold3.setNormal(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)));
            hold3.setColor(glm::vec4(800.0f, 800.0f, 1000.0f, 1000.0f), glm::vec4(0.0f, 0.0f, 100.0f, 100.0f), glm::vec4(0.0f, 0.0f, 1000.0f, 1000.0f));
            hold3.setGeometry(gTest);
            hold3.setTexture(&innerMask);
            hold3.shader = &Shaders::diffuseShadow;
            Screen.addFunction3D(&hold3);
            InfinitySphere hold4;
            hold4.setRadius(1000000.0f);
            hold4.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f), glm::vec4(100.0f, 0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 7000.0f, 7000.0f));
            hold4.setTexture(&outerMask);
            hold4.setTexNorms(glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f)), glm::normalize(glm::vec3(0.5f, 0.0f, -1.0f)));
            hold4.shader = &Shaders::skySphere;
            //Screen.addFunction3D(&hold4);

            DirectionalLight light;
            light.initialize(glm::normalize(glm::vec3(0.0f, 0.5f, 0.5f)));
            light.setColor(glm::vec4(200.0f, 200.0f, 200.0f, 200.0f));
            Screen.addLight(&light);
            PointLight light2;
            light2.initialize(glm::vec3(-300.0, -200.0f, 0.0f));hold6.createTetrahedron(200);
            light2.setColor(glm::vec4(1000.0f, 1000.0f, 1000.0f, 1000.0f));
            //Screen.addLight(&light2);
            SpotLight light3;
            light3.initialize(glm::vec3(100.0f, -400.0f, 0.0f), glm::normalize(glm::vec3(-0.5f, 0.5f, 0.0f)), 0.6, 0.5);
            light3.setColor(glm::vec4(500.0f, 500.0f, 500.0f, 500.0f));
            //Screen.addLight(&light3);
            SpotLight light5;
            light5.initialize(glm::vec3(-100.0f, -300.0f, 0.0f), glm::normalize(glm::vec3(0.5f, 0.5f, 0.0f)), 0.6, 0.5);
            light5.setColor(glm::vec4(500.0f, 500.0f, 500.0f, 500.0f));
            Screen.addLight(&light5);
            AreaLight light4;
            light4.initialize(glm::vec3(100.0f, -300.0f, 0.0f), glm::normalize(glm::vec3(-0.5f, 0.5f, 0.0f)), glm::vec3(0.0f, 1.0f, 0.0f), 100, 100, 1);
            light4.setColor(glm::vec4(500.0f, 500.0f, 500.0f, 500.0f));
            //Screen.addLight(&light4);
            /*QuadraticFunction3D hold4;
            hold4.setPoint(glm::vec3((float)Screen.getWidth() / 2.0f - 300, (float)Screen.getHeight() / 2.0f, 0.0f));
            hold4.setNormal(glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
            hold4.setQParams(0, 0, 0, 1, 0);
            hold4.setQReals(1, 1, 1);
            hold4.setColor(glm::vec4(0.0f, 0.0f, 100.0f, 100.0f));
            Screen.addFunction3D(&hold4);
            QuadraticFunction3D hold5;
            hold5.setPoint(glm::vec3((float)Screen.getWidth() / 2.0f - 200, (float)Screen.getHeight() / 2.0f - 200, 0.0f));
            hold5.setNormal(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            hold5.setQParams(1, 1, 0, 0, -1);
            hold5.setQReals(100.0f, 50.0f, 100.0);
            hold5.setColor(glm::vec4(0.0f, 100.0f, 100.0f, 100.0f));
            Screen.addFunction3D(&hold5);*/
            Screen.draw3D(glm::vec3(0.0f, 0.0f, 1000.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), (float)Screen.getHeight(), 1);
            int count = 0;
            int idx = imageManager.addImage(*Screen.getPtr());
            string nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
            imageManager.writePNG((char*)nameBuf.c_str(), idx);
            for(int i = 0; i < 40; i++)
            {
                if(i < 10)hold6.setNormal(glm::vec3(0.0f, -1.0f, 0.0f), glm::normalize(glm::vec3(1.0f - ((float)i/10.0f), 0.0f, ((float)i/10.0f))), glm::vec3(0.0f));
                else if(i < 20) hold6.setNormal(glm::vec3(0.0f, -1.0f, 0.0f), glm::normalize(glm::vec3(-((float)(i - 10)/10.0f), 0.0f, (1.0f - (float)(i - 10)/10.0f))), glm::vec3(0.0f));
                else if(i < 30) hold6.setNormal(glm::vec3(0.0f, -1.0f, 0.0f), glm::normalize(glm::vec3(-1.0f + ((float)(i - 20)/10.0f), 0.0f, ( - (float)(i - 20)/10.0f))), glm::vec3(0.0f));
                else hold6.setNormal(glm::vec3(0.0f, -1.0f, 0.0f), glm::normalize(glm::vec3(((float)(i - 30)/10.0f), 0.0f, (-1.0f + (float)(i - 30)/10.0f))), glm::vec3(0.0f));
                hold6.createTetrahedron(200);
                Screen.draw3D(glm::vec3(0.0f, 0.0f, 1000.0f), 500.0f,  glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), (float)Screen.getHeight(), 1);
                nameBuf = std::string("../Mov2/frame" + std::to_string(count++) + ".png");
                imageManager.writePNG((char*)nameBuf.c_str(), idx);
            }
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

