#ifndef VECTORFIELDS_H
#define VECTORFIELDS_H

#include <../glm-0.9.8.5/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//

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
        return f1.eval(p) * f2.eval(p);
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
