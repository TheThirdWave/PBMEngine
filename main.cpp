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
#include "quadraticfunction.h"
#include "structpile.h"
#include "renderobject.h"
#include "physicsobject.h"
#include "physicsmanager.h"
#include "edgeobject.h"
#include "sphereobject.h"
#include "particleobject.h"
#include "planeobject.h"
#include "camera.h"



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
void setFace(ParticleObject*, ParticleObject*, ParticleObject*, ParticleObject*, int idx);
void setFace1(ParticleObject*, ParticleObject*, ParticleObject*, ParticleObject*, int idx);
void configureAttributes(void);
void testRender(void);
void testTexture(void);
void update(float);
void handleKeyStates(float);
void display();
void reshape(GLsizei width, GLsizei height);
void Timer(int value);
void KeyHandler(unsigned char key, int x, int y);
void MouseHandler(int button, int state, int x, int y);
void KeyUpHandler(unsigned char key, int x, int y);
unsigned long getTickCount();

ShaderManager shaderManager;
ReaderWriter imageManager;
ModelManager modelManager;
PhysicsManager physicsManager;
Imagemanip Screen;

SphereObject sphere, sphere1;
EdgeObject edge[18];
ParticleObject* part;
ParticleGenerator pGen;
PolygonObject plane1, plane2, plane3, plane4, plane5;
PolygonObject plane[6];
RenderObject sModel, pModel, poModel;

Camera camera;
unsigned int kState = 0;

unsigned long prev_time, cur_time;
glm::mat4 modelViewProj, Proj, View, Model;
GLuint buf, idx, tex, posAttrib, vao, vao1, vao2;
GLfloat angle = 0.0f;
int refreshMills = 30;
int vShade, fShade, cShade, sModelIndex;
model* hold1;

Function2D* functions[MAX_FUNCTIONS];

int numFuncs = 0;
int numSFuncs = 0;
bool mouseDown = false;
glm::vec2 mDownPos;
glm::vec2 mUpPos;
int progState = CONVEX;

const float timeStep = 1000 / (60.0f * 20);

int rotation = 15;


const char* vertShade;

const char* fragShade;


int main(int argc, char *argv[])
{
    int width = 1920;
    int height = 1080;
    part = new ParticleObject[NUM_PARTS];



//    image* img = flatImageRWStuff(argc, argv);

    glutInit(&argc, argv);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);

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
//    initTexture(img);
    camera.setViewMatrix(&View);
    initMatricies(width, height);

    initShade();

    //create sphere vao
    initSphere();

    sphere.setGeometry(0.5f);
    sphere.setRenderObject(&sModel);
    sphere.setVelocity(glm::vec3(-0.00f, 0.0f, 0.0f));
    sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
    sphere.setPosition(glm::vec3(-100.0f, 1.0f, 1.0f));
    physicsManager.addPhysObj((PhysicsObject*)&sphere);


    /*sphere1.setGeometry(1.0f);
    sphere1.setRenderObject(&sModel);
    sphere1.setPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    physicsManager.addPhysObj((PhysicsObject*)&sphere1);*/

    float verticies0[] = {
        0.0f, -(float)std::sin(60)/2, 0.0f,
        -0.5f, (float)std::sin(60)/2, 0.0f,
        0.5f, (float)std::sin(60)/2, 0.0f,
    };
    float colors0[] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    unsigned int indicies0[] = {
        0, 1, 2,
    };

    //create polygon vao
    initParticle(verticies0, colors0, indicies0);
