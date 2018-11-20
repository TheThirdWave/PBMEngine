#ifndef VECTORFIELDS_H
#define VECTORFIELDS_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "grid.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//
class VecGrid:public Field<glm::vec3>
{
public:
    VecGrid(Grid<glm::vec3>* g, glm::vec3 def)
    {
        LLC = g->getLLC();
        URC = g->getURC();
        int arr[3];
        g->getDimensions(arr);
        Nx = arr[0];
        Ny = arr[1];
        Nz = arr[2];
        dx = (URC.x - LLC.x) / Nx;
        dy = (URC.y - LLC.y) / Ny;
        dz = (URC.z - LLC.z) / Nz;
        grid = g;
        defaultVal = def;
    }

    const glm::vec3 eval(const glm::vec3 &P) const
    {
        glm::vec3 Xg = P - LLC;
        int i = Xg.x / dx;
        int j = Xg.y / dy;
        int k = Xg.z / dz;
        if(i < 0 || i >= Nx - 1 || j < 0 || j >= Ny - 1 || k < 0 || k >= Nz - 1) return P;
        return grid->trilerp(P);
    }

    void setGrid(Grid<glm::vec3>* g)
    {
        LLC = g->getLLC();
        URC = g->getURC();
        int arr[3];
        g->getDimensions(arr);
        Nx = arr[0];
        Ny = arr[1];
        Nz = arr[2];
        dx = (URC.x - LLC.x) / Nx;
        dy = (URC.y - LLC.y) / Ny;
        dz = (URC.z - LLC.z) / Nz;
        grid = g;
    }

    void setDefaultVal(glm::vec3 df)
    {
        defaultVal = df;
    }
private:
    glm::vec3 LLC;
    glm::vec3 URC;
    int Nx;
    int Ny;
    int Nz;
    float dx;
    float dy;
    float dz;
    Grid<glm::vec3>* grid;
    glm::vec3 defaultVal;
};

class VecVelField:public Field<glm::vec3>
{
  public:
    VecVelField(const Field<float>* fspn, glm::vec3 v)
    {
        FSPN = fspn;
        v1 = v;
    }
    const glm::vec3 eval(const glm::vec3 &p) const
    {
        return glm::vec3(FSPN->eval(p), FSPN->eval(p + v1), FSPN->eval(p - v1));
    }
private:
    const Field<float>* FSPN;
    glm::vec3 v1;
};

//-----------------------TRANSFORMS---------------------//
class VecTranslate:public Field<glm::vec3>
{
  public:
    VecTranslate(const Field<glm::vec3>* f, glm::vec3 v)
    {
        f1 = f;
        movVec = v;
    }
    const glm::vec3 eval(const glm::vec3 &p) const
    {
        return f1->eval(p - movVec);
    }
private:
    const Field<glm::vec3>* f1;
    glm::vec3 movVec;
};

class VecScale:public Field<glm::vec3>
{
public:
    VecScale(const Field<glm::vec3>* f, float s)
    {
        f1 = f;
        scaleFac = s;
    }
    const glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1->eval(P/scaleFac)*scaleFac;
    }
private:
    const Field<glm::vec3>* f1;
    float scaleFac;
};

class VecRotate:public Field<glm::vec3>
{
public:
    VecRotate(const Field<glm::vec3>* f, glm::vec3 r)
    {
        f1 = f;
        rotAxis = glm::normalize(r);
        rotAngle = glm::length(r);
    }
    const glm::vec3 eval(const glm::vec3 &P) const
    {
        float A = std::cos(rotAngle);
        float B = glm::dot(rotAxis, P) * (1 - A);
        float C = std::sin(rotAngle);
        glm::vec3 rotVec = P * A + rotAxis * B + glm::cross(rotAxis, P) * C;
        glm::vec3 fVec = f1->eval(-rotVec);
        B = glm::dot(rotAxis, fVec) * (1 - A);
        fVec = fVec * A + rotAxis * B + glm::cross(rotAxis, fVec) * C;
        return fVec;
    }
private:
    const Field<glm::vec3>* f1;
    glm::vec3 rotAxis;
    float rotAngle;
};

//-----------------------FIELD ALGEBRA STUFF---------------------//

class VectorFieldAdd:public Field<glm::vec3>
{
public:
    VectorFieldAdd(const Field<glm::vec3>* f, const Field<glm::vec3>* g)
    {
        f1 = f;
        f2 = g;
    }
    const glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1->eval(P) + f2->eval(P);
    }
private:
    const Field<glm::vec3>* f1;
    const Field<glm::vec3>* f2;
};

class VectorFieldSub:public Field<glm::vec3>
{
public:
    VectorFieldSub(const Field<glm::vec3>* f, const Field<glm::vec3>* g)
    {
        f1 = f;
        f2 = g;
    }
    const glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1->eval(P) - f2->eval(P);
    }
private:
    const Field<glm::vec3>* f1;
    const Field<glm::vec3>* f2;
};

class VectorFieldMult:public Field<glm::vec3>
{
public:
    VectorFieldMult(const Field<glm::vec3>* f, const Field<glm::vec3>* g)
    {
        f1 = f;
        f2 = g;
    }
    const glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1->eval(P) * f2->eval(P);
    }
private:
    const Field<glm::vec3>* f1;
    const Field<glm::vec3>* f2;
};

class VectorFieldDiv:public Field<glm::vec3>
{
public:
    VectorFieldDiv(const Field<glm::vec3>* f, const Field<glm::vec3>* g)
    {
        f1 = f;
        f2 = g;
    }
    const glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1->eval(P) / f2->eval(P);
    }
private:
    const Field<glm::vec3>* f1;
    const Field<glm::vec3>* f2;
};

class VectorFieldCross:public Field<glm::vec3>
{
public:
    VectorFieldCross(const Field<glm::vec3>* f, const Field<glm::vec3>* g)
    {
        f1 = f;
        f2 = g;
    }
    const glm::vec3 eval(const glm::vec3 &P) const
    {
        return glm::cross(f1->eval(P), f2->eval(P));
    }
private:
    const Field<glm::vec3>* f1;
    const Field<glm::vec3>* f2;
};

/*class VectorFieldOuterCross:public Field<glm::vec3>
{
public:
    VectorFieldOuterCross(const Field<glm::vec3>* f, const Field<glm::vec3>* g)
    {
        f1 = f;
        f2 = g;
    }
    const glm::mat3x3 eval(const glm::vec3* P) const
    {
        return glm::outerProduct(f1->eval(P), f2->eval(P));
    }
private:
    const Field<glm::vec3>* f1;
    const Field<glm::vec3>* f2;
};*/

#endif // VECTORFIELDS_H
