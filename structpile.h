#ifndef STRUCTPILE_H
#define STRUCTPILE_H

#define YFOV    50.625f
#define ZNEAR   0.01f
#define ZFAR    100.0f
#define RGBA    4

#define MAX_IMAGES 10
#define MAX_PICTURES 10
#define MAX_SHADERS 10
#define MAX_MODELS 10
#define MAX_PHYS_OBJECTS 10
#define MAX_FORCES 10
#define MAX_FUNCTIONS 100

#include "../glm-0.9.8.5/glm/glm.hpp"

struct image {
    unsigned char* data;
    int height;
    int width;
    int unitbytes;
    unsigned int size;
    int* rowsize;
};

struct model {
    float*          vertices;
    float*            colors;
    unsigned int*   indicies;
    int             vertLen, idxLen, colorLen;
};

struct geometry {
    glm::vec3       normal;
    float           radius;
    float           width;
    float           height;
    float           depth;
};

enum collideType{
    SPHERE = 0,
    PLANE = 1
};

enum progState{
    CONVEX = 0,
    STAR = 1
};

#endif // STRUCTPILE_H
