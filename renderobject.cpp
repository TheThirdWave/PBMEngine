#include "renderobject.h"

RenderObject::RenderObject()
{
    posMatrix = glm::mat4(1.0f);
    data = new model();
}

void RenderObject::setModel(model* inData)
{
    data = inData;
    setVertexBuffer(data->vertices, data->vertLen);
    setColorBuffer(data->colors, data->colorLen);
    setIndexBuffer(data->vertIdx, data->idxLen);
}

void RenderObject::setIndexBuffer(unsigned int* ptr, int size)
{
    data->vertIdx = ptr;
    data->idxLen = size;

    glGenBuffers(1, &glIdxName);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glIdxName);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * data->idxLen, data->vertIdx, GL_STATIC_DRAW);
}

void RenderObject::setVertexBuffer(float* ptr, int size)
{
    data->vertices = ptr;
    data->vertLen = size;

    glGenBuffers(1, &glBufName);
    glBindBuffer(GL_ARRAY_BUFFER, glBufName);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data->vertLen, data->vertices, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderObject::setColorBuffer(float* ptr, int size)
{
    data->colors = ptr;
    data->colorLen = size;

    glGenBuffers(1, &glColName);
    glBindBuffer(GL_ARRAY_BUFFER, glColName);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data->colorLen, data->colors, GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderObject::setPosMatrix(glm::mat4 mat)
{
    posMatrix = mat;
}

unsigned int* RenderObject::getIndexBuffer()
{
    return data->vertIdx;
}

float* RenderObject::getColorBuffer()
{
    return data->colors;
}

float* RenderObject::getVertexBuffer()
{
    return data->vertices;
}

GLuint RenderObject::getIdxBufName()
{
    return glIdxName;
}

GLuint RenderObject::getColBufName()
{
    return glColName;
}

GLuint RenderObject::getVertBufName()
{
    return glBufName;
}

model* RenderObject::getData()
{
    return data;
}

glm::mat4* RenderObject::getMatrix()
{
    return &posMatrix;
}

glm::mat4* RenderObject::rotateMatrix(float angle, float x, float y, float z)
{
    glm::mat4 hold = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(x, y, z));
    posMatrix = posMatrix * hold;
    return &posMatrix;
}

glm::mat4* RenderObject::translateMatrix(float x, float y, float z)
{
    glm::mat4 hold = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
    posMatrix = posMatrix * hold;
    return &posMatrix;
}

glm::mat4* RenderObject::scaleMatrix(float x, float y, float z)
{
    glm::mat4 hold = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
    posMatrix = posMatrix* hold;
    return &posMatrix;
}
