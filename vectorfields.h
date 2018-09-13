#ifndef VECTORFIELDS_H
#define VECTORFIELDS_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//

//-----------------------TRANSFORMS---------------------//
class VecTranslate:public Field<glm::vec3>
{
  public:
    VecTranslate(const Field<glm::vec3> &f, glm::vec3 v)
    {
        f1 = f;
        movVec = v;
    }
    glm::vec3 eval(const glm::vec3 &p) const
    {
        return f1.eval(p - movVec);
    }
private:
    Field<glm::vec3>& f1;
    glm::vec3 movVec;
};

class VecScale:public Field<glm::vec3>
{
public:
    VecScale(const Field<glm::vec3> &f, float s)
    {
        f1 = f;
        scaleFac = s;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1.eval(p/s)*s;
    }
private:
    Field<glm::vec3>& f1;
    float scaleFac;
};

class VecRotate:public Field<glm::vec3>
{
public:
    VecRotate(const Field<glm::vec3> &f, glm::vec3 r)
    {
        f1 = f;
        rotAxis = glm::normalize(r);
        rotAngle = glm::length(r);
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        float A = std::cos(rotAngle);
        float B = glm::dot(rotAxis, P) * (1 - A);
        float C = std::sin(rotAngle);
        glm::vec3 rotVec = P * A + rotAxis * B + glm::cross(rotAxis, P) * C;
        glm::vec3 fVec = f1.eval(glm::inverse(rotVec));
        B = glm::dot(rotAxis, fVec) * (1 - A);
        fVec = fVec * A + rotAxis * B + glm::cross(rotAxis, fVec) * C;
        return fVec;
    }
private:
    Field<glm::vec3> f1;
    glm::vec3 rotAxis;
    float rotAngle;
};

//-----------------------FIELD ALGEBRA STUFF---------------------//

class VectorFieldAdd:public Field<glm::vec3>
{
public:
    VectorFieldAdd(const Field<glm::vec3>& f, const Field<glm::vec3>& g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1.eval(p) + f2.eval(p);
    }
private:
    Field<glm::vec3>& f1;
    Field<glm::vec3>& f2;
};

class VectorFieldSub:public Field<glm::vec3>
{
public:
    VectorFieldSub(const Field<glm::vec3>& f, const Field<glm::vec3>& g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1.eval(p) - f2.eval(p);
    }
private:
    Field<glm::vec3>& f1;
    Field<glm::vec3>& f2;
};

class VectorFieldMult:public Field<glm::vec3>
{
public:
    VectorFieldMult(const Field<glm::vec3>& f, const Field<glm::vec3>& g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1.eval(p) * f2.eval(p);
    }
private:
    Field<glm::vec3>& f1;
    Field<glm::vec3>& f2;
};

class VectorFieldDiv:public Field<glm::vec3>
{
public:
    VectorFieldDiv(const Field<glm::vec3>& f, const Field<glm::vec3>& g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1.eval(p) / f2.eval(p);
    }
private:
    Field<glm::vec3>& f1;
    Field<glm::vec3>& f2;
};

class VectorFieldDot:public Field<glm::vec3>
{
public:
    VectorFieldDot(const Field<glm::vec3>& f, const Field<glm::vec3>& g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return glm::dot(f1.eval(p), f2.eval(p));
    }
private:
    Field<glm::vec3>& f1;
    Field<glm::vec3>& f2;
};

class VectorFieldCross:public Field<glm::vec3>
{
public:
    VectorFieldCross(const Field<glm::vec3>& f, const Field<glm::vec3>& g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return glm::cross(f1.eval(p), f2.eval(p));
    }
private:
    Field<glm::vec3>& f1;
    Field<glm::vec3>& f2;
};

class VectorFieldCross:public Field<glm::vec3>
{
public:
    VectorFieldCross(const Field<glm::vec3>& f, const Field<glm::vec3>& g)
    {
        f1 = f;
        f2 = g;
    }
    glm::mat3x3 eval(const glm::vec3 &P) const
    {
        return glm::outerProduct(f1.eval(p), f2.eval(p));
    }
private:
    Field<glm::vec3>& f1;
    Field<glm::vec3>& f2;
};

#endif // VECTORFIELDS_H
