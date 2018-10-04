#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

//openGL stuff
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//glm includes
#include <../glm-0.9.9.1/glm/glm.hpp>
#include <../glm-0.9.9.1/glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <../glm-0.9.9.1/glm/gtx/rotate_vector.hpp>

//image buffer stuff
#include "buffer2d.h"
#include "ppmrw.h"

//miscellaneous structs and enums and stuff.
#include "structpile.h"

//Dr. Tessendorf's weird command line reading thing
#include "CmdLineFind.h"

//the actual physics engine.
#include "fluidmodel.h"

//SPH stuff
#include "particle.h"
#include "sphmodel.h"
#include "stuffbuilder.h"

//Volume Rendering Stuff
#include "camera.h"
#include "field.h"
#include "scalarfields.h"
#include "colorfields.h"
#include "grid.h"
#include "volumelight.h"
#include "volumerenderer.h"
#include "volumerw.h"
#include "lsgenerator.h"

//headers
void printControls();
void keyHandler(GLFWwindow* win, int key, int scancode, int action, int mods);

//static variables.
static float WIDTH, HEIGHT;

//globals (because I am lazy)
Buffer2D loadedImg;
Buffer2D displayBuf;
Buffer2D sourceInBuf;
GLuint uvbuffer, textureID;
FluidModel fluidModel;
SPHModel sphModel;
StuffBuilder stuffbuilder;
volumerenderer volRenderer;
VolumeRW volRW;
LSGenerator lsGen;
camera cam;

GLfloat* g_vertex_buffer_data;
GLfloat* g_uv_buffer_data;

#define BRUSH_SIZE 11
float obstruction_brush[BRUSH_SIZE][BRUSH_SIZE];
float source_brush[BRUSH_SIZE][BRUSH_SIZE];
int paint_mode;

int prog_state = 0;
int capturedFrames = 0;
int display_state = IMAGE;
float brightness;
double prev_time, cur_time, delta_time, d_delta_time, d_prev_time, d_cur_time;
double timeStep = 1.0 / (60.0 * 1.0);
double simTime = 0.0f;
double displayTime = 1.0f / (60.0f);
double holdXPos, holdYPos;

