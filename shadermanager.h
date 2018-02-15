#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "structpile.h"
#include "renderobject.h"z

class ShaderManager
{
private:
    const char* vertexShadersRaw[MAX_SHADERS];
    const char* fragmentShadersRaw[MAX_SHADERS];

    GLuint vShaders[MAX_SHADERS];
    GLuint fShaders[MAX_SHADERS];
    GLuint cShaders[MAX_SHADERS];

    int vPtr, fPtr, cPtr;

    void configure2DShaders(GLuint cShade);

public:
    ShaderManager();
    int loadVertexShader(char* path);
    int loadFragmentShader(char* path);
    int combineShaders(int vertShade, int fragShade);
    int set2dShaderProgram(int combShade);
    int set3dShaderProgram(int combShade);
    void configure3DShaders(GLuint cShade, RenderObject*);
    GLuint getVertexShader(int idx);
    GLuint getFragmentShader(int idx);
    GLuint getCombinedShader(int idx);

};

#endif // SHADERMANAGER_H
