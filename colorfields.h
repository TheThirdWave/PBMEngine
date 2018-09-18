#ifndef COLORFIELDS_H
#define COLORFIELDS_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//
class ColorField:public Field<color>
{
public:
    ColorField(color v)
    {
        value = v;
    }

    const color eval(const glm::vec3 &p) const
    {
        return value;
    }
private:
    color value;
};

//-----------------------TRANSFORMS------------------------------//
class ColorTranslate:public Field<color>
{
  public:
    ColorTranslate(const Field<color>* f, glm::vec3 v)
    {
        f1 = f;
        movVec = v;
    }
    const color eval(const glm::vec3 &p) const override
    {
        return f1->eval(p - movVec);
    }
private:
    const Field<color>* f1;
    glm::vec3 movVec;
};

class ColorScale:public Field<color>
{
public:
    ColorScale(const Field<color>* f, float s)
    {
        f1 = f;
        scaleFac = s;
    }
    const color eval(const glm::vec3 &P) const override
    {
        return f1->eval(P/scaleFac);
    }
private:
    const Field<color>* f1;
    float scaleFac;
};

class ColorRotate:public Field<color>
{
public:
    ColorRotate(const Field<color>* f, glm::vec3 r)
    {
        f1 = f;
        rotAxis = glm::normalize(r);
        rotAngle = glm::length(r);
    }
    const color eval(const glm::vec3 &P) const override
    {
        float A = std::cos(rotAngle);
        float B = glm::dot(rotAxis, P) * (1 - A);
        float C = std::sin(rotAngle);
        glm::vec3 rotAngle = P * A + rotAxis * B + glm::cross(rotAxis, P) * C;
        return f1->eval(-rotAngle);
    }
private:
    const Field<color>* f1;
    glm::vec3 rotAxis;
    float rotAngle;
};

//-----------------------FIELD ALGEBRA STUFF---------------------//

class ColorFieldAdd:public Field<color>
{
public:
    ColorFieldAdd(const Field<color>* f, const Field<color>* g)
    {
        f1 = f;
        f2 = g;
    }
    const color eval(const glm::vec3 &P) const override
    {
        return f1->eval(P) + f2->eval(P);
    }
private:
    const Field<color>* f1;
    const Field<color>* f2;
};

class ColorFieldMult:public Field<color>
{
public:
    ColorFieldMult(const Field<color>* f, const Field<color>* g)
    {
        f1 = f;
        f2 = g;
    }
    const color eval(const glm::vec3 &P) const override
    {
        color eval1 = f1->eval(P);
        color eval2 = f2->eval(P);
        return color(eval1[0] * eval2[0], eval1[1] * eval2[1], eval1[2] * eval2[2], eval1[3] * eval2[3]);
    }
private:
    const Field<color>* f1;
    const Field<color>* f2;
};

class ColorFieldScalarMult:public Field<color>
{
public:
    ColorFieldScalarMult(const Field<color>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const color eval(const glm::vec3 &P) const
    {
        color eval1 = f1->eval(P);
        float eval2 = f2->eval(P);
        return color(eval1[0] * eval2, eval1[1] * eval2, eval1[2] * eval2, eval1[3] * eval2);
    }
private:
    const Field<color>* f1;
    const Field<float>* f2;
};

#endif // COLORFIELDS_H
