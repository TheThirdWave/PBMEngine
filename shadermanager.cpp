#include "shadermanager.h"
using namespace std;

ShaderManager::ShaderManager()
{
    vPtr = 0;
    fPtr = 0;
    cPtr = 0;
}

int ShaderManager::loadVertexShader(char *path)
{
    if(vPtr != MAX_SHADERS)
    {
        char* buf1;
        ifstream shadeReader;
        shadeReader.open(path);
        if(shadeReader.is_open())
        {
            streampos begin, end;
            begin = shadeReader.tellg();
            shadeReader.seekg(0, ios::end);
            end = shadeReader.tellg();
            shadeReader.seekg(0, ios::beg);
            buf1 = new char[end-begin + 1];

            shadeReader.read(buf1, end);
            if(!shadeReader)
            {
                fprintf(stderr,"Could not read Vertex shader\n");
            }
            buf1[end-begin] = '\0';
            shadeReader.close();


            vertexShadersRaw[vPtr] = buf1;
        }
        else
        {
            fprintf(stderr, "ifstream not open.\n");
            return -1;
        }

        vShaders[vPtr] = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vShaders[vPtr], 1,  &vertexShadersRaw[vPtr], NULL);
        glCompileShader(vShaders[vPtr]);

        GLint status;
        glGetShaderiv(vShaders[vPtr], GL_COMPILE_STATUS, &status);
        if(status != GL_TRUE)
        {
            char buffer[512];
            glGetShaderInfoLog(vShaders[vPtr], 512, NULL, buffer);
            fprintf(stderr,"Error: %s\n", buffer);
            return -1;
        }

        return vPtr++;
    }
    else
    { fprintf(stderr, "Error: Cannot load more shaders!\n");
      return -1;
    }
}

int ShaderManager::loadFragmentShader(char *path)
{
    if(fPtr != MAX_SHADERS)
    {
        char* buf1;
        ifstream shadeReader;
        shadeReader.open(path);
        if(shadeReader.is_open())
        {
            streampos begin, end;
            begin = shadeReader.tellg();
            shadeReader.seekg(0, ios::end);
            end = shadeReader.tellg();
            shadeReader.seekg(0, ios::beg);
            buf1 = new char[end-begin + 1];

            shadeReader.read(buf1, end);
            if(!shadeReader)
            {
                fprintf(stderr,"Could not read Fragment shader\n");
            }
            buf1[end-begin] = '\0';
            shadeReader.close();

            fragmentShadersRaw[fPtr] = buf1;
        }
        else
        {
            fprintf(stderr, "ifstream not open.\n");
            return -1;
        }

        fShaders[fPtr] = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fShaders[fPtr], 1, &fragmentShadersRaw[fPtr], NULL);
        glCompileShader(fShaders[fPtr]);

        GLint status;
        glGetShaderiv(fShaders[fPtr], GL_COMPILE_STATUS, &status);
        if(status != GL_TRUE)
        {
            char buffer[512];
            glGetShaderInfoLog(fShaders[fPtr], 512, NULL, buffer);
            fprintf(stderr,"Error: %s\n", buffer);
            return -1;
        }

        return fPtr++;
    }
    else
    { fprintf(stderr, "Error: Cannot load more shaders!\n");
      return -1;
    }
}

int ShaderManager::combineShaders(int vertShade, int fragShade)
{
    if(cPtr != MAX_SHADERS)
    {
        cShaders[cPtr] = glCreateProgram();
        glAttachShader(cShaders[cPtr], vShaders[vertShade]);
        glAttachShader(cShaders[cPtr], fShaders[fragShade]);

        glBindFragDataLocation(cShaders[cPtr], 0, "outColor");

        GLint status;
        glLinkProgram(cShaders[cPtr]);
        glGetProgramiv(cShaders[cPtr], GL_LINK_STATUS, &status);
        if(status != GL_TRUE)
        {
            char buffer[512];
            glGetProgramInfoLog(cShaders[cPtr], 512, NULL, buffer);
            fprintf(stderr,"Error: %s\n", buffer);
        }
        return cPtr++;
    }
    else fprintf(stderr, "Error: Cannot load more shaders!\n");
    return -1;
}

void ShaderManager::configure2DShaders(GLuint cShade)
{
    GLuint posAttrib = glGetAttribLocation(cShade, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);
    posAttrib = glGetAttribLocation(cShade, "texCoord");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(posAttrib);
}

void ShaderManager::configure3DShaders(GLuint cShade, RenderObject* obj){
    GLuint posAttrib = glGetAttribLocation(cShade, "position");
    glBindBuffer(GL_ARRAY_BUFFER, obj->getVertBufName());
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);
    posAttrib = glGetAttribLocation(cShade, "color");
    glBindBuffer(GL_ARRAY_BUFFER, obj->getColBufName());
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);
}

int ShaderManager::set2dShaderProgram(int combShade)
{
    glUseProgram(cShaders[combShade]);
    configure2DShaders(cShaders[combShade]);
    return 1;
}

int ShaderManager::set3dShaderProgram(int combShade)
{
    glUseProgram(cShaders[combShade]);
    return 1;
}

GLuint ShaderManager::getVertexShader(int idx)
{
    return vShaders[idx];
}

GLuint ShaderManager::getFragmentShader(int idx)
{
    return fShaders[idx];
}

GLuint ShaderManager::getCombinedShader(int idx)
{
    return cShaders[idx];
}