/*
    for(int i = 0; i < NUM_PARTS; i++)
    {
        part[i].setGeometry(glm::normalize((camera.getPosition() - part[i].getPosition())), glm::vec3(0.0f, 0.0f, -1.0f));
        part[i].setRenderObject(&sModel);
        part[i].setVelocity(glm::vec3(0.02f, 0.0f, 0.0f));
        part[i].setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        part[i].setTTL(-0);
    }
    physicsManager.addParticleList(part, NUM_PARTS);
*/
/*
    part[0].setGeometry(glm::normalize((camera.getPosition() - part[0].getPosition())), glm::vec3(0.0f, 0.0f, -1.0f));
    part[0].setRenderObject(&sModel);
    part[0].setPosition(glm::vec3(-1.0, 1.0f, 0.0f));
    part[0].setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    part[0].setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    part[0].setTTL(-0);
    part[1].setGeometry(glm::normalize((camera.getPosition() - part[1].getPosition())), glm::vec3(0.0f, 0.0f, -1.0f));
    part[1].setRenderObject(&sModel);
    part[1].setPosition(glm::vec3(-1.0, -1.0f, 0.0f));
    part[1].setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    part[1].setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    part[1].setTTL(-0);
    physicsManager.addPhysObj(&part[0]);
    physicsManager.addPhysObj(&part[1]);

    edge.addChild(&part[0]);
    edge.addChild(&part[1]);
    edge.setSpring(3.0, 0.001, 0.0005);
    physicsManager.addPhysObj(&edge);

    part[2].setGeometry(glm::normalize((camera.getPosition() - part[0].getPosition())), glm::vec3(0.0f, 0.0f, -1.0f));
    part[2].setRenderObject(&sModel);
    part[2].setPosition(glm::vec3(1.0, 1.0f, 1.0f));
    part[2].setVelocity(glm::vec3(-0.001f, 0.0f, 0.0f));
    part[2].setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    part[2].setTTL(-0);
    part[3].setGeometry(glm::normalize((camera.getPosition() - part[1].getPosition())), glm::vec3(0.0f, 0.0f, -1.0f));
    part[3].setRenderObject(&sModel);
    part[3].setPosition(glm::vec3(1.0, 1.0f, -1.0f));
    part[3].setVelocity(glm::vec3(-0.001f, 0.0f, 0.0f));
    part[3].setScale(glm::vec3(0.5f, 0.5f, 0.5f));
    part[3].setTTL(-0);
    physicsManager.addPhysObj(&part[2]);
    physicsManager.addPhysObj(&part[3]);

    edge1.addChild(&part[2]);
    edge1.addChild(&part[3]);
    edge1.setSpring(2.0, 0.001, 0.0005);
    physicsManager.addPhysObj(&edge1);
*/

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

    //create plane vao
    initPlane(vertices, colors, indicies);
    initMatricies(width, height);



    //add gravity and wind resistance
//    geometry george;
//    george.radius = 0.00001f;
//    george.normal = glm::vec3(10.0f, 30.0f, 0.0f);
//    physicsManager.addDirectionalForce(glm::vec3(0.0f, -0.00001f, 0.0f));
//    physicsManager.addAttractorForce(george);
//    physicsManager.addScalarForce(-0.001);

    //create particle generator
/*
    pGen.setGeometry(1.0f, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
    pGen.setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    pGen.setVelocity(0.02f);
    pGen.setTTL(timeStep * 600);
    physicsManager.addParticleGen(&pGen);
*/

