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
#include "objectcreator.h"
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
#include "objcollection.h"
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
void scene1Init();
void scene2Init();
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
ObjectCreator objectCreator;
Imagemanip Screen;

SphereObject sphere, sphere1;
EdgeObject edge[100];
ParticleObject part[100];
PolygonObject plane1, plane2, plane3, plane4, plane5;
PolygonObject plane[100];
ObjCollection collection[100];
ObjCollection cube1, cube2, tri;
RenderObject sModel, pModel, poModel;

Camera camera;
unsigned int kState = 0;

unsigned long prev_time, cur_time, delta_time, delta_time2;
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

const float timeStep = 1000 / (60.0f * 40.0f);

int rotation = 15;


const char* vertShade;

const char* fragShade;


int main(int argc, char *argv[])
{
    int width = 1920;
    int height = 1080;



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

    sphere.addToManager((void*)&physicsManager);
    sphere.setID(SPHERE);
    sphere.setGeometry(0.5f);
    sphere.setRenderObject(&sModel);
    sphere.setVelocity(glm::vec3(-0.00f, 0.0f, 0.0f));
    sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
    sphere.setPosition(glm::vec3(-100.0f, 1.0f, 1.0f));


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

    physicsManager.setParticleModel(&sModel);
    objectCreator.setPhysManager(&physicsManager);
    objectCreator.setColArray(collection, 100);
    objectCreator.setPolyArray(plane, 100);
    objectCreator.setEdgeArray(edge, 100);
    objectCreator.setVertArray(part, 100);
    objectCreator.setDefaultVModel(&sModel);
    objectCreator.setDefaultPModel(&pModel);
    objectCreator.setSpringAttribs(4.0f, 0.001f, 0.00005f);




/*
    int c1 = objectCreator.createCube(glm::vec3(-1.0f, 0.0f, 0.0f), 0.1f);
    int c2 = objectCreator.createSTriangle(glm::vec3(-10.0f, -1.0f, 0.0f), glm::vec3(4.0f, -6.0f, 6.0f), glm::vec3(4.0f, -6.0f, -6.0f));
    int c3 = objectCreator.createSTriangle(glm::vec3(15.0f, -6.0f, 0.0f), glm::vec3(-6.0f, -16.0f, 10.0f), glm::vec3(-6.0f, -16.0f, -10.0f));
    int c4 = objectCreator.createSTriangle(glm::vec3(8.0f, 6.0f, -6.0f), glm::vec3(8.0f, 6.0f, 6.0f), glm::vec3(1.0f, 10.0f, 0.0f));
    collection[c1].setVertVelocities(glm::vec3(0.001f, 0.0f, 0.0f));
*/
/*    geometry george;
    george.radius = 0.0001f;
    george.normal = glm::vec3(10.0f, 30.0f, 0.0f);
//    physicsManager.addDirectionalForce(glm::vec3(0.0f, -0.0001f, 0.0f));
    physicsManager.addAttractorForce(george);

    int genP = physicsManager.addParticleGen();
    ParticleGenerator* pGen = physicsManager.getGenerator(genP);
    pGen->setGeometry(1.0f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
    pGen->setPosition(glm::vec3(0.0f, -1.0f, 0.0f));
    pGen->setVelocity(0.2f);
    pGen->setTTL(timeStep * 100000);
    physicsManager.addParticles(100);
    objectCreator.setSpringAttribs(25.0f, 0.001f, 0.005f);
    int c1 = objectCreator.createTriangle(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(25.0f, 25.0f, 25.0f), glm::vec3(25.0f, 25.0f, -25.0f));
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

void scene1Init()
{
    //add gravity and wind resistance
    geometry george;
    george.radius = 0.00001f;
    george.normal = glm::vec3(10.0f, 30.0f, 0.0f);
    physicsManager.addDirectionalForce(glm::vec3(0.0f, -0.00001f, 0.0f));
//    physicsManager.addAttractorForce(george);
//    physicsManager.addScalarForce(-0.001);
    int c1 = objectCreator.createCube(glm::vec3(-1.0f, 0.0f, 0.0f), 0.1f);
    int c2 = objectCreator.createSTriangle(glm::vec3(-10.0f, -1.0f, 0.0f), glm::vec3(4.0f, -6.0f, 6.0f), glm::vec3(4.0f, -6.0f, -6.0f));
    int c3 = objectCreator.createSTriangle(glm::vec3(15.0f, -6.0f, 0.0f), glm::vec3(-6.0f, -16.0f, 10.0f), glm::vec3(-6.0f, -16.0f, -10.0f));
    int c4 = objectCreator.createSTriangle(glm::vec3(8.0f, 6.0f, -6.0f), glm::vec3(8.0f, 6.0f, 6.0f), glm::vec3(1.0f, 10.0f, 0.0f));
    collection[c1].setVertVelocities(glm::vec3(0.001f, 0.0f, 0.0f));
}

void scene2Init()
{
    int c1 = objectCreator.createCube(glm::vec3(-4.0f, 0.0f, 0.0f), 1.0f);
    int c2 = objectCreator.createTriangle(glm::vec3(2.0f, 3.0f, 0.0f), glm::vec3(2.0f, -3.0f, 0.0f), glm::vec3(8.0f, 0.0f, 0.0f));
    //int c3 = objectCreator.createTriangle(glm::vec3(-3.0f, 3.0f, 0.0f), glm::vec3(-3.0f, -3.0f, 0.0f), glm::vec3(-9.0f, 0.0f, 0.0f));
    collection[c1].setVertVelocities(glm::vec3(0.01f, 0.0f, 0.0f));
    collection[c2].setVertVelocities(glm::vec3(-0.01f, 0.0f, 0.0f));
    //collection[c3].setVertVelocities(glm::vec3(0.01f, 0.0f, 0.0f));
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
    camera.updateViewMatrix(0.0f);
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
    delta_time = cur_time - prev_time;
    /*if(delta_time2 > delta_time)
    {
        delta_time2 = timeStep;
        delta_time = timeStep;
    }*/
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
    delta_time2 = delta_time;
    //cur_time = getTickCount();
}