int main(int argc, char* argv[])
{
    //set up Dr. Tessendorf's command line reader stuff
    lux::CmdLineFind clf(argc, argv);
    printControls();

    int noImage = clf.find("-blank", 0, "Set to one if you want a blank image (must set width and height)");
    int volRen = clf.find("-volRen", 1, "Set to one if you're using the volume renderer.");
    int readLights = clf.find("-rLights", 0, "Set to one if you wish to read the light grids from memory.");
    int writeLights = clf.find("-wLights", 0, "Set to one if you wish to write the light grids to memory.");
    int readObjects = clf.find("-rObj", 0, "Set to one if you wish to write the object grids to memory.");
    int writeObjects = clf.find("-wObj", 0, "Set to one if you wish to read the object grids from memory.");
    int sceneSwitch = clf.find("-scene", 0, "# determines which scene to load");
    int width = clf.find("-width", 1920, "Width of simulation if no image supplied");
    int height = clf.find("-height", 1080, "Height of simulation if no image supplied.");
    brightness = clf.find("-b", 1.0f, "The initial display brightness.");
    std::string imgName = clf.find("-image", "../black.png", "File name for base image.");

    timeStep = clf.find("-ts", (1.0f / (60.0f)), "Starting timestep size.");

    //init display buffer.
    displayBuf.init(width, height, 4, 1.0f);
    

    //set static variables.
    WIDTH = displayBuf.getWidth();
    HEIGHT = displayBuf.getHeight();

    //-------------------------------------------BUILD SCENE------------------------------------------//
    cam.setFOV(55);
    cam.setPos(glm::vec3(0.0f, 0.0f, -50.0f));
    cam.setLookDir(glm::vec3(0.0f, 0.0f, 1.0f));
    cam.setUpDir(glm::vec3(0.0f, 1.0f, 0.0f));
    cam.setRenderDistances(1.0, 70.0);


    //ScalarSphere s2 = ScalarSphere(glm::vec3(0.0f, 3.0f, 0.0f), 5.0f);
    //ScalarCutout cut = ScalarCutout(&s1, &s2);
    //ScalarTorus t = ScalarTorus(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, 3.0f);
    //ScalarSteinerPatch steiner = ScalarSteinerPatch(glm::vec3(0.0f));
    char oname[100];
    //--------------------MAKE CYLINDER------------------------//
/*    ScalarInfCylinder cyl = ScalarInfCylinder(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1);
    ScalarPlane p1 = ScalarPlane(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ScalarPlane p2 = ScalarPlane(glm::vec3(0.0f, -4.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    ScalarIntersect u1 = ScalarIntersect(&cyl, &p1);
    ScalarIntersect u2 = ScalarIntersect(&u1, &p2);
    //--------------------COPY CYLINDER 1-----------------------//
    ScalarXYZRotate r1 = ScalarXYZRotate(&u2, 0, 0, -90.0);
    ScalarTranslate t1 = ScalarTranslate(&r1, glm::vec3(-5.0f, 0.0f, 0.0f));
    //--------------------COPY CYLINDER 2-----------------------//
    ScalarXYZRotate r2 = ScalarXYZRotate(&u2, 0, 0, -90.0);
    ScalarTranslate t2 = ScalarTranslate(&r2, glm::vec3(5.0f, 0.0f, 0.0f));

    //---------------------CREATE ARM 1-------------------------//
    ScalarUnion au1 = ScalarUnion(&t1, &t2);
    ScalarTranslate arm1 = ScalarTranslate(&au1, glm::vec3(-18.0f, -5.0f, 0.0f));
    //---------------------CREATE ARM 2-------------------------//
    ScalarTranslate arm2 = ScalarTranslate(&arm1, glm::vec3(36.0f, 0.0f, 0.0f));
    //---------------------CREATE LEG 1-------------------------//
    ScalarXYZRotate lr1 = ScalarXYZRotate(&au1, 0, 0, 90);
    ScalarTranslate leg1 = ScalarTranslate(&lr1, glm::vec3(-5.0f, 20.0f, 0.0f));
    //---------------------CREATE LEG 2-------------------------//
    ScalarTranslate leg2 = ScalarTranslate(&leg1, glm::vec3(10.0f, 0.0f, 0.0f));
    //---------------------CREATE BODY-------------------------//
    ScalarEllipse body = ScalarEllipse(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 10.0f, 5.0f);
    //---------------------CREATE CHEST-------------------------//
    ScalarCone chest = ScalarCone(glm::vec3(0.0f, -18.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 20.0f, 0.2f);
    //---------------------CREATE HEAD-------------------------//
    ScalarSphere head = ScalarSphere(glm::vec3(0.0f, -20.0f, 0.0f), 5.0);
    //---------------------CREATE FOOT 1-------------------------//
    ScalarCube foot1 = ScalarCube(glm::vec3(-5, 30.0f, 0.0f), 2.2, 6);
    //---------------------CREATE FOOT 2-------------------------//
    ScalarCube foot2 = ScalarCube(glm::vec3(5, 30.0f, 0.0f), 2.2, 6);
    //---------------------CREATE ICO-------------------------//
    ScalarIcosahedron ico = ScalarIcosahedron(glm::vec3(-18.0f, -5.0f, 0.0f), 2);
    ScalarScale ico1 = ScalarScale(&ico, 2);
    //---------------------CREATE STEINER-------------------------//
    ScalarSteinerPatch steiner = ScalarSteinerPatch(glm::vec3(18.0f, -5.0f, 0.0f));
    ScalarScale steiner1 = ScalarScale(&steiner, 10);
    //--------------------UNION EVERYTHING----------------------//
    ScalarUnion add = ScalarUnion(&arm1, &arm2);
    ScalarUnion add1 = ScalarUnion(&leg1, &leg2);
    ScalarUnion add2 = ScalarUnion(&add, &add1);
    ScalarUnion add3 = ScalarUnion(&add2, &body);
    ScalarUnion add4 = ScalarUnion(&add3, &chest);
    ScalarUnion add5 = ScalarUnion(&add4, &head);
    ScalarUnion add6 = ScalarUnion(&foot1, &foot2);
    ScalarUnion add7 = ScalarUnion(&add5, &add6);
    ScalarUnion add8 = ScalarUnion(&add7, &ico1);
    ScalarUnion add9 = ScalarUnion(&add8, &steiner1);
    ScalarScale scale = ScalarScale(&add9, 1.0);


    ScalarRotate rot = ScalarRotate(&scale, glm::vec3(0.0f, 0.0f , 0.0f));
    Grid<float>* scene;
    if(readObjects == 1)
    {
        printf("--------------------reading FULL SCENE--------------\n");
        sprintf(oname, "../grids/scene.grid");
        scene = volRW.readScalarGrid(oname);
    }
    else
    {
        scene = new Grid<float>(glm::vec3(-35.0f, -30.0f, -30.0f), glm::vec3(35.0f, 30.0f, 30.0f), 400, 400, 400);
        scene->stampField(&rot);
    }
    if(writeObjects == 1 && readObjects == 0)
    {
        printf("--------------------writing BUNNY--------------\n");
        sprintf(oname, "../grids/scene.grid");
        volRW.writeScalarGrid(scene, oname);
    }
    ScalarGrid scene1 = ScalarGrid(scene, -1);
    ScalarClamp m = ScalarClamp(&scene1, 0.0f, 1.0f);
*/
    //--------------------LOAD BUNNY----------------------//
    Grid<float>* bun;
    if(readObjects == 1)
    {
        printf("--------------------reading BUNNY--------------\n");
        sprintf(oname, "../grids/bun.grid");
        bun = volRW.readScalarGrid(oname);
    }
    else
    {
        lsGen.readObj("../models/cleanbunny.obj");
        bun = new Grid<float>(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), 50, 50, 50);
        lsGen.stampLS(*bun);
    }
    if(writeObjects == 1 && readObjects == 0)
    {
        printf("--------------------writing BUNNY--------------\n");
        sprintf(oname, "../grids/bun.grid");
        volRW.writeScalarGrid(bun, oname);
    }
    //ScalarSphere test = ScalarSphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0);
    //bun->stampField(&test);
    ScalarGrid bunGrid = ScalarGrid(bun, -1.0f);
    ScalarScale enhance = ScalarScale(&bunGrid, -15);
    ScalarClamp bunClamp = ScalarClamp(&enhance, 0.0f, 1.0f);

    //--------------------LOAD TEAPOT----------------------//
    Grid<float>* tea;
    if(readObjects == 1)
    {
        printf("--------------------reading BUNNY--------------\n");
        sprintf(oname, "../grids/tea.grid");
        tea = volRW.readScalarGrid(oname);
    }
    else
    {
        lsGen.readObj("../models/cleanteapot1.obj");
        tea = new Grid<float>(glm::vec3(-1.5f, -1.0f, -1.0f), glm::vec3(1.5f, 1.0f, 1.0f), 50, 50, 50);
        lsGen.stampLS(*tea);
    }
    if(writeObjects == 1 && readObjects == 0)
    {
        printf("--------------------writing TEAPOT--------------\n");
        sprintf(oname, "../grids/tea.grid");
        volRW.writeScalarGrid(tea, oname);
    }
    //ScalarSphere test = ScalarSphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0);
    //bun->stampField(&test);
    ScalarGrid teaGrid = ScalarGrid(tea, -1.0f);
    ScalarScale teahance = ScalarScale(&teaGrid, -15);
    ScalarClamp teaClamp = ScalarClamp(&teahance, 0.0f, 1.0f);

    //---------------------CREATE BUNNY BOWL---------------------//
    ScalarCutout hat = ScalarCutout(&teaGrid, new ScalarSphere(glm::vec3(0.0f, 1.0f, 0.0f), 1.0));
    ScalarUnion bbowl = ScalarUnion(&hat, new ScalarTranslate(&bunGrid, glm::vec3(0.0f, 0.5f, 0.0f)));
    ScalarScale bigBowl = ScalarScale(&bbowl, -15);
    ScalarClamp cBowl = ScalarClamp(&bigBowl, 0, 1);
    /*
    //---------------------CREATE COLOR FIELD---------------------//
    ColorField c1 = ColorField(color(1.0f, 1.0f, 1.0f, 1.0f));
    ColorField c2 = ColorField(color(0.0f, 1.0f, 0.0f, 1.0f));
    ColorField c3 = ColorField(color(1.0f, 0.0f, 0.0f, 1.0f));
    ColorField c4 = ColorField(color(1.0f, 0.2f, 0.2f, 1.0f));
    ScalarFieldInverse i = ScalarFieldInverse(&body);
    ScalarFieldInverse i1 = ScalarFieldInverse(&ico);
    ScalarFieldInverse i2 = ScalarFieldInverse(&ico1);
    ScalarIntersect is = ScalarIntersect(&i, &i1);
    ScalarIntersect is2 = ScalarIntersect(&is, &i2);
    ColorScalarClamp cs4 = ColorScalarClamp(&c1, &is2, 0, 1);

    ColorScalarClamp cs1 = ColorScalarClamp(&c2, &body, 0, 1);
    ColorScalarClamp cs3 = ColorScalarClamp(&c3, &ico, 0, 1);
    ColorScalarClamp cs5 = ColorScalarClamp(&c4, &ico1, 0, 1);

    //---------------------CREATE COLOR FIELD---------------------//
    ColorFieldAdd cAdd = ColorFieldAdd(&cs1, &cs3);
    ColorFieldAdd cAdd1 = ColorFieldAdd(&cs4, &cAdd);
    ColorFieldAdd cAdd2 = ColorFieldAdd(&cAdd1, &cs5);
    */
    ColorField white = ColorField(color(1.0f, 1.0f, 1.0f, 1.0f));

    //-------------------------------------------SET UP BBOX------------------------------------------//
    bbox bounds;
    bounds.LLC = glm::vec3(-35.0f, -30.0f, -30.0f);
    bounds.URC = glm::vec3(35.0f, 30.0f, 30.0f);

    //-------------------------------------------SET UP LIGHTS------------------------------------------//
    VolumeLight lights[4];
    lights[0].setPos(glm::vec3(35.0f, -30.0f, -20.0f));
    lights[0].setColor(color(0.0f, 1.0f, 1.0f, 1.0f));
    lights[1].setPos(glm::vec3(-30.0f, 30.0f, -10.0f));
    lights[1].setColor(color(1.0f, 0.0f, 0.0f, 1.0f));
    lights[2].setPos(glm::vec3(-10.0f, 10.0f, 40.0f));
    lights[2].setColor(color(0.0f, 1.0f, 1.0f, 1.0f));

    //-------------------------------------------SET UP VOLUME RENDERER------------------------------------------//
    volRenderer.setLights(lights, 3);
    volRenderer.setBoundingBox(&bounds);
    volRenderer.setCamera(&cam);
    volRenderer.setDisplayBuf(&displayBuf);
    volRenderer.setTCoeff(3);
    volRenderer.setColorFields(&white, 1);
    volRenderer.setScalarFields(&teaClamp, 1);
    volRenderer.setMarchSize(0.1);

    //-------------------------------------------CALCULATE DSMs------------------------------------------//
    Grid<float>* dsmb[3];
    Grid<float>* dsmt[3];
    Grid<float>* dsmh[3];
    Grid<float>* dsms[3];
    ScalarGrid* blgrid[3];
    ScalarGrid* tlgrid[3];
    ScalarGrid* hlgrid[3];
    ScalarGrid* slgrid[3];
    char gname[100];
    if(readLights == 0)
    {
        printf("--------------------starting DSM calc--------------\n");
        for(int i = 0; i < 3; i++)
        {
            dsmb[i] = new Grid<float>(bounds.LLC, bounds.URC, 400, 400, 400);
            volRenderer.calcDSM(*dsmb[i], lights[i].getPos());
        }
        volRenderer.setScalarFields(&teahance, 1);
        for(int i = 0; i < 3; i++)
        {
            dsmt[i] = new Grid<float>(bounds.LLC, bounds.URC, 400, 400, 400);
            volRenderer.calcDSM(*dsmt[i], lights[i].getPos());
        }
        volRenderer.setScalarFields(&cBowl, 1);
        for(int i = 0; i < 3; i++)
        {
            dsmh[i] = new Grid<float>(bounds.LLC, bounds.URC, 400, 400, 400);
            volRenderer.calcDSM(*dsmh[i], lights[i].getPos());
        }
 /*       volRenderer.setScalarFields(&m, 1);
        for(int i = 0; i < 3; i++)
        {
            dsms[i] = new Grid<float>(bounds.LLC, bounds.URC, 400, 400, 400);
            volRenderer.calcDSM(*dsms[i], lights[i].getPos());
        }*/
    }
    else
    {
        printf("--------------------starting DSM read--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/blgrid%d.grid", i);
            dsmb[i] = volRW.readScalarGrid(gname);
        }
        printf("--------------------starting DSM read--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/tlgrid%d.grid", i);
            dsmt[i] = volRW.readScalarGrid(gname);
        }
        printf("--------------------starting DSM read--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/hlgrid%d.grid", i);
            dsmh[i] = volRW.readScalarGrid(gname);
        }
        /*printf("--------------------starting DSM read--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/slgrid%d.grid", i);
            dsms[i] = volRW.readScalarGrid(gname);
        }*/
    }
    if(writeLights == 1 && readLights == 0)
    {
        printf("--------------------writing DSM--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/blgrid%d.grid", i);
            volRW.writeScalarGrid(dsmb[i], gname);
        }
        printf("--------------------writing DSM--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/tlgrid%d.grid", i);
            volRW.writeScalarGrid(dsmt[i], gname);
        }
        printf("--------------------writing DSM--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/hlgrid%d.grid", i);
            volRW.writeScalarGrid(dsmh[i], gname);
        }
        /*printf("--------------------writing DSM--------------\n");
        for(int i = 0; i < 3; i++)
        {
            sprintf(gname, "../grids/slgrid%d.grid", i);
            volRW.writeScalarGrid(dsms[i], gname);
        }*/
    }
    for(int i = 0; i < 3; i++)
    {
        blgrid[i] = new ScalarGrid(dsmb[i], 0);
        tlgrid[i] = new ScalarGrid(dsmt[i], 0);
        hlgrid[i] = new ScalarGrid(dsmh[i], 0);
        //slgrid[i] = new ScalarGrid(dsms[i], 0);
    }

    //-------------------------------------------RENDER FRAMES------------------------------------------//
    if(volRen == 1)
    {
        printf("--------------------starting Render---------------\n");
        printf("--------------------BUNNY---------------\n");
        char fname[100];
        int numFrames = 120;
        for(int i = 0; i < 3; i++)
        {
            lights[i].setDSM(blgrid[i]);
        }
        volRenderer.setScalarFields(&bunClamp, 1);
        for(int i = 0; i < numFrames; i++)
        {
            glm::vec3 cPos = glm::rotateY(glm::vec3(0.0f, 0.0f, -50.0f), 2*(float)PI * i/numFrames);
            glm::vec3 cLook = glm::rotateY(glm::vec3(0.0f, 0.0f, 1.0f), 2*(float)PI * i/numFrames);
            cam.setPos(cPos);
            cam.setLookDir(cLook);
            volRenderer.renderFrame();
            volRenderer.passToDisplay();
            sprintf(fname, "../turntable/turn%d.exr", i);
            displayBuf.writeImage(fname);
            printf("finished rendering %s\n", fname);
        }
        printf("--------------------starting Render---------------\n");
        printf("--------------------TEAPOT---------------\n");
        for(int i = 0; i < 3; i++)
        {
            lights[i].setDSM(tlgrid[i]);
        }
        volRenderer.setScalarFields(&teaClamp, 1);
        for(int i = 0; i < numFrames; i++)
        {
            glm::vec3 cPos = glm::rotateY(glm::vec3(0.0f, 0.0f, -50.0f), 2*(float)PI * i/numFrames);
            glm::vec3 cLook = glm::rotateY(glm::vec3(0.0f, 0.0f, 1.0f), 2*(float)PI * i/numFrames);
            cam.setPos(cPos);
            cam.setLookDir(cLook);
            volRenderer.renderFrame();
            volRenderer.passToDisplay();
            sprintf(fname, "../turntable1/turn%d.exr", i);
            displayBuf.writeImage(fname);
            printf("finished rendering %s\n", fname);
        }
        printf("--------------------starting Render---------------\n");
        printf("--------------------BUNNY BOWL---------------\n");
        for(int i = 0; i < 3; i++)
        {
            lights[i].setDSM(hlgrid[i]);
        }
        volRenderer.setScalarFields(&cBowl, 1);
        for(int i = 0; i < numFrames; i++)
        {
            glm::vec3 cPos = glm::rotateY(glm::vec3(0.0f, 0.0f, -50.0f), 2*(float)PI * i/numFrames);
            glm::vec3 cLook = glm::rotateY(glm::vec3(0.0f, 0.0f, 1.0f), 2*(float)PI * i/numFrames);
            cam.setPos(cPos);
            cam.setLookDir(cLook);
            volRenderer.renderFrame();
            volRenderer.passToDisplay();
            sprintf(fname, "../turntable2/turn%d.exr", i);
            displayBuf.writeImage(fname);
            printf("finished rendering %s\n", fname);
        }
        /*printf("--------------------starting Render---------------\n");
        printf("--------------------FULL SCENE---------------\n");
        for(int i = 0; i < 3; i++)
        {
            lights[i].setDSM(slgrid[i]);
        }
        volRenderer.setScalarFields(&m, 1);
        for(int i = 0; i < numFrames; i++)
        {
            glm::vec3 cPos = glm::rotateY(glm::vec3(0.0f, 0.0f, -50.0f), 2*(float)PI * i/numFrames);
            glm::vec3 cLook = glm::rotateY(glm::vec3(0.0f, 0.0f, 1.0f), 2*(float)PI * i/numFrames);
            cam.setPos(cPos);
            cam.setLookDir(cLook);
            volRenderer.renderFrame();
            volRenderer.passToDisplay();
            sprintf(fname, "../turntable3/turn%d.exr", i);
            displayBuf.writeImage(fname);
            printf("finished rendering %s\n", fname);
        }*/
    }

}

