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
#include "volumerenderer.h"

//headers
void printControls();
void keyHandler(GLFWwindow* win, int key, int scancode, int action, int mods);
void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos);
void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
void initializeBrushes();
void dabSomePaint(Buffer2D *scr, int x, int y);
void convertSourceIn(float intensity);
void display(GLFWwindow* window, GLuint matID, GLuint progID, GLuint vertBuf, glm::mat4 &mvp, int verts);
void displaySPH(GLFWwindow* window, GLuint matID, GLuint progID, GLuint vertBuf, glm::mat4 &mvp, int verts);
void update(double ts);

//static variables.
static float WIDTH, HEIGHT;

//globals (because I am lazy)
Buffer2D loadedImg;
Buffer2D displayBuf;
Buffer2D sourceInBuf;
Buffer2D sourceBuf;
GLuint uvbuffer, textureID;
FluidModel fluidModel;
SPHModel sphModel;
StuffBuilder stuffbuilder;
volumerenderer volRenderer;
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
    int sph = clf.find("-SPH", 0, "Set to one if you want to run a Something Particle Hydrodynamics simulation");
    int width = clf.find("-width", 720, "Width of simulation if no image supplied");
    int height = clf.find("-height", 480, "Height of simulation if no image supplied.");
    brightness = clf.find("-b", 1.0f, "The initial display brightness.");
    std::string sourceIn = clf.find("-source", "", "File name for source input");
    std::string imgName = clf.find("-image", "../black.png", "File name for base image.");

    if(sourceIn != "")
    {
        prog_state = prog_state | SOURCEIN;
        sourceInBuf.readImage(sourceIn.c_str());
    }
    if(sph > 0)
    {
        prog_state = prog_state | SPH;
        noImage = 1;
    }
    else prog_state = prog_state & ~SPH;
    timeStep = clf.find("-ts", (1.0f / (60.0f)), "Starting timestep size.");

    //load initial images;
    if(noImage == 0 && !volRen)
    {
        loadedImg.readImage(imgName.c_str());
        displayBuf.readImage(imgName.c_str());
    }
    else if(volRen)
    {
        displayBuf.init(width, height, 4, 1.0f);
    }
    else
    {
        loadedImg.init(width, height, 3, 1.0f);
        displayBuf.init(width, height, 3, 1.0f);
    }
    
    sourceBuf.init(displayBuf.getWidth(), displayBuf.getHeight(), 1, 1.0);

    //init brushes.
    initializeBrushes();

    //set program state;
    prog_state = prog_state | RUNNING;

    //set static variables.
    WIDTH = displayBuf.getWidth();
    HEIGHT = displayBuf.getHeight();

    //initialize glfw
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window;
    window = glfwCreateWindow(WIDTH, HEIGHT, "Fluid Model Display", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, keyHandler);
    glfwSetCursorPosCallback(window, mouseMoveHandler);
    glfwSetMouseButtonCallback(window, mouseButtonHandler);

    //GLEW init stuff
    //Get depth buffer going
    glEnable(GL_DEPTH_TEST);
    //Accept a frament if it's closer to the camera than the previous value in the buffer.
    glDepthFunc(GL_LESS);

    //create a VAO
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    //create vertex buffer
    // An array of 3 vectors which represents 3 vertices
    int numVerts;

    numVerts = 6;
    g_vertex_buffer_data = new GLfloat[numVerts * 3]{
        -1.0f,-1.0f, -1.0f, // triangle 1 : begin
        1.0f,-1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, // triangle 1 : end
        1.0f, 1.0f, -1.0f, // triangle 2 : begin
        1.f,-1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, // triangle 2 : end
    };

    glPointSize(3.0f);

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVerts * 3, g_vertex_buffer_data, GL_STATIC_DRAW);

    // Create one OpenGL texture
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, displayBuf.getWidth(), displayBuf.getHeight(), 0, GL_RGB, GL_FLOAT, NULL);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, displayBuf.getWidth(), displayBuf.getHeight(), GL_RGB, GL_FLOAT, displayBuf.getBuf());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Create uv buffer.
    // One color for each vertex. They were generated randomly.
    g_uv_buffer_data = new GLfloat[numVerts * 2]{
        0.0f,  0.0f,
        1.0f,  0.0f,
        0.0f,  1.0f,
        1.0f,  1.0f,
        1.0f,  0.0f,
        0.0f,  1.0f
    };

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    if(prog_state & SPH) glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVerts * 3, g_uv_buffer_data, GL_STATIC_DRAW);
    else glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVerts * 2, g_uv_buffer_data, GL_STATIC_DRAW);


    //load the shaders.
    GLuint programID;
    if(prog_state & SPH) programID = LoadShaders("../PBMEngine/VertS1.glsl", "../PBMEngine/FragS1.glsl");
    else programID = LoadShaders("../PBMEngine/VertS2.glsl", "../PBMEngine/FragS2.glsl");

    //Compute the ModelViewPerspective matrix.
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 Projection;
    Projection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f,0.1f, 100.0f);

    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(0, 0, 1), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around

    //Pass the matrix to GLSL.
    // Get a handle for our "MVP" uniform
    // Only during the initialisation
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    //-------------------------------------------BUILD SCENE------------------------------------------//
    cam.setFOV(55);
    cam.setPos(glm::vec3(0.0f, 5.0f, -55.0f));
    cam.setLookDir(glm::vec3(0.0f, 0.0f, 1.0f));
    cam.setUpDir(glm::vec3(0.0f, 1.0f, 0.0f));
    cam.setRenderDistances(1.0, 70.0);


    //ScalarSphere s2 = ScalarSphere(glm::vec3(0.0f, 3.0f, 0.0f), 5.0f);
    //ScalarCutout cut = ScalarCutout(&s1, &s2);
    //ScalarTorus t = ScalarTorus(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, 3.0f);
    //ScalarSteinerPatch steiner = ScalarSteinerPatch(glm::vec3(0.0f));
    //--------------------MAKE CYLINDER------------------------//
    ScalarInfCylinder cyl = ScalarInfCylinder(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1);
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
    ScalarClamp m = ScalarClamp(&rot, 0.0f, 1.0f);

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


    glm::vec3 test = glm::vec3(1.0f, 1.0f, 0.0f);
    float result = m.eval(test);
    printf("sphere eval: %f\n", result);

    //-------------------------------------------SET UP VOLUME RENDERER------------------------------------------//
    volRenderer.setCamera(&cam);
    volRenderer.setDisplayBuf(&displayBuf);
    volRenderer.setTCoeff(100);
    volRenderer.setColorFields(&cAdd2, 1);
    volRenderer.setScalarFields(&m, 1);
    volRenderer.setMarchSteps(400);

    char fname[100];
    int numFrames = 4;
    for(int i = 1; i < numFrames; i++)
    {
        glm::vec3 cPos = glm::rotateY(glm::vec3(0.0f, 5.0f, -55.0f), 2*(float)PI * i/numFrames);
        glm::vec3 cLook = glm::rotateY(glm::vec3(0.0f, 0.0f, 1.0f), 2*(float)PI * i/numFrames);
        cam.setPos(cPos);
        cam.setLookDir(cLook);
        volRenderer.renderFrame();
        volRenderer.passToDisplay();
        sprintf(fname, "../turntable/turn%d.exr", i);
        displayBuf.writeImage(fname);
        printf("finished rendering %s\n", fname);
    }

    /*do {

        display(window, MatrixID, programID, vertexbuffer, mvp, numVerts);
		glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);*/

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

