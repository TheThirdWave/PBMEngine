#ifndef SCALARFIELDS_H
#define SCALARFIELDS_H

#include <algorithm>
#include <libnoise/noise.h>
#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "grid.h"
#include "field.h"

//-----------------------IMPLICIT SHAPES---------------------//

class ScalarField:public Field<float>
{
public:
    ScalarField(float v)
    {
        value = v;
    }

    const float eval(const glm::vec3 &p) const override
    {
        return value;
    }

private:
    float value;
};

class ScalarSphere:public Field<float>
{
public:
    ScalarSphere(glm::vec3 c, float r)
    {
       radius = r;
       center = c;
    }

    const float eval(const glm::vec3 &p) const override
    {
        return radius - glm::length(p - center);
    }

    const glm::vec3 grad(const glm::vec3 &p) const override
    {
        return  -(p - center)/glm::length(p - center);
    }
private:
    glm::vec3 center;
    float radius;
};

class ScalarEllipse:public Field<float>
{
public:
    ScalarEllipse(glm::vec3 c, glm::vec3 n, float rmin, float rmax)
    {
        minRadius = rmin;
        maxRadius = rmax;
        normal = glm::normalize(n);
        center = c;
    }

    const float eval(const glm::vec3 &p) const override
    {
        glm::vec3 x = p - center;
        float z = glm::dot(x, normal);
        glm::vec3 xperp = x - z*normal;
        return 1 - (z * z)/(maxRadius*maxRadius) - (glm::length(xperp) * glm::length(xperp))/(minRadius * minRadius);
    }

private:
    glm::vec3 center;
    glm::vec3 normal;
    float minRadius;
    float maxRadius;
};

class ScalarTorus:public Field<float>
{
public:
    ScalarTorus(glm::vec3 c, glm::vec3 n, float rmin, float rmax)
    {
       minRadius = rmin;
       maxRadius = rmax;
       normal = glm::normalize(n);
       center = c;
    }

    const float eval(const glm::vec3 &p) const
    {
        glm::vec3 x = p - center;
        glm::vec3 xperp = x - glm::dot(x, normal) * normal;
        float xlen = glm::length(x);
        float xplen = glm::length(xperp);
        return 4*maxRadius*maxRadius*xplen*xplen - std::pow((xlen*xlen + maxRadius*maxRadius-minRadius*minRadius), 2);
    }

private:
    glm::vec3 center;
    glm::vec3 normal;
    float minRadius;
    float maxRadius;
};

class ScalarCube:public Field<float>
{
public:
    ScalarCube(glm::vec3 c, float r, int s)
    {
       radius = r;
       center = c;
       sharpness = s;
    }

    const float eval(const glm::vec3 &p) const
    {
        glm::vec3 x = p - center;
        return std::pow(radius, sharpness) - std::pow(x.x, sharpness) - std::pow(x.y, sharpness) - std::pow(x.z, sharpness);
    }

private:
    glm::vec3 center;
    float radius;
    int sharpness;
};

class ScalarPlane:public Field<float>
{
public:
    ScalarPlane(glm::vec3 c, glm::vec3 n)
    {
       normal = glm::normalize(n);
       center = c;
    }

    const float eval(const glm::vec3 &p) const
    {
        return glm::dot(-(p - center), normal);
    }

private:
    glm::vec3 center;
    glm::vec3 normal;
};

class ScalarCone:public Field<float>
{
public:
    ScalarCone(glm::vec3 c, glm::vec3 n, float h, float mt)
    {
       height = h;
       normal = glm::normalize(n);
       center = c;
       maxTheta = mt;
    }

    const float eval(const glm::vec3 &p) const
    {
        float pheight = glm::dot((p - center), normal);
        if(p == center) return 0;
        else if(pheight > height) return height - pheight;
        else if(pheight < 0) return pheight;
        else
        {
            return pheight - glm::length(p) * std::cos(maxTheta);
        }
    }

private:
    glm::vec3 center;
    glm::vec3 normal;
    float height;
    float maxTheta;
};

