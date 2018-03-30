#ifndef MODELMANAGER_H
#define MODELMANAGER_H

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
    int modelIndex;
public:
    ModelManager();
    int readObj(char*);
    int readObjLoader(char*);
    model* getModel(int);
};

#endif // MODELMANAGER_H
