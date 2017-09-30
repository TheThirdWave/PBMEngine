#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
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
#include "function2d.h"
#include "linefunction.h"
#include "spherefunction.h"
#include "trigfunction.h"
#include "structpile.h"
#include "renderobject.h"
#include "physicsobject.h"
#include "physicsmanager.h"
#include "sphereobject.h"
#include "planeobject.h"



using namespace std;

image* flatImageRWStuff(int, char**);
void loadShades(void);
void initBuf(void);
void initPlane(float*, float*, unsigned int*);
void initShade(void);
void initMatricies(int, int);
void initTexture();
void initTexture(image*);
void configureAttributes(void);
void testRender(void);
void testTexture(void);
void update(float);
void display();
void reshape(GLsizei width, GLsizei height);
void Timer(int value);
void KeyHandler(unsigned char key, int x, int y);
void MouseHandler(int button, int state, int x, int y);
unsigned long getTickCount();

ShaderManager shaderManager;
ReaderWriter imageManager;
ModelManager modelManager;
PhysicsManager physicsManager;
Imagemanip Screen;

SphereObject sphere;
SphereObject sphere1;
PlaneObject plane, plane1, plane2, plane3, plane4, plane5;
RenderObject sModel, pModel;

unsigned long prev_time, cur_time;
glm::mat4 modelViewProj, Proj, View, Model;
GLuint buf, idx, tex, posAttrib, vao, vao1;
GLfloat angle = 0.0f;
int refreshMills = 30;
int vShade, fShade, cShade;
model* hold1;

Function2D* functions[MAX_FUNCTIONS];

int numFuncs = 0;
int numSFuncs = 0;
bool mouseDown = false;
glm::vec2 mDownPos;
glm::vec2 mUpPos;
int progState = CONVEX;

const float timeStep = 1000 / (60.0f * 5);

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
    initShade();
    initBuf();
    float vertices[] = {
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f
    };
    float colors[] = {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
    };
    unsigned int indicies[] = {
        0, 1, 2,
        3, 2, 1
    };

    initPlane(vertices, colors, indicies);
    initMatricies(width, height);

    plane1.setGeometry(glm::vec3(0.0f, 0.0f, -1.0f));
    plane1.setRenderObject(&pModel);
    plane1.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    physicsManager.addPhysObj((PhysicsObject*)&plane1);

    glutDisplayFunc(testTexture);
    glutKeyboardFunc(KeyHandler);
    glutMouseFunc(MouseHandler);
    glutTimerFunc(0, Timer, 0);

    cur_time = getTickCount();
    prev_time = cur_time;
    glutMainLoop();

    return 0;
}

image* flatImageRWStuff(int argc, char** argv)
{
    //int count = stoi(argv[2], NULL, 10);
    Screen.initScreen(800, 800);
    Screen.clearScreen();
    image* img = Screen.getPtr();

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
    //Proj = glm::perspective(glm::radians(YFOV), ((float)width / (float)height), ZNEAR, ZFAR);
    //View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    View = glm::lookAt(glm::vec3(0,0,-1.0), glm::vec3(0,0,0), glm::vec3(0,1,0));
    Model = glm::mat4(1.0f);
    modelViewProj = Proj * View * Model;

    int shader = shaderManager.getCombinedShader(cShade);
    glUniform1i(tex, 0);
    GLuint mvpID = glGetUniformLocation(shader, "MVPMat");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));
}

void initBuf()
{
    int index = modelManager.readObj("../testObj.obj");
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    sModel.setModel(modelManager.getModel(index));

    sphere.setGeometry(0.5f);
    sphere.setRenderObject(&sModel);
    sphere.setVelocity(glm::vec3(0.002f, 0.0f, 0.03f));
    physicsManager.addPhysObj((PhysicsObject*)&sphere);

    /*sphere1.setGeometry(1.0f);
    sphere1.setRenderObject(&sModel);
    sphere1.setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    physicsManager.addPhysObj((PhysicsObject*)&sphere1);*/


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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao1);
    plane1.updateRenderObject();
    Model = *(plane1.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    RenderObject* puts = plane1.getRenderObj();


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    int shader = shaderManager.getCombinedShader(cShade);
    GLint mvpID = glGetUniformLocation(shader, "MVPMat");

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    model* hold = plane1.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);

    glutSwapBuffers();
}