/*
    //set plane normal
    plane.setGeometry(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    //set render object
    plane.setRenderObject(&pModel);
    //set plane position
    plane.setPosition(glm::vec3(0.0f, -5.0f, 0.0f));
    plane.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    //add to the physics manager for collision detection
    physicsManager.addPhysObj((PhysicsObject*)&plane);


    plane1.setGeometry(glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)), glm::vec3(0.0f, 0.0f, -1.0f));
    plane1.setRenderObject(&pModel);
    plane1.setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    plane1.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    physicsManager.addPhysObj((PhysicsObject*)&plane1);


    glutDisplayFunc(testTexture);
    glutKeyboardFunc(KeyHandler);
    glutMouseFunc(MouseHandler);

    plane2.setGeometry(glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0f, -1.0f));
    plane2.setRenderObject(&pModel);
    plane2.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    plane2.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    physicsManager.addPhysObj((PhysicsObject*)&plane2);
*/
    for(int i = 0; i < NUM_PARTS; i++)
    {
        part[i].setGeometry(glm::normalize((camera.getPosition() - part[0].getPosition())), glm::vec3(0.0f, 0.0f, -1.0f));
        part[i].setRenderObject(&sModel);
        part[i].setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        part[i].setScale(glm::vec3(0.5f, 0.5f, 0.5f));
        part[i].setTTL(-0);
        physicsManager.addPhysObj(&part[i]);
    }
    part[0].setPosition(glm::vec3(-1.0f, 1.0f, 1.0f));
    part[1].setPosition(glm::vec3(-1.0f, 1.0f, -1.0f));
    part[2].setPosition(glm::vec3(1.0f, 1.0f, -1.0f));
    part[3].setPosition(glm::vec3(1.0f, 1.0f, 1.0f));
    part[4].setPosition(glm::vec3(-1.0f, -1.0f, 1.0f));
    part[5].setPosition(glm::vec3(-1.0f, -1.0f, -1.0f));
    part[6].setPosition(glm::vec3(1.0f, -1.0f, -1.0f));
    part[7].setPosition(glm::vec3(1.0f, -1.0f, 1.0f));

    setFace(&part[0], &part[1], &part[2], &part[3], 0);
    setFace(&part[4], &part[5], &part[6], &part[7], 5);
    edge[10].addChild(&part[0]);
    edge[10].addChild(&part[4]);
    edge[10].setSpring(2.0, 0.001, 0.0005);
    edge[11].addChild(&part[1]);
    edge[11].addChild(&part[7]);
    edge[11].setSpring(2.0, 0.001, 0.0005);
    edge[12].addChild(&part[3]);
    edge[12].addChild(&part[7]);
    edge[12].setSpring(2.0, 0.001, 0.0005);
    edge[13].addChild(&part[3]);
    edge[13].addChild(&part[6]);
    edge[13].setSpring(2.0, 0.001, 0.0005);
    edge[14].addChild(&part[2]);
    edge[14].addChild(&part[6]);
    edge[14].setSpring(2.0, 0.001, 0.0005);
    edge[15].addChild(&part[2]);
    edge[15].addChild(&part[5]);
    edge[15].setSpring(2.0, 0.001, 0.0005);
    edge[16].addChild(&part[1]);
    edge[16].addChild(&part[5]);
    edge[16].setSpring(2.0, 0.001, 0.0005);
    edge[17].addChild(&part[1]);
    edge[17].addChild(&part[4]);
    edge[17].setSpring(2.0, 0.001, 0.0005);
    for(int i = 0; i < 18; i++) physicsManager.addPhysObj(&edge[i]);
    /*
    setFace1(&part[0], &part[1], &part[2], &part[3], 0);
    setFace1(&part[0], &part[3], &part[7], &part[4], 1);
    setFace1(&part[3], &part[2], &part[6], &part[7], 2);
    setFace1(&part[2], &part[1], &part[5], &part[6], 3);
    setFace1(&part[1], &part[0], &part[4], &part[5], 4);
    setFace1(&part[4], &part[5], &part[6], &part[7], 5);
    for(int i = 0; i < 6; i++) physicsManager.addPhysObj((PhysicsObject*)&plane[i]);
/*
    plane4.setGeometry(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    plane4.setRenderObject(&pModel);
    plane4.setPosition(glm::vec3(-5.0f, 0.0f, 0.0f));
    plane4.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    physicsManager.addPhysObj((PhysicsObject*)&plane4);

    plane5.setGeometry(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    plane5.setRenderObject(&pModel);
    plane5.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
    plane5.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    physicsManager.addPhysObj((PhysicsObject*)&plane5);
*/
    glutDisplayFunc(testRender);
    glutKeyboardFunc(KeyHandler);
    glutKeyboardUpFunc(KeyUpHandler);

    glutTimerFunc(0, Timer, 0);

    cur_time = getTickCount();
    prev_time = cur_time;
    glutMainLoop();

    return 0;
}

void setFace(ParticleObject* o, ParticleObject* t, ParticleObject* th, ParticleObject* f, int idx)
{
    edge[idx].addChild(o);
    edge[idx].addChild(t);
    edge[idx].setSpring(2.0, 0.001, 0.0005);
    edge[idx + 1].addChild(t);
    edge[idx + 1].addChild(th);
    edge[idx + 1].setSpring(2.0, 0.001, 0.0005);
    edge[idx + 2].addChild(th);
    edge[idx + 2].addChild(f);
    edge[idx + 2].setSpring(2.0, 0.001, 0.0005);
    edge[idx + 3].addChild(f);
    edge[idx + 3].addChild(o);
    edge[idx + 3].setSpring(2.0, 0.001, 0.0005);
    edge[idx + 4].addChild(o);
    edge[idx + 4].addChild(th);
    edge[idx + 4].setSpring(2.0, 0.001, 0.0005);
}

void setFace1(ParticleObject* o, ParticleObject* t, ParticleObject* th, ParticleObject* f, int idx)
{
    plane[idx].addChild(o);
    plane[idx].addChild(t);
    plane[idx].addChild(th);
    plane[idx].addChild(f);
    plane[idx].setGeometry(glm::normalize(glm::normalize(glm::cross(o->getPosition() - t->getPosition(),o->getPosition() - th->getPosition()))), glm::vec3(0.0f, 0.0f, -1.0f));
    plane[idx].setRenderObject(&pModel);
    plane[idx].setScale(glm::vec3(1.0f, 1.0f, 1.0f));
}

image* flatImageRWStuff(int argc, char** argv)
{
    //int count = stoi(argv[2], NULL, 10);
    Screen.initScreen(800, 800);
    Screen.clearScreen();

    //int holdImage = imageManager.openPNG(argv[1]);
    //if(holdImage < 0) fprintf(stderr,"Error, couldn't read PPM file.\n");
    //image* img = imageManager.getImgPtr(holdImage);
    //int count = stoi(argv[2], NULL, 10);
    image* img = Screen.getPtr();

    return img;
}


