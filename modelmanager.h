#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include "../tinyobjloader-master/tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "structpile.h"

class ModelManager
{
    model models[MAX_MODELS];
    tinyobj::attrib_t modelHold;
    int modelIndex;
public:
    ModelManager();
    int readObj(char*);
    void readObjLoader(char*);
    model* getModel(int);
};

#endif // MODELMANAGER_H