void testRender()
{
    prev_time = cur_time;
    cur_time = getTickCount();
    long delta_time = cur_time - prev_time;
    if(delta_time >= timeStep)
    {
        double loops  = 0;
        while(loops < delta_time)
        {
            update(timeStep);

            loops += timeStep;
        }
        display();
    }
}

void update(float tStep)
{
    physicsManager.runTimeStep(tStep);
}

void display()
{
    //draw sphere
    glBindVertexArray(vao);
    sphere.updateRenderObject();
    Model = *(sphere.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderObject* puts = sphere.getRenderObj();


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);


    int shader = shaderManager.getCombinedShader(cShade);
    GLint mvpID = glGetUniformLocation(shader, "MVPMat");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    model* hold = sphere.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);


    glBindVertexArray(vao1);

    //draw plane1
    plane1.updateRenderObject();
    Model = *(plane1.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = plane1.getRenderObj();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane1.getRenderObj()->getData();
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
    case 'r':
    {
        Screen.psychedelic(7);
        image* img = Screen.getPtr();
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        break;
    }
    case's':
    {
        int imgIdx = imageManager.addImage(Screen.getScreen());
        if(!imageManager.writePNG("../ping1AA.png", imgIdx)) fprintf(stderr, "Error: Couldn't write png file\n");
        Screen.clearScreen();
        switch(progState)
        {
        case FUNCTION:
        case CONVEX:
            Screen.drawConvex();
            break;
        case STAR:
            Screen.drawStar();
            break;
        case BLOBBY:
            Screen.drawBlobby();
            break;
        case MODULUS:
            Screen.drawMod(2);
        default:
            break;
        }
        imgIdx = imageManager.addImage(Screen.getScreen());
        if(!imageManager.writePNG("../ping1.png", imgIdx)) fprintf(stderr, "Error: Couldn't write png file\n");
        break;
    }
    case'z':
    {
        progState = CONVEX;
        numFuncs = 0;
        numSFuncs = 0;
        Screen.emptyFunctions();
        Screen.clearScreen();
        image* img = Screen.getPtr();
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        break;
    }
    case'x':
    {
        progState = STAR;
        numFuncs = 0;
        numSFuncs = 0;
        Screen.emptyFunctions();
        Screen.clearScreen();
        image* img = Screen.getPtr();
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        break;
    }
    case'c':
    {
        progState = BLOBBY;
        numFuncs = 0;
        numSFuncs = 0;
        Screen.emptyFunctions();
        Screen.clearScreen();
        image* img = Screen.getPtr();
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        break;
    }
    case 'v':
    {
        progState = SHADED;
        numFuncs = 0;
        Screen.emptyFunctions();
        Screen.clearScreen();
        functions[numFuncs] = new SphereFunction(glm::vec2(0, 400), glm::vec2(400, 400));
        Screen.addFunction(functions[numFuncs]);
        numFuncs++;
        Screen.drawConvexAA(2);
        numFuncs = 0;
        Screen.emptyFunctions();
        image* img = Screen.getPtr();
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        break;
    }
    case'b':
    {
        progState = MODULUS;
        numFuncs = 0;
        numSFuncs = 0;
        Screen.emptyFunctions();
        Screen.clearScreen();
        image* img = Screen.getPtr();
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
        break;
    }
    case'n':
    {
        progState = FUNCTION;
        numFuncs = 0;
        numSFuncs = 0;
        Screen.emptyFunctions();
        Screen.clearScreen();
        functions[numFuncs] = new TrigFunction(glm::vec2(0, 400), glm::vec2(400, 400));
        Screen.addFunction(functions[numFuncs]);
        numFuncs++;
        Screen.drawConvexAA(3);
        image* img = Screen.getPtr();
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
    }
    default:
        break;
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
                default:
                    break;
                }

                Screen.addFunction(functions[numFuncs]);
                numFuncs++;

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
                default:
                    break;
                }
                image* img = Screen.getPtr();
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->height, img->width, GL_RGBA, GL_UNSIGNED_BYTE, img->data);
            }
            else fprintf(stderr, "ERROR: MAX_FUNCTIONS reached\n");
        }
        break;
    }
    default:
        break;
    }
}