void initShade()
{
    vShade = shaderManager.loadVertexShader("../PBMEngine/vertshade");
    fShade = shaderManager.loadFragmentShader("../PBMEngine/FragShade");
    cShade = shaderManager.combineShaders(vShade, fShade);
    shaderManager.set3dShaderProgram(cShade);
}

void initMatricies(int width, int height)
{
    //Proj = glm::mat4(1.0f);
    Proj = glm::perspective(glm::radians(YFOV), ((float)width / (float)height), ZNEAR, ZFAR);
    camera.setPosition(glm::vec3(0.0f, 0.0f, -10.f));
    camera.setRotation(glm::vec3(0.0f, 0, 0.0f));
    camera.updateViewMatrix();
    //View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.9f));
    //View = View * glm::rotate(glm::mat4(1.0f), 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    //View = glm::lookAt(glm::vec3(0,0,-0.01), glm::vec3(0,0,0), glm::vec3(0,1,0));
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
    //cur_time = getTickCount();
}

void update(float tStep)
{
    handleKeyStates(tStep);
    physicsManager.runTimeStep(tStep);
}

void handleKeyStates(float ts)
{
    if(kState & FORWARD)
    {
        camera.addVelocity(glm::vec3(0.0f, 0.0f, 0.01f));
    }
    if(kState & SLEFT) camera.addVelocity(glm::vec3(0.01f, 0.0f, 0.0f));
    if(kState & BACK) camera.addVelocity(glm::vec3(0.0f, 0.0f, -0.01f));
    if(kState & SRIGHT) camera.addVelocity(glm::vec3(-0.01f, 0.0f, 0.0f));
    if(kState & LUP)
    {
        camera.addNewRotation(glm::vec3(0.0f, -0.01f, 0.0f));
    }
    if(kState & LLEFT) camera.addNewRotation(glm::vec3(-0.01f, 0.0f, 0.0f));
    if(kState & LDOWN) camera.addNewRotation(glm::vec3(0.0f, 0.01f, 0.0f));
    if(kState & LRIGHT) camera.addNewRotation(glm::vec3(0.01f, 0.0f, 0.0f));

    camera.getNextState(ts);
    camera.updateState();
    camera.setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.updateViewMatrix();
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

    glBindVertexArray(vao2);
    for(int i = 0; i < NUM_PARTS; i++)
    {
//        part[i].setGeometry(glm::normalize((camera.getPos() - part[i].getPos())));
        part[i].updateRenderObject();
        Model = *(part[i].getRenderObj()->getMatrix());
        modelViewProj = Proj * View * Model;

        RenderObject* puts = part[i].getRenderObj();


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        shaderManager.configure3DShaders(cShade, puts);


        shader = shaderManager.getCombinedShader(cShade);
        mvpID = glGetUniformLocation(shader, "MVPMat");
        glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

        hold = part[i].getRenderObj()->getData();
        glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
    }

    //draw sphere1
   /* sphere1.updateRenderObject();
    Model = *(sphere1.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = sphere1.getRenderObj();


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);


    shader = shaderManager.getCombinedShader(cShade);
    mvpID = glGetUniformLocation(shader, "MVPMat");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = sphere1.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
*/
    //draw box faces
    glBindVertexArray(vao1);
/*    plane.updateRenderObject();
    Model = *(plane.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = plane.getRenderObj();


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
>>>>>>> digitalImg

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

<<<<<<< HEAD
=======
    //draw plane2
    plane2.updateRenderObject();
    Model = *(plane2.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = plane2.getRenderObj();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane2.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
*/
    //draw plane3
/*    plane3.updateRenderObject();
    Model = *(plane3.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane3.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
*//*
    for(int i = 0; i < 6; i++)
    {
        plane[i].updateRenderObject();
        Model = *(plane[i].getRenderObj()->getMatrix());
        modelViewProj = Proj * View * Model;

        glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

        hold = plane[i].getRenderObj()->getData();
        glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
    }
/*
    //draw plane4
    plane4.updateRenderObject();
    Model = *(plane4.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = plane4.getRenderObj();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane4.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);

    //draw plane5
    plane5.updateRenderObject();
    Model = *(plane5.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = plane5.getRenderObj();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane5.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
*/
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
    case 'w':
        kState = kState | FORWARD;

        break;
    case 'a':
        kState = kState | SLEFT;

        break;
    case 's':
        kState = kState | BACK;

        break;
    case 'd':
        kState = kState | SRIGHT;

        break;
    case 'i':
        kState = kState | LUP;
        break;
    case 'j':
        kState = kState | LLEFT;
        break;
    case 'k':
        kState = kState | LDOWN;
        break;
    case 'l':
        kState = kState | LRIGHT;
        break;
    case 'r':
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

