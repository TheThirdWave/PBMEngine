#ifndef COLORFIELDS_H
#define COLORFIELDS_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//

//-----------------------TRANSFORMS------------------------------//
class ScalarTranslate:public Field<color>
{
  public:
    ScalarTranslate(const Field<color> &f, glm::vec3 v)
    {
        f1 = f;
        movVec = v;
    }
    color eval(const glm::vec3 &p) const
    {
        return f1.eval(p - movVec);
    }
private:
    Field<color>& f1;
    glm::vec3 movVec;
};

class ScalarScale:public Field<color>
{
public:
    ScalarScale(const Field<color> &f, color s)
    {
        f1 = f;
        scaleFac = s;
    }
    color eval(const glm::vec3 &P) const
    {
        return f1.eval(p/s);
    }
private:
    Field<color>& f1;
    float scaleFac;
};

class ScalarRotate:public Field<color>
{
public:
    ScalarRotate(const Field<color> &f, glm::vec3 r)
    {
        f1 = f;
        rotAxis = glm::normalize(r);
        rotAngle = glm::length(r);
    }
    color eval(const glm::vec3 &P) const
    {
        float A = std::cos(rotAngle);
        float B = glm::dot(rotAxis, P) * (1 - A);
        float C = std::sin(rotAngle);
        glm::vec3 rotAngle = P * A + rotAxis * B + glm::cross(rotAxis, P) * C;
        return f1.eval(glm::inverse(rotAngle));
    }
private:
    Field<color> f1;
    glm::vec3 rotAxis;
    float rotAngle;
};

//-----------------------FIELD ALGEBRA STUFF---------------------//

class ColorFieldAdd:public Field<color>
{
public:
    ColorFieldAdd(const Field<color>& f, const Field<color>& g)
    {
        f1 = f;
        f2 = g;
    }
    color eval(const glm::vec3 &P) const
    {
        return f1.eval(p) + f2.eval(p);
    }
private:
    Field<color>& f1;
    Field<color>& f2;
};

class ColorFieldMult:public Field<color>
{
public:
    ColorFieldMult(const Field<color>& f, const Field<color>& g)
    {
        f1 = f;
        f2 = g;
    }
    color eval(const glm::vec3 &P) const
    {
        color eval1 = f1.eval(P);
        color eval2 = f2.eval(P);
        return color(eval1[0] * eval2[0], eval1[1] * eval2[1], eval1[2] * eval2[2], eval1[3] * eval2[3]);
    }
private:
    Field<color>& f1;
    Field<color>& f2;
};

class ColorFieldScalarMult:public Field<color>
{
public:
    ColorFieldMult(const Field<color>& f, const Field<float>& g)
    {
        f1 = f;
        f2 = g;
    }
    color eval(const glm::vec3 &P) const
    {
        color eval1 = f1.eval(P);
        float eval2 = f2.eval(P);
        return color(eval1[0] * eval2, eval1[1] * eval2, eval1[2] * eval2, eval1[3] * eval2);
    }
private:
    Field<color>& f1;
    Field<float>& f2;
};

#endif // COLORFIELDS_H
