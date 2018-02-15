#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "structpile.h"z

class ModelManager
{
    model models[MAX_MODELS];
    int modelIndex;
public:
    ModelManager();
    int readObj(char*);
    model* getModel(int);
};

#endif // MODELMANAGER_H