class ScalarInfCylinder:public Field<float>
{
public:
    ScalarInfCylinder(glm::vec3 c, glm::vec3 n, float r)
    {
       radius = r;
       normal = glm::normalize(n);
       center = c;
    }

    const float eval(const glm::vec3 &p) const
    {
        glm::vec3 xperp = p - glm::dot(p, normal) * normal;
        return radius - glm::length(xperp);
    }

private:
    glm::vec3 center;
    glm::vec3 normal;
    float radius;
};

class ScalarIcosahedron:public Field<float>
{
public:
    ScalarIcosahedron(glm::vec3 c, float r)
    {
       radius = r;
       center = c;
    }

    const float eval(const glm::vec3 &p) const
    {
        glm::vec3 x = p - center;
        float len = glm::length(x);
        if(len > 1.8*PI) return -1.8*PI;
        else{
            return std::cos(x.x + GOLDR * x.y) + std::cos(x.x - GOLDR * x.y) + std::cos(x.y + GOLDR * x.z) + std::cos(x.y - GOLDR * x.z) + std::cos(x.z - GOLDR * x.x) + std::cos(x.z + GOLDR * x.x) - 2.0;
        }
    }

private:
    glm::vec3 center;
    float radius;
};

class ScalarSteinerPatch:public Field<float>
{
public:
    ScalarSteinerPatch(glm::vec3 c)
    {
        center = c;
    }

    const float eval(const glm::vec3 &p) const
    {
        glm::vec3 x = p - center;
        return -((x.x*x.x)*(x.y*x.y) + (x.x*x.x)*(x.z*x.z) + (x.y*x.y)*(x.z*x.z) - (x.x*x.y*x.z));
    }

private:
    glm::vec3 center;
};

class ScalarGrid:public Field<float>
{
public:
    ScalarGrid(Grid<float>* g, float def)
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

    const float eval(const glm::vec3 &P) const
    {
        glm::vec3 Xg = P - LLC;
        int i = Xg.x / dx;
        int j = Xg.y / dy;
        int k = Xg.z / dz;
        if(i < 0 || i >= Nx - 1 || j < 0 || j >= Ny - 1 || k < 0 || k >= Nz - 1) return defaultVal;
        return grid->trilerp(P);
    }

    void setGrid(Grid<float>* g)
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
private:
    glm::vec3 LLC;
    glm::vec3 URC;
    int Nx;
    int Ny;
    int Nz;
    float dx;
    float dy;
    float dz;
    Grid<float>* grid;
    float defaultVal;
};

class ScalarPyro:public Field<float>
{
public:
    ScalarPyro(const Field<float>* f, double freq, double lac, double per, float amp, float gam, int oct)
    {
        f1 = f;
        amplitude = amp;
        gamma = gam;
        perlin.SetFrequency(freq);
        perlin.SetLacunarity(lac);
        perlin.SetPersistence(per);
        perlin.SetOctaveCount(oct);
    }

    const float eval(const glm::vec3 &P) const
    {
        float dist = f1->eval(P); //STUPID ASSUMPTION: assumes f1 is an SDF.
        glm::vec3 surface = P + (-dist * f1->grad(P));
        float N = amplitude * std::pow(std::abs(perlin.GetValue(surface.x, surface.y, surface.z)), gamma);
        if(-N <= dist)
        {
            return 1;
        }
        else return 0;
    }

    void setAmplitude(float amp)
    {
        amplitude = amp;
    }

    void setGammap(float gam)
    {
        gamma = gam;
    }

    void setFrequency(double freq)
    {
        perlin.SetFrequency(freq);
    }

    void setLacunarity(double lac)
    {
        perlin.SetLacunarity(lac);
    }

    void setNoiseQuality(noise::NoiseQuality nQ)
    {
        perlin.SetNoiseQuality(nQ);
    }

    void setOctaveCount(int oct)
    {
        perlin.SetOctaveCount(oct);
    }

    void setPersistence(double per)
    {
        perlin.SetPersistence(per);
    }

    void setSeed(int seed)
    {
        perlin.SetSeed(seed);
    }

private:
    noise::module::Perlin perlin;
    const Field<float>* f1;
    float amplitude;
    float gamma;
};

