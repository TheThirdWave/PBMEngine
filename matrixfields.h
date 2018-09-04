#ifndef MATRIXFIELDS_H
#define MATRIXFIELDS_H

#include <../glm-0.9.8.5/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//

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

#endif // MATRIXFIELDS_H