void display(GLFWwindow* window, GLuint matID, GLuint progID, GLuint vertBuf, glm::mat4 &mvp, int verts)
{
    //update the texture from the display buffer.
    glBindTexture(GL_TEXTURE_2D, textureID);
    Buffer2D* h;
    if(display_state == IMAGE) h = &displayBuf;
    else if(display_state == DENSITY) h = fluidModel.getDensity();
    else if(display_state == PRESSURE) h = fluidModel.getPressure();
    else if(display_state == ERROR) h = fluidModel.getError();

    int size = h->getHeight() * h->getWidth() * h->getNumChannels();
    float* buf = new float[h->getHeight() * h->getWidth() * h->getNumChannels()];
    memcpy(buf, h->getBuf(), sizeof(float) * size);
    for(int i = 0; i < h->getWidth() * h->getHeight() * h->getNumChannels(); i++)
    {
        if(display_state != ERROR) buf[i] *= brightness;
        else buf[i] *= brightness * 10;
    }

    if(display_state == IMAGE) glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RGBA, GL_FLOAT, buf);
    //if(display_state == IMAGE) glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sourceBuf.getWidth(), sourceBuf.getHeight(), GL_RED, GL_FLOAT, sourceBuf.getBuf());)
    else if(display_state == DENSITY)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RED, GL_FLOAT, buf);
    }
    else if(display_state == PRESSURE)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RED, GL_FLOAT, buf);
    }
    else if(display_state == ERROR)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RG, GL_FLOAT, buf);
    }

    //draw stuff.
    //clear the screen.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    glUniformMatrix4fv(matID, 1, GL_FALSE, &mvp[0][0]);

    //set the shader.
    glUseProgram(progID);


    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );


    // 2nd attribute buffer : uv positions
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, verts);// Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    //swap image buffers (though shouldn't it be a buffer chain, technically?)
    glfwSwapBuffers(window);
}

