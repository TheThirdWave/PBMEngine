#ifndef KERNEL_H
#define KERNEL_H

#include "structpile.h"
#include "function2d.h"

class Kernel
{
    friend class Imagemanip;
private:
    int width, height;
    int wr, hr;
    float** weights;
public:
    Kernel();
    Kernel(int, int);
    void setWeights(float);
    void setFuncWeights(Function2D*);
    void setFExactWeightsAvg(Function2D*);
    void setFExactWeights(Function2D*);
    void setFExact(Function2D*);
    void makeIdentity();
    void makeNegIdentity();
    void invertKernel();
};

#endif // KERNEL_H