void update(float tStep)
{
    handleKeyStates(tStep);
    //physicsManager.runTimeStep(tStep);
    physicsManager.runRK4TimeStep(tStep);
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
        camera.addRotation(glm::vec3(0.0f, -0.001f, 0.0f));
    }
    if(kState & LLEFT) camera.addRotation(glm::vec3(-0.001f, 0.0f, 0.0f));
    if(kState & LDOWN) camera.addRotation(glm::vec3(0.0f, 0.001f, 0.0f));
    if(kState & LRIGHT) camera.addRotation(glm::vec3(0.001f, 0.0f, 0.0f));

    camera.updateViewMatrix(ts);
    camera.setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
}

void display()
{
    //draw sphere
    RenderObject* puts;
    int shader;
    GLint mvpID;
    model* hold;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
/*    sphere.updateRenderObject();
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
*/

    glBindVertexArray(vao1);

    glBindVertexArray(vao2);
    for(int i = 0; i < objectCreator.getNumVerts(); i++)
    {
//        part[i].setGeometry(glm::normalize((camera.getPos() - part[i].getPos())));
        part[i].updateRenderObject();
        Model = *(part[i].getRenderObj()->getMatrix());
        modelViewProj = Proj * View * Model;

        puts = part[i].getRenderObj();


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        shaderManager.configure3DShaders(cShade, puts);


        shader = shaderManager.getCombinedShader(cShade);
        mvpID = glGetUniformLocation(shader, "MVPMat");
        glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

        hold = part[i].getRenderObj()->getData();
        glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);
    }
    ParticleObject* ptr;
    int len = physicsManager.getParticleList(ptr);
    for(int i = 0; i < len; i++)
    {
//        part[i].setGeometry(glm::normalize((camera.getPos() - part[i].getPos())));
        ptr[i].updateRenderObject();
        Model = *(ptr[i].getRenderObj()->getMatrix());
        modelViewProj = Proj * View * Model;

        puts = ptr[i].getRenderObj();


        glBindBuffer(GL_ARRAY_BUFFER, 0);
        shaderManager.configure3DShaders(cShade, puts);


        shader = shaderManager.getCombinedShader(cShade);
        mvpID = glGetUniformLocation(shader, "MVPMat");
        glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

        hold = ptr[i].getRenderObj()->getData();
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

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(msetSpringodelViewProj));

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
        break;
    case 'c':
        physicsManager.clearAllObjects();
        objectCreator.clearAllObjects();
        break;
    case 'x':
        scene2Init();
        break;
    case 'z':
        scene1Init();
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