void displaySPH(GLFWwindow* window, GLuint matID, GLuint progID, GLuint vertBuf, glm::mat4 &mvp, int verts)
{
    //update the texture from the display buffer.
    glBindTexture(GL_TEXTURE_2D, textureID);
    /*
    Buffer2D* h;
    if(display_state == IMAGE) h = &displayBuf;
    else if(display_state == DENSITY) h = fluidModel.getDensity();
    else if(display_state == PRESSURE) h = fluidModel.getPressure();
    else if(display_state == ERROR) h = fluidModel.getError();

    int size = h->getHeight() * h->getWidth() * h->getNumChannels();
    float* buf = new float[h->getHeight() * h->getWidth() * h->getNumChannels()];
    memcpy(buf, h->getBuf(), sizeof(float) * size);
    for(int i = 0; i < h->getWidth() * h->getHeight() * h->getNumChannels(); i++)
    {
        if(display_state != ERROR) buf[i] *= brightness;
        else buf[i] *= brightness * 10;
    }

    if(display_state == IMAGE) glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RGB, GL_FLOAT, buf);
    //if(display_state == IMAGE) glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sourceBuf.getWidth(), sourceBuf.getHeight(), GL_RED, GL_FLOAT, sourceBuf.getBuf());)
    else if(display_state == DENSITY)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RED, GL_FLOAT, buf);
    }
    else if(display_state == PRESSURE)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RED, GL_FLOAT, buf);
    }
    else if(display_state == ERROR)
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, h->getWidth(), h->getHeight(), GL_RG, GL_FLOAT, buf);
    }
    */

    //draw stuff.
    //clear the screen.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
    glUniformMatrix4fv(matID, 1, GL_FALSE, &mvp[0][0]);

    //set the shader.
    glUseProgram(progID);


    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts * 3, g_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );


    // 2nd attribute buffer : uv positions
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts * 3, g_uv_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    // Draw the triangle !
    glDrawArrays(GL_POINTS, 0, verts);// Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    //swap image buffers (though shouldn't it be a buffer chain, technically?)
    glfwSwapBuffers(window);
}

void update(double ts)
{
    fluidModel.runTimeStep(ts);
}

