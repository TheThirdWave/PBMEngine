#ifndef MATRIXFIELDS_H
#define MATRIXFIELDS_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//

//-----------------------TRANSFORMS---------------------//
class MatTranslate:public Field<glm::mat3x3>
{
  public:
    MatTranslate(const Field<glm::mat3x3> &f, glm::vec3 v)
    {
        f1 = f;
        movVec = v;
    }
    glm::mat3x3 eval(const glm::vec3 &p) const
    {
        return f1.eval(p - movVec);
    }
private:
    Field<glm::mat3x3>& f1;
    glm::vec3 movVec;
};

class MatScale:public Field<glm::mat3x3>
{
public:
    MatScale(const Field<glm::mat3x3> &f, float s)
    {
        f1 = f;
        scaleFac = s;
    }
    glm::mat3x3 eval(const glm::vec3 &P) const
    {
        return f1.eval(p/s)*s*s;
    }
private:
    Field<glm::mat3x3>& f1;
    float scaleFac;
};

class MatRotate:public Field<glm::mat3x3>
{
public:
    MatRotate(const Field<glm::mat3x3> &f, glm::vec3 r)
    {
        f1 = f;
        rotAxis = glm::normalize(r);
        rotAngle = glm::length(r);
    }
    glm::mat3x3 eval(const glm::mat3x3 &P) const
    {
        float A = std::cos(rotAngle);
        float B = glm::dot(rotAxis, P) * (1 - A);
        float C = std::sin(rotAngle);
        glm::vec3 rotVec = P * A + rotAxis * B + glm::cross(rotAxis, P) * C;
        glm::mat3x3 fmat = f1.eval(glm::inverse(rotVec));
        glm::mat3x3 i = glm::mat3(1.0f);
        //"Pauli Spin Matrices"
        glm::mat3x3 t0 = glm::Mat3(0,0,0,
                                   0,0,1,
                                   0,-1,0);
        glm::mat3x3 t1 = glm::Mat3(0,0,-1,
                                   0,0,0,
                                   1,0,0);
        glm::mat3x3 t2 = glm::Mat3(0,1,0,
                                   -1,0,0,
                                   0,0,0);
        glm::mat3x3 rotMat = i * A + glm::outerProduct(rotAxis, rotAxis) * B + fmat * (C/rotAngle);
        return rotMat * fmat * glm::transpose(rotMat);
    }
private:
    Field<glm::mat3x3> f1;
    glm::vec3 rotAxis;
    float rotAngle;
};

//-----------------------FIELD ALGEBRA STUFF---------------------//

class MatrixFieldAdd:public Field<glm::mat3x3>
{
public:
    MatrixFieldAdd(const Field<glm::mat3x3> &f, const Field<glm::mat3x3> &g)
    {
        f1 = f;
        f2 = g;
    }
    glm::mat3x3 eval(const glm::vec3 &P) const
    {
        return f1.eval(P) + f2.eval(P);
    }
private:
    Field<glm::mat3x3>& f1;
    Field<glm::mat3x3>& f2;
};

class MatrixFieldSub:public Field<glm::mat3x3>
{
public:
    MatrixFieldSub(const Field<glm::mat3x3> &f, const Field<glm::mat3x3> &g)
    {
        f1 = f;
        f2 = g;
    }
    glm::mat3x3 eval(const glm::vec3 &P) const
    {
        return f1.eval(P) - f2.eval(P);
    }
private:
    Field<glm::mat3x3>& f1;
    Field<glm::mat3x3>& f2;
};

class MatrixFieldMult:public Field<glm::mat3x3>
{
public:
    MatrixFieldMult(const Field<glm::mat3x3> &f, const Field<glm::mat3x3> &g)
    {
        f1 = f;
        f2 = g;
    }
    glm::mat3x3 eval(const glm::vec3 &P) const
    {
        return f1.eval(P) * f2.eval(P);
    }
private:
    Field<glm::mat3x3>& f1;
    Field<glm::mat3x3>& f2;
};

class MatrixFieldInverse:public Field<glm::mat3x3>
{
public:
    MatrixFieldInverse(const Field<glm::mat3x3> &f)
    {
        f1 = f;
    }
    glm::mat3x3 eval(const glm::vec3 &P) const
    {
        return glm::inverse(f1.eval(P));
    }
private:
    Field<glm::mat3x3>& f1;
};

class MatrixFieldVecAdd:public Field<glm::mat3x3>
{
public:
    MatrixFieldVecAdd(const Field<glm::mat3x3> &f, const Field<glm::vec3> &g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return f2.eval(P) + f1.eval(P);
    }
private:
    Field<glm::mat3x3>& f1;
    Field<glm::vec3>& f2;
};

class MatrixFieldVecMult:public Field<glm::mat3x3>
{
public:
    MatrixFieldVecMult(const Field<glm::mat3x3> &f, const Field<glm::vec3> &g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        return f1.eval(P) * f2.eval(P);
    }
private:
    Field<glm::mat3x3>& f1;
    Field<glm::vec3>& f2;
};

class MatrixFieldVecCompMult:public Field<glm::mat3x3>
{
public:
    MatrixFieldVecCompMult(const Field<glm::mat3x3> &f, const Field<glm::vec3> &g)
    {
        f1 = f;
        f2 = g;
    }
    glm::vec3 eval(const glm::vec3 &P) const
    {
        int matSize = 3;
        float sums[3];
        glm::mat3x3* matEvalPtr = f1.eval(P);
        glm::vec3* vecEvalPtr = f2.eval(P);
        for(int j = 0; j < matSize; i++)
        {
            float hold = 0;
            for(int i = 0; i < matSize; i++)
            {
                hold += (*vecEvalPtr)[j] * (*matEvalPtr)[i][j];
            }
            sums[j] = hold;
        }
        return glm::vec3(sums[0], sums[1], sums[2]);
    }
private:
    Field<glm::mat3x3>& f1;
    Field<glm::vec3>& f2;
}

#endif // MATRIXFIELDS_H