void printControls()
{
    printf("r       Reset the simulation (buoyancy and timesteps will not reset)\n");
    printf("p       Pause the simulation\n");
    printf("d       Toggle density display\n");
    printf("f       Toggle pressure display\n");
    printf("-, +    Change the buoyancy of the fluid (no shift)\n");
    printf("<, >    Change the timestep size of the simulation (no shift)\n");
    printf("[, ]    Change the display brightness\n");
    printf(";, ',   Change the viscocity coefficient\n");
    printf("9, 0,   Change the vorticity coefficient\n");
}

void keyHandler(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    switch(key)
    {
     case GLFW_KEY_R:
        if(action == GLFW_PRESS)
        {
        }
        break;
    case GLFW_KEY_O:
        if(action == GLFW_PRESS)
        {
        }
        break;
    case GLFW_KEY_I:
        if(action == GLFW_PRESS)
        {
        }
        break;
    case GLFW_KEY_P:
        if(action == GLFW_PRESS)
        {
            prog_state = prog_state ^ RUNNING;
            if(prog_state & RUNNING)
            {
                printf("Prog State: RUNNING\n");
            }
            else
            {
                printf("Prog State: PAUSED\n");
            }
        }
        break;
    case GLFW_KEY_D:
        if(action == GLFW_PRESS)
        {
            if(display_state != DENSITY)
            {
                display_state = DENSITY;
                printf("Display: DENSITY\n");
            }
            else if(display_state == DENSITY)
            {
                display_state = IMAGE;
                printf("Display: IMAGE\n");
            }
        }
        break;
    case GLFW_KEY_F:
        if(action == GLFW_PRESS)
        {
            if(display_state != PRESSURE)
            {
                display_state = PRESSURE;
                printf("Display: PRESSURE\n");
            }
            else if(display_state == PRESSURE)
            {
                display_state = IMAGE;
                printf("Display: IMAGE\n");
            }
        }
        break;
    case GLFW_KEY_G:
        if(action == GLFW_PRESS)
        {
            if(display_state != ERROR)
            {
                display_state = ERROR;
                printf("Display: ERROR\n");
            }
            else if(display_state == ERROR)
            {
                display_state = IMAGE;
                printf("Display: IMAGE\n");
            }
        }
        break;
    case GLFW_KEY_9:
        if(action == GLFW_PRESS)
        {
            float add = 1.0f;
            float vort = fluidModel.getVorticity();
            fluidModel.setVorticity(vort - add);
            printf("Vorticity force: %f\n", vort - add);
        }
    break;
    case GLFW_KEY_0:
        if(action == GLFW_PRESS)
        {
            float add = 1.0f;
            float vort = fluidModel.getVorticity();
            fluidModel.setVorticity(vort + add);
            printf("Vorticity force: %f\n", vort + add);
        }
    break;
    case GLFW_KEY_MINUS:
        if(action == GLFW_PRESS)
        {
            float add = 1.0f;
            float grav = fluidModel.getGravity();
            fluidModel.setGravity(grav - add);
            printf("Buoyancy force: %f\n", grav - add);
        }
        break;
    case GLFW_KEY_EQUAL:
        if(action == GLFW_PRESS)
        {
            float add = 1.0f;
            float grav = fluidModel.getGravity() + add;
            fluidModel.setGravity(grav);
            printf("Buoyancy force: %f\n", grav + add);
        }
        break;
    case GLFW_KEY_LEFT_BRACKET:
        if(action == GLFW_PRESS)
        {
            brightness -= 0.2f;
            printf("Brightness force: %f\n", brightness);
        }
        break;
    case GLFW_KEY_RIGHT_BRACKET:
        if(action == GLFW_PRESS)
        {
            brightness += 0.2f;
            printf("Brightness force: %f\n", brightness);
        }
        break;
    case GLFW_KEY_SEMICOLON:
        if(action == GLFW_PRESS)
        {
            float add = 1.0f;
            float visc = fluidModel.getViscosity() - add;
            fluidModel.setViscosity(visc);
            printf("Viscosity force: %f\n", visc - add);
        }
        break;
    case GLFW_KEY_APOSTROPHE:
        if(action == GLFW_PRESS)
        {
            float add = 1.0f;
            float visc = fluidModel.getViscosity() + add;
            fluidModel.setViscosity(visc);
            printf("Viscosity force: %f\n", visc + add);
        }
        break;
    case GLFW_KEY_COMMA:
        if(action == GLFW_PRESS)
        {
            float add = 1.0 / (60.0 * 10);
            timeStep -= add;
            printf("TimeStep: %f\n", timeStep);
        }
        break;
    case GLFW_KEY_PERIOD:
        if(action == GLFW_PRESS)
        {
            float add = 1.0 / (60.0 * 10);
            timeStep += add;
            printf("TimeStep: %f\n", timeStep);
        }
        break;
    default:
        break;
    }

}
