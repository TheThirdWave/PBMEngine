#ifndef STRUCTPILE_H
#define STRUCTPILE_H

#define YFOV    50.625f
#define ZNEAR   0.01f
#define ZFAR    100.0f
#define RGBA    4

#define MAX_IMAGES 10
#define PI 3.14159265359

#define MAX_PICTURES 10
#define MAX_SHADERS 10
#define MAX_MODELS 10
#define MAX_PHYS_OBJECTS 100000
#define MAX_FORCES 10
#define MAX_FUNCTIONS 100
#define MAX_POINTS 1000
#define MAX_GENERATORS 10

#define NUM_PARTS 1000

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
    glm::vec3       upVec;
    float           radius;
    float           width;
    float           height;
    float           depth;
};

struct triangle {
    glm::vec3   a;
    glm::vec3   b;
    glm::vec3   c;
};

enum collideType{
    SPHERE = 0,
    PLANE = 1,
    POLYGON = 2,
    PARTICLE = 3
};

enum keyState{
    FORWARD = 1,
    SLEFT = 1 << 1,
    BACK = 1 << 2,
    SRIGHT = 1 << 3,
    UP = 1 << 4,
    DOWN = 1 << 5,
    LUP = 1 << 6,
    LDOWN = 1 << 7,
    LLEFT = 1 << 8,
    LRIGHT = 1 << 9
};

enum progState{
    CONVEX = 0,
    STAR = 1,
    FUNCTION = 2,
    BLOBBY = 3,
    SHADED = 4,
    MODULUS = 5,
    BLUR = 6,
    EMBOSS = 7,
    DILATION = 8,
    EROSION = 9,
    MBLUR = 10,
    MDILATION = 11,
    MEROSION = 12,
    BDLPF = 13,
    ALPHA = 14,
    MULT = 15,
    SUBTRACT = 16,
    MAX = 17,
    MIN = 18,
    COMPOSITE = 19,
    DITHER = 20,
    FSDITHER = 21,
    CDITHER = 22,
    DIFFUSE = 23
};

#endif // STRUCTPILE_H
