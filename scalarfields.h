#ifndef SCALARFIELDS_H
#define SCALARFIELDS_H

#include <../glm-0.9.8.5/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//

class ScalarFieldSphere:public Field<float>
{
public:
    ScalarFieldSphere(glm::vec3 c, float r)
    {
       radius = r;
       center = f;
    }

    float eval(const glm::vec3 &p) const
    {
        return radius - glm::length(p - center);
    }

    glm::vec3 gradient(const glm::vec3 &p) const
    {
        return  -(p - center)/glm::length(p - center);
    }
private:
    glm::vec3 center;
    float radius;
};


//-----------------------FIELD ALGEBRA STUFF---------------------//

class ScalarFieldAdd:public Field<float>
{
public:
    ScalarFieldAdd(const Field<float> &f, const Field<float> &g)
    {
        f1 = f;
        f2 = g;
    }
    float eval(const glm::vec3 &p) const
    {
        return f1.eval(p) + f2.eval(p);
    }
private:
    Field<float>& f1;
    Field<float>& f2;
};

class ScalarFieldSub:public Field<float>
{
public:
    ScalarFieldSub(const Field<float> &f, const Field<float> &g)
    {
        f1 = f;
        f2 = g;
    }
    float eval(const glm::vec3 &p) const
    {
        return f1.eval(p) - f2.eval(p);
    }
private:
    Field<float>& f1;
    Field<float>& f2;
};

class ScalarFieldMult:public Field<float>
{
public:
    ScalarFieldMult(const Field<float> &f, const Field<float> &g)
    {
        f1 = f;
        f2 = g;
    }
    float eval(const glm::vec3 &p)
    {
        return f1.eval(p) * f2.eval(p);
    }
private:
    Field<float>& f1;
    Field<float>& f2;
};

class ScalarFieldDiv:public Field<float>
{
public:
    ScalarFieldDiv(const Field<float> &f, const Field<float> &g)
    {
        f1 = f;
        f2 = g;
    }
    float eval(const glm::vec3 &p) const
    {
        return f1.eval(p) / f2.eval(p);
    }
private:
    Field<float>& f1;
    Field<float>& f2;
};

class ScalarFieldInverse:public Field<float>
{
public:
    ScalarFieldInverse(const Field<float> &f)
    {
        f1 = f;
    }
    float eval(const glm::vec3 &p) const
    {
        return -f1.eval(p);
    }
private:
    Field<float>& f1;
}

#endif // SCALARFIELDS_H
