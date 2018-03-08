#ifndef STRUCTPILE_H
#define STRUCTPILE_H

#define YFOV    50.625f
#define ZNEAR   0.01f
#define ZFAR    1000.0f
#define RGBA    4

#define MAX_IMAGES 10
#define PI 3.14159265359

#define MAX_PICTURES 10
#define MAX_SHADERS 10
#define MAX_MODELS 10
#define MAX_PHYS_OBJECTS 100000
#define MAX_FORCES 10
#define MAX_FUNCTIONS 100
#define MAX_GENERATORS 10
#define MAX_CHILDREN 100
#define MAX_PARENTS 100
#define MAX_EDGE_CHILDREN MAX_CHILDREN
#define MAX_POLYGON_CHILDREN MAX_CHILDREN
#define MAX_PARTICLE_PARENTS MAX_PARENTS

#define NUM_PARTS 10000
#define NUM_DERIV_STATES 4

#include <../glm-0.9.8.5/glm/glm.hpp>

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
    glm::vec3       scale;
    float           radius;
    float           width;
    float           height;
    float           depth;
};

struct state
{
    glm::vec3 rotation;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
};

struct attributes
{
    double ttl;
    float mass, springL, springD, springK;
    int id;
    int numChildren, numParents;
    bool alive, active, solid, particle;
    geometry geo;
};

enum{ PAINT_OBSTRUCTION, PAINT_SOURCE, PAINT_DIVERGENCE, PAINT_COLOR };

struct triangle {
    glm::vec3   a;
    glm::vec3   b;
    glm::vec3   c;
};

enum collideType{
    SPHERE = 0,
    PLANE = 1,
    POLYGON = 2,
    PARTICLE = 3,
    EDGE = 4,
    COLLECTION = 5
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
    RUNNING = 1,
    DRAW = 1 << 1,
    PAUSED = 1 << 2,
    SOURCEIN = 1 << 3,
};

#endif // STRUCTPILE_H
