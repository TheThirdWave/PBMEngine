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
#include <../glm-0.9.8.5/glm/glm.hpp>
#include <../glm-0.9.8.5/glm/gtc/matrix_transform.hpp>

//image buffer stuff
#include "buffer2d.h"

//miscellaneous structs and enums and stuff.
#include "structpile.h"

//Dr. Tessendorf's weird command line reading thing
#include "CmdLineFind.h"

//the actual physics engine.
#include "fluidmodel.h"

//headers
void keyHandler(GLFWwindow* win, int key, int scancode, int action, int mods);
void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos);
void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods);
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
void initializeBrushes();
void dabSomePaint(Buffer2D *scr, int x, int y);
void display(GLFWwindow* window, GLuint matID, GLuint progID, GLuint vertBuf, glm::mat4 &mvp);
void update(double ts);

//static variables.
static float WIDTH, HEIGHT;

//globals (because I am lazy)
Buffer2D loadedImg;
Buffer2D displayBuf;
Buffer2D sourceBuf;
GLuint uvbuffer, textureID;
FluidModel fluidModel;

#define BRUSH_SIZE 11
float obstruction_brush[BRUSH_SIZE][BRUSH_SIZE];
float source_brush[BRUSH_SIZE][BRUSH_SIZE];
int paint_mode;

int prog_state;
double prev_time, cur_time, delta_time, d_delta_time, d_prev_time, d_cur_time;
double timeStep = 1.0 / (60.0 * 1.0);
double simTime = 0.0f;
double displayTime = 1.0f / (60.0f);

int main(int argc, char* argv[])
{
    //load initial images;
    loadedImg.readImage("../GWbackground.png");
    displayBuf.readImage("../GWbackground.png");
    sourceBuf.init(displayBuf.getWidth(), displayBuf.getHeight(), 1, 1.0);

    //init brushes.
    initializeBrushes();

    //set program state;
    prog_state = IDLE;

    //initialize Fluid Model.
    fluidModel.init(&displayBuf, &sourceBuf);

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
	window = glfwCreateWindow(WIDTH, HEIGHT, "Tutorial 01", NULL, NULL);
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
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f,-1.0f, -1.0f, // triangle 1 : begin
        1.0f,-1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, // triangle 1 : end
        1.0f, 1.0f, -1.0f, // triangle 2 : begin
        1.0f,-1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, // triangle 2 : end
      };

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

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
    static const GLfloat g_uv_buffer_data[] = {
        0.0f,  0.0f,
        1.0f,  0.0f,
        0.0f,  1.0f,
        1.0f,  1.0f,
        1.0f,  0.0f,
        0.0f,  1.0f
    };

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);


    //load the shaders.
    GLuint programID = LoadShaders("../PBMEngine/VertS1.glsl", "../PBMEngine/FragS1.glsl");

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


    cur_time = glfwGetTime();
    d_cur_time = cur_time;
    d_prev_time = d_cur_time;

    do {
/*
        cur_time = glfwGetTime();
        delta_time = cur_time - prev_time;
        /*if(delta_time2 > delta_time)
        {
            delta_time2 = timeStep;
            delta_time = timeStep;
        }
        if(delta_time >= timeStep)
        {
            prev_time = cur_time;
            double loops  = 0;
            while(loops < delta_time)
            {
                update(timeStep);

                loops += timeStep;
            }
        }
*/
        update(timeStep);
        simTime += timeStep;
        d_cur_time = glfwGetTime();
        d_delta_time = d_cur_time - d_prev_time;
        if(d_delta_time >= displayTime)
        {
            d_prev_time = d_cur_time;
            display(window, MatrixID, programID, vertexbuffer, mvp);
        }
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

}

void display(GLFWwindow* window, GLuint matID, GLuint progID, GLuint vertBuf, glm::mat4 &mvp)
{
    //update the texture from the display buffer.
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, displayBuf.getWidth(), displayBuf.getHeight(), GL_RGB, GL_FLOAT, displayBuf.getBuf());
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sourceBuf.getWidth(), sourceBuf.getHeight(), GL_R, GL_FLOAT, sourceBuf.getBuf());

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
    glDrawArrays(GL_TRIANGLES, 0, 2 * 3);// Starting from vertex 0; 3 vertices total -> 1 triangle
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    //swap image buffers (though shouldn't it be a buffer chain, technically?)
    glfwSwapBuffers(window);
}

void update(double ts)
{
    fluidModel.runSLTimeStep(ts);
}

void keyHandler(GLFWwindow* win, int key, int scancode, int action, int mods)
{
    switch(key)
    {
     case GLFW_KEY_R:
        if(action == GLFW_PRESS)
        {
            displayBuf.setDataBuffer(loadedImg);
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
             glfwGetCursorPos(window, &xpos, &ypos);
             dabSomePaint(&displayBuf, xpos, ypos);
             dabSomePaint(&sourceBuf, xpos, ypos);
             fluidModel.setHasSource(true);
             prog_state = DRAW;
        }
        else if(action == GLFW_RELEASE)
        {
            prog_state = IDLE;
        }
        break;
        default:
        break;
    }
}

void mouseMoveHandler(GLFWwindow* window, double xpos, double ypos)
{
    if(prog_state == DRAW)
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
      for(int ix=xstart;ix <= xend; ix++)
      {
         for( int iy=ystart;iy<=yend; iy++)
         {
            int index = ix + iwidth*(iheight-iy-1);
            for(int w = 0; w < nBytes; w++)
            {
                baseimage[nBytes*index + w] *= obstruction_brush[ix-xstart][iy-ystart];
            }
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
                baseimage[nBytes*index + w] += source_brush[ix-xstart][iy-ystart];
             }
         }
      }
   }


   return;
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
