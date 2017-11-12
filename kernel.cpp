#include "kernel.h"

Kernel::Kernel()
{
}

Kernel::Kernel(int w, int h)
{
    wr = w;
    hr = h;
    width = w*2+1;
    height = h*2+1;
    weights = new float*[height];
    for(int i = 0; i < height; i++)
    {
        weights[i] = new float[width];
    }
}

void Kernel::setWeights(float f)
{
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            weights[i][j] = f;
        }
    }
}

void Kernel::setFuncWeights(Function2D * f)
{
    float hold = 0.0f;
    f->setPoint(glm::vec2(wr + 0.5, hr + 0.5));
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j ++)
        {
            weights[i][j] = abs(1/f->getRelative(glm::vec2(j + 0.5, i + 0.5)));
            hold += weights[i][j];
        }
    }
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j ++)
        {
            weights[i][j] = weights[i][j] / hold;
        }
    }
}

void Kernel::setFExactWeights(Function2D * f)
{
    float hold2 = 0.0f;
    f->setPoint(glm::vec2(wr + 0.5, hr + 0.5));
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j ++)
        {
            hold2 = f->getRelative(glm::vec2(j + 0.5, i + 0.5));
            if(abs(hold2) < 0.5) weights[i][j] =  1;
            else weights[i][j] = 0;
        }
    }
}


void Kernel::setFExactWeightsAvg(Function2D * f)
{
    float hold = 0.0f;
    float hold2 = 0.0f;
    f->setPoint(glm::vec2(wr + 0.5, hr + 0.5));
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j ++)
        {
            hold2 = f->getRelative(glm::vec2(j + 0.5, i + 0.5));
            if(abs(hold2) < 0.5) weights[i][j] =  1;
            else weights[i][j] = 0;
            hold += weights[i][j];
        }
    }
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j ++)
        {
            weights[i][j] = weights[i][j] / hold;
        }
    }
}

void Kernel::makeIdentity()
{
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if(i == hr && j == wr) weights[i][j] = 1;
            else weights[i][j] = 0;
        }
    }
}

void Kernel::makeNegIdentity()
{
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if(i == hr && j == wr) weights[i][j] = 0;
            else weights[i][j] = 1;
        }
    }
}

void Kernel::invertKernel()
{
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            weights[i][j] == -weights[i][j];
        }
    }
}
