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

#include "CJRW.h"
#include "shadermanager.h"
#include "modelmanager.h"
#include "imagemanip.h"
#include "structpile.h"
#include "renderobject.h"
#include "physicsobject.h"
#include "physicsmanager.h"
#include "sphereobject.h"
#include "planeobject.h"
#include "camera.h"


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
void handleKeyStates(float);
void display();
void reshape(GLsizei width, GLsizei height);
void Timer(int value);
void KeyHandler(unsigned char key, int x, int y);
void KeyUpHandler(unsigned char key, int x, int y);
unsigned long getTickCount();

ShaderManager shaderManager;
ReaderWriter imageManager;
ModelManager modelManager;
PhysicsManager physicsManager;
Imagemanip Screen;

SphereObject sphere;
SphereObject sphere1;
PolygonObject plane1, plane2, plane3, plane4, plane5;
PolygonObject plane;
RenderObject sModel, pModel;

Camera camera;
unsigned int kState = 0;

unsigned long prev_time, cur_time;
glm::mat4 modelViewProj, Proj, View, Model;
GLuint buf, idx, tex, posAttrib, vao, vao1;
GLfloat angle = 0.0f;
int refreshMills = 30;
int vShade, fShade, cShade;
model* hold1;

const float timeStep = 1000 / (60.0f * 5);

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

    camera.setViewMatrix(&View);
    initMatricies(width, height);

    //add gravity and wind resistance
    physicsManager.addDirectionalForce(glm::vec3(0.0f, -0.00001f, 0.0f));
    physicsManager.addScalarForce(-0.001);


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

    plane2.setGeometry(glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)), glm::vec3(0.0f, 0.0f, -1.0f));
    plane2.setRenderObject(&pModel);
    plane2.setPosition(glm::vec3(0.0f, 0.0f, -5.0f));
    plane2.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    physicsManager.addPhysObj((PhysicsObject*)&plane2);

    plane3.setGeometry(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    plane3.setRenderObject(&pModel);
    plane3.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));
    plane3.setScale(glm::vec3(5.0f, 5.0f, 5.0f));
    physicsManager.addPhysObj((PhysicsObject*)&plane3);

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

    glutDisplayFunc(testRender);
    glutKeyboardFunc(KeyHandler);
    glutKeyboardUpFunc(KeyUpHandler);
    glutTimerFunc(0, Timer, 0);

    cur_time = getTickCount();
    prev_time = cur_time;
    glutMainLoop();

    return 0;
}

image* flatImageRWStuff(int argc, char** argv)
{
    //int holdImage = imageManager.openPNG(argv[1]);
    //if(holdImage < 0) fprintf(stderr,"Error, couldn't read PPM file.\n");
    //image* img = imageManager.getImgPtr(holdImage);
    //int count = stoi(argv[2], NULL, 10);
    Screen.initScreen(500, 500);
    Screen.psychedelic(1);
    image* img = Screen.getPtr();
    //int imgIdx = imageManager.addImage(Screen.getScreen());
    //if(!imageManager.writePPM(argv[2], holdImage)) fprintf(stderr, "Error: Couldn't write ppm file\n");
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
    camera.setPosition(glm::vec3(0.0f, 0.0f, -20.f));
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

void initBuf()
{
    int index = modelManager.readObj("../testObj.obj");
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    sModel.setModel(modelManager.getModel(index));

    sphere.setGeometry(0.5f);
    sphere.setRenderObject(&sModel);
    sphere.setVelocity(glm::vec3(0.02f, 0.0f, 0.0f));
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
    handleKeyStates(tStep);
    physicsManager.runTimeStep(tStep);

    /*
    glm::mat4 herp = glm::rotate(glm::mat4(1.0f), (0.0005f * tStep), glm::vec3(0.0f, 1.0f, 0.0f));
    Model = herp;
    modelViewProj = modelViewProj * Model;
    */
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
        camera.addRotation(glm::vec3(0.0f, -0.01f, 0.0f));
    }
    if(kState & LLEFT) camera.addRotation(glm::vec3(-0.01f, 0.0f, 0.0f));
    if(kState & LDOWN) camera.addRotation(glm::vec3(0.0f, 0.01f, 0.0f));
    if(kState & LRIGHT) camera.addRotation(glm::vec3(0.01f, 0.0f, 0.0f));

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
    plane.updateRenderObject();
    Model = *(plane.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = plane.getRenderObj();


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);

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

    //draw plane3
    plane3.updateRenderObject();
    Model = *(plane3.getRenderObj()->getMatrix());
    modelViewProj = Proj * View * Model;

    puts = plane3.getRenderObj();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    shaderManager.configure3DShaders(cShade, puts);

    glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(modelViewProj));

    hold = plane3.getRenderObj()->getData();
    glDrawElements(GL_TRIANGLES, hold->idxLen, GL_UNSIGNED_INT, 0);

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
