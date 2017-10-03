#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sys/time.h>
#include "../glm-0.9.8.5/glm/glm.hpp"
#include "../glm-0.9.8.5/glm/gtc/matrix_transform.hpp"
#include "../glm-0.9.8.5/glm/gtc/type_ptr.hpp"
#include "../glm-0.9.8.5/glm/gtx/transform.hpp"

#include "modelmanager.h"

class RenderObject
{
    friend class PhysicsManager;
private:
    glm::mat4 posMatrix;
    model* data;
    GLuint glBufName, glIdxName, glColName;

public:
    RenderObject();
    void setModel(model*);
    void setVertexBuffer(float*, int);
    void setColorBuffer(float*, int);
    void setIndexBuffer(unsigned int*, int);
    void setPosMatrix(glm::mat4);
    float* getVertexBuffer();
    float* getColorBuffer();
    unsigned int* getIndexBuffer();
    GLuint getVertBufName();
    GLuint getColBufName();
    GLuint getIdxBufName();
    model* getData();
    glm::mat4* getMatrix();
    glm::mat4* rotateMatrix(float, float, float, float);
    glm::mat4* translateMatrix(float, float, float);
    glm::mat4* scaleMatrix(float, float, float);
};

#endif // RENDEROBJECT_H