void keyHandler(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    switch(key)
    {
     case GLFW_KEY_R:
        if(action == GLFW_PRESS)
        {
            displayBuf.setDataBuffer(loadedImg);
            sourceBuf.zeroOut();
            fluidModel.reset();
            printf("SIMULATION RESET!\n");
        }
        break;
    case GLFW_KEY_O:
        if(action == GLFW_PRESS)
        {
            if(paint_mode == PAINT_SOURCE)
            {
                paint_mode = PAINT_OBSTRUCTION;
                printf("Paint mode: Obstruction\n");
            }
            else
            {
                paint_mode = PAINT_SOURCE;
                printf("Paint mode: Source\n");
            }
        }
        break;
    case GLFW_KEY_I:
        if(action == GLFW_PRESS)
        {
            if(paint_mode == PAINT_SOURCE)
            {
                paint_mode = PAINT_TARGET;
                printf("Paint mode: Target\n");
            }
            else
            {
                paint_mode = PAINT_SOURCE;
                printf("Paint mode: Source\n");
            }
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

void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    switch(button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
        if(action == GLFW_PRESS)
        {
            if(prog_state & SPH)
            {
                glfwGetCursorPos(window, &xpos, &ypos);
                xpos = xpos / WIDTH;
                ypos = ypos / HEIGHT;
                xpos = xpos * sphModel.getWidth();
                ypos = ypos * sphModel.getHeight();
                ypos = sphModel.getHeight() - ypos;
                holdXPos = xpos;
                holdYPos = ypos;
            }
            else
            {
                glfwGetCursorPos(window, &xpos, &ypos);
                dabSomePaint(&displayBuf, xpos, ypos);
                dabSomePaint(&sourceBuf, xpos, ypos);
                fluidModel.setHasSource(true);
                prog_state = prog_state | DRAW;
            }
        }
        else if(action == GLFW_RELEASE)
        {
            if(prog_state & SPH)
            {
                glfwGetCursorPos(window, &xpos, &ypos);
                xpos = xpos / WIDTH;
                ypos = ypos / HEIGHT;
                xpos = xpos * sphModel.getWidth();
                ypos = ypos * sphModel.getHeight();
                ypos = sphModel.getHeight() - ypos;
                int idx = sphModel.addPart(Particle(glm::vec2(holdXPos, holdYPos), glm::vec3(0.0f, 1.0f, 0.0f), 10, 0.5f));
                Particle* p = sphModel.getPart(idx);
                p->setVelocity(glm::vec2((xpos - holdXPos) * 10, (ypos - holdYPos) * 10));

            }
            else prog_state = prog_state & ~DRAW;
        }
        break;
        default:
        break;
    }
}

void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos)
{
    if(prog_state & DRAW)
    {
        dabSomePaint(&displayBuf, xpos, ypos);
        dabSomePaint(&sourceBuf, xpos, ypos);
        fluidModel.setHasSource(true);
    }
}

void initializeBrushes()
{
   paint_mode = PAINT_SOURCE;
   int brush_width = (BRUSH_SIZE-1)/2;
   for( int j=-brush_width;j<=brush_width;j++ )
   {
      int jj = j + brush_width;
      float jfactor =  (float(brush_width) - fabs(j) )/float(brush_width);
      for( int i=-brush_width;i<=brush_width;i++ )
      {
         int ii = i + brush_width;
         float ifactor =  (float(brush_width) - fabs(i) )/float(brush_width);
         float radius = (jfactor*jfactor + ifactor*ifactor)/2.0;
         source_brush[ii][jj] = pow(radius,0.5);
         obstruction_brush[ii][jj] = 1.0 - pow(radius, 1.0/4.0);
      }
   }
}


void dabSomePaint(Buffer2D* scr, int x, int y )
{
   int brush_width = (BRUSH_SIZE-1)/2;
   int xstart = x - brush_width;
   int ystart = y - brush_width;
   if( xstart < 0 ){ xstart = 0; }
   if( ystart < 0 ){ ystart = 0; }

   int xend = x + brush_width;
   int yend = y + brush_width;
   int iwidth = scr->getWidth();
   int iheight = scr->getHeight();
   if( xend >= iwidth ){ xend = iwidth-1; }
   if( yend >= iheight ){ yend = iheight-1; }
   float* baseimage = scr->getBuf();
   int nBytes = scr->getNumChannels();


   if( paint_mode == PAINT_OBSTRUCTION )
   {
      Buffer2D* obs = fluidModel.getObstruction();
      float* obsBuf = obs->getBuf();
      int obsChannel = obs->getNumChannels();
      for(int ix=xstart;ix <= xend; ix++)
      {
         for( int iy=ystart;iy<=yend; iy++)
         {
            int index = ix + iwidth*(iheight-iy-1);
            for(int w = 0; w < nBytes; w++)
            {
                if(nBytes * index + w < nBytes * iwidth * iheight) baseimage[nBytes*index + w] *= obstruction_brush[ix-xstart][iy-ystart];
            }

            if(obsChannel*index < obs->getHeight() * obs->getWidth() * obsChannel) obsBuf[obsChannel*index] = 0.0f;

        }
      }
   }
   else if( paint_mode == PAINT_SOURCE )
   {
      for(int ix=xstart;ix <= xend; ix++)
      {
         for( int iy=ystart;iy<=yend; iy++)
         {
             int index = ix + iwidth*(iheight-iy-1);
             for(int w = 0; w < nBytes; w++)
             {
                if(nBytes * index + w < nBytes * iwidth * iheight) baseimage[nBytes*index + w] += source_brush[ix-xstart][iy-ystart];
             }
         }
      }
   }
   else if( paint_mode == PAINT_TARGET )
   {
      for(int ix=xstart;ix <= xend; ix++)
      {
         for( int iy=ystart;iy<=yend; iy++)
         {
             int index = ix + iwidth*(iheight-iy-1);
             for(int w = 0; w < nBytes; w++)
             {
                if(nBytes * index + w < nBytes * iwidth * iheight)
                {
                    if(nBytes == 1) baseimage[nBytes*index + w] += source_brush[ix-xstart][iy-ystart];
                    else if(w == 1) baseimage[nBytes*index + w] += source_brush[ix-xstart][iy-ystart];
                    else
                    {
                        baseimage[nBytes*index + w] += source_brush[ix-xstart][iy-ystart];
                    }
                }
             }
         }
      }
   }


   return;
}

void convertSourceIn(float intensity)
{
    Buffer2D* source = fluidModel.getSource();
    float* sourceGrid = source->getBuf();
    float* displayGrid = displayBuf.getBuf();
    float* inGrid = sourceInBuf.getBuf();
    int width = source->getWidth();
    int height = source->getHeight();

    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            int index = i + j * width;
            sourceGrid[index * source->getNumChannels()] += inGrid[index * sourceInBuf.getNumChannels()] * intensity;
            displayGrid[index * displayBuf.getNumChannels()] += inGrid[index * sourceInBuf.getNumChannels()] * intensity;
            displayGrid[index * displayBuf.getNumChannels() + 1] += inGrid[index * sourceInBuf.getNumChannels()] * intensity;
            displayGrid[index * displayBuf.getNumChannels() + 2] += inGrid[index * sourceInBuf.getNumChannels()] * intensity;
        }
    }
    fluidModel.setHasSource(true);
}


GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