class ScalarFSPN:public Field<float>
{
public:
    ScalarFSPN(glm::vec3 trans, double freq, double lac, double per, int oct)
    {
        translate = trans;
        perlin.SetFrequency(freq);
        perlin.SetLacunarity(lac);
        perlin.SetPersistence(per);
        perlin.SetOctaveCount(oct);
    }

    const float eval(const glm::vec3 &P) const
    {
        glm::vec3 pos = P + translate;
        float ret = perlin.GetValue(pos.x, pos.y, pos.z);
        if (ret > 1) ret = 1;
        else if (ret < -1) ret = -1;
        return ret;
    }

    void setTranslate(glm::vec3 trans)
    {
        translate = trans;
    }

    void setFrequency(double freq)
    {
        perlin.SetFrequency(freq);
    }

    void setLacunarity(double lac)
    {
        perlin.SetLacunarity(lac);
    }

    void setNoiseQuality(noise::NoiseQuality nQ)
    {
        perlin.SetNoiseQuality(nQ);
    }

    void setOctaveCount(int oct)
    {
        perlin.SetOctaveCount(oct);
    }

    void setPersistence(double per)
    {
        perlin.SetPersistence(per);
    }

    void setSeed(int seed)
    {
        perlin.SetSeed(seed);
    }

private:
    noise::module::Perlin perlin;
    glm::vec3 translate;
};

//-----------------------CONSTRUCTIVE SOLID GEOMETRY------------------------------//

class ScalarIntersect:public Field<float>
{
public:
    ScalarIntersect(const Field<float>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const float eval(const glm::vec3 &P) const
    {
        return std::min(f1->eval(P), f2->eval(P));
    }
private:
    const Field<float>* f1;
    const Field<float>* f2;
};

class ScalarUnion:public Field<float>
{
public:
    ScalarUnion(const Field<float>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const float eval(const glm::vec3 &P) const
    {
        return std::max(f1->eval(P), f2->eval(P));
    }
private:
    const Field<float>* f1;
    const Field<float>* f2;
};

class ScalarCutout:public Field<float>
{
public:
    ScalarCutout(const Field<float>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const float eval(const glm::vec3 &P) const
    {
        return std::min(f1->eval(P), -f2->eval(P));
    }
private:
    const Field<float>* f1;
    const Field<float>* f2;
};

class ScalarShell:public Field<float>
{
public:
    ScalarShell(const Field<float>* f, float d)
    {
        f1 = f;
        depth = d;
    }
    const float eval(const glm::vec3 &P) const
    {
        return std::min(f1->eval(P) + depth, -(f1->eval(P) - depth));
    }
private:
    const Field<float>* f1;
    float depth;
};

class ScalarMask:public Field<float>
{
public:
    ScalarMask(const Field<float>* f)
    {
        f1 = f;
    }

    const float eval(const glm::vec3 &P) const
    {
        float ret = f1->eval(P);
        if(ret > 0) return 1;
        else return 0;
    }
private:
    const Field<float>* f1;
};

class ScalarClamp:public Field<float>
{
public:
    ScalarClamp(const Field<float>* f, float a, float b)
    {
        f1 = f;
        min = a;
        max = b;
    }

