#ifndef SCALARFIELDS_H
#define SCALARFIELDS_H

#include <../glm-0.9.9.1/glm/glm.hpp>
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

    glm::vec3 grad(const glm::vec3 &p) const
    {
        return  -(p - center)/glm::length(p - center);
    }
private:
    glm::vec3 center;
    float radius;
};

//-----------------------CONSTRUCTIVE SOLID GEOMETRY------------------------------//

class ScalarIntersect:public Field<float>
{
public:
    ScalarIntersect(Field<float>& f, Field<float>& g)
    {
        f1 = f;
        f2 = g;
    }
    float eval(const glm::vec3 &P) const
    {
        return std::min(f1.eval(P), f2.eval(P));
    }
private:
    Field<float> f1;
    Field<float> f2;
};

class ScalarUnion:public Field<float>
{
public:
    ScalarUnion(Field<float>& f, Field<float>& g)
    {
        f1 = f;
        f2 = g;
    }
    float eval(const glm::vec3 &P) const
    {
        return std::max(f1.eval(P), f2.eval(P));
    }
private:
    Field<float>& f1;
    Field<float>& f2;
};

class ScalarCutout:public Field<float>
{
public:
    ScalarCutout(Field<float>& f, Field<float>& g)
    {
        f1 = f;
        f2 = g;
    }
    float eval(const glm::vec3 &P) const
    {
        return std::min(f1.eval(P), -f2.eval(P));
    }
private:
    Field<float>& f1;
    Field<float>& f2;
};

class ScalarShell:public Field<float>
{
public:
    ScalarShell(Field<float>& f, float d)
    {
        f1 = f;
        depth = d;
    }
    float eval(const glm::vec3 &P) const
    {
        return std::min(f1.eval(P) + depth, -(f1.eval(P) - depth));
    }
private:
    Field<float>& f1;
    float depth;
};

//-----------------------TRANSFORMS------------------------------//

class ScalarTranslate:public Field<float>
{
  public:
    ScalarTranslate(const Field<float> &f, glm::vec3 v)
    {
        f1 = f;
        movVec = v;
    }
    float eval(const glm::vec3 &p) const
    {
        return f1.eval(p - movVec);
    }
private:
    Field<float>& f1;
    glm::vec3 movVec;
};

class ScalarScale:public Field<float>
{
public:
    ScalarScale(const Field<float> &f, float s)
    {
        f1 = f;
        scaleFac = s;
    }
    float eval(const glm::vec3 &P) const
    {
        return f1.eval(p/s);
    }
private:
    Field<float>& f1;
    float scaleFac;
};

class ScalarRotate:public Field<float>
{
public:
    ScalarRotate(const Field<float> &f, glm::vec3 r)
    {
        f1 = f;
        rotAxis = glm::normalize(r);
        rotAngle = glm::length(r);
    }
    float eval(const glm::vec3 &P) const
    {
        float A = std::cos(rotAngle);
        float B = glm::dot(rotAxis, P) * (1 - A);
        float C = std::sin(rotAngle);
        glm::vec3 rotAngle = P * A + rotAxis * B + glm::cross(rotAxis, P) * C;
        return f1.eval(glm::inverse(rotAngle));
    }
private:
    Field<float> f1;
    glm::vec3 rotAxis;
    float rotAngle;
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