    const float eval(const glm::vec3 &P) const
    {
        float ret = f1->eval(P);
        if(ret <= min) return min;
        else if(ret > max) return max;
        else return ret;
    }
private:
    const Field<float>* f1;
    float min;
    float max;
};

//-----------------------TRANSFORMS------------------------------//

class ScalarTranslate:public Field<float>
{
  public:
    ScalarTranslate(const Field<float>* f, glm::vec3 v)
    {
        f1 = f;
        movVec = v;
    }
    const float eval(const glm::vec3 &p) const
    {
        return f1->eval(p - movVec);
    }
private:
    const Field<float>* f1;
    glm::vec3 movVec;
};


class ScalarScale:public Field<float>
{
public:
    ScalarScale(const Field<float>* f, float s)
    {
        f1 = f;
        scaleFac = s;
    }
    const float eval(const glm::vec3 &P) const
    {
        return f1->eval(P/scaleFac);
    }
private:
    const Field<float>* f1;
    float scaleFac;
};

class ScalarRotate:public Field<float>
{
public:
    ScalarRotate(const Field<float>* f, glm::vec3 r)
    {
        f1 = f;
        rotAxis = glm::normalize(r);
        rotAngle = glm::length(r);
    }
    const float eval(const glm::vec3 &P) const
    {
        if(rotAngle != 0)
        {
            float A = std::cos(rotAngle);
            float B = glm::dot(rotAxis, P) * (1 - A);
            float C = std::sin(rotAngle);
            glm::vec3 rotAngle = P * A + rotAxis * B + glm::cross(rotAxis, P) * C;
            return f1->eval(-rotAngle);
        }
        else return f1->eval(P);
    }
private:
    const Field<float>* f1;
    glm::vec3 rotAxis;
    float rotAngle;
};

class ScalarXYZRotate:public Field<float>
{
public:
    ScalarXYZRotate(const Field<float>* f, float x, float y, float z)
    {
        f1 = f;
        rx = new ScalarRotate(f1, glm::vec3(x * ((float)PI / 180), 0.0f, 0.0f));
        ry = new ScalarRotate(rx, glm::vec3(0.0f, y * ((float)PI / 180), 0.0f));
        rz = new ScalarRotate(ry, glm::vec3(0.0f, 0.0f, z * ((float)PI / 180)));
    }

    const float eval(const glm::vec3 &P) const
    {
        return rz->eval(P);
    }
private:
    const Field<float>* f1;
    ScalarRotate* rx;
    ScalarRotate* ry;
    ScalarRotate* rz;
};

//-----------------------FIELD ALGEBRA STUFF---------------------//

class ScalarFieldAdd:public Field<float>
{
public:
    ScalarFieldAdd(const Field<float>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const float eval(const glm::vec3 &p) const
    {
        return f1->eval(p) + f2->eval(p);
    }
private:
    const Field<float>* f1;
    const Field<float>* f2;
};

class ScalarFieldSub:public Field<float>
{
public:
    ScalarFieldSub(const Field<float>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const float eval(const glm::vec3 &p) const
    {
        return f1->eval(p) - f2->eval(p);
    }
private:
    const Field<float>* f1;
    const Field<float>* f2;
};

class ScalarFieldMult:public Field<float>
{
public:
    ScalarFieldMult(const Field<float>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const float eval(const glm::vec3 &p)
    {
        return f1->eval(p) * f2->eval(p);
    }
private:
    const Field<float>* f1;
    const Field<float>* f2;
};

class ScalarFieldDiv:public Field<float>
{
public:
    ScalarFieldDiv(const Field<float>* f, const Field<float>* g)
    {
        f1 = f;
        f2 = g;
    }
    const float eval(const glm::vec3 &p) const
    {
        return f1->eval(p) / f2->eval(p);
    }
private:
    const Field<float>* f1;
    const Field<float>* f2;
};

class ScalarFieldInverse:public Field<float>
{
public:
    ScalarFieldInverse(const Field<float>* f)
    {
        f1 = f;
    }
    const float eval(const glm::vec3 &p) const
    {
        return -f1->eval(p);
    }
private:
    const Field<float>* f1;
};

//-----------------------OVERRIDES---------------------//

inline const ScalarFieldAdd operator+ (const Field<float>& a, const Field<float>& b)
{
    return ScalarFieldAdd(&a, &b);
}

inline const ScalarFieldSub operator- (const Field<float>& a, const Field<float>& b)
{
    return ScalarFieldSub(&a, &b);
}

inline const ScalarFieldInverse operator- (const Field<float>& a)
{
    return ScalarFieldInverse(&a);
}

inline const ScalarFieldMult operator* (const Field<float>& a, const Field<float>& b)
{
    return ScalarFieldMult(&a, &b);
}

inline const ScalarFieldDiv operator/ (const Field<float>& a, const Field<float>& b)
{
    return ScalarFieldDiv(&a, &b);
}






































#endif // SCALARFIELDS_H
