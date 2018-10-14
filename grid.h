#ifndef GRID_H
#define GRID_H

#include <../glm-0.9.9.1/glm/glm.hpp>
#include "structpile.h"
#include "field.h"

template < class T >
class Grid
{
public:
    Grid(glm::vec3 L, glm::vec3 U, int nx, int ny, int nz)
    {
        LLC = L;
        URC = U;
        Nx = nx;
        Ny = ny;
        Nz = nz;
        dx = (URC.x - LLC.x) / Nx;
        dy = (URC.y - LLC.y) / Ny;
        dz = (URC.z - LLC.z) / Nz;
        data = new T[Nx * Ny * Nz];
        memset(data, 0, sizeof(T) * Nx * Ny * Nz);
    }

    void setLLC(glm::vec3 l)
    {
        LLC = l;
    }

    void setURC(glm::vec3 u)
    {
        URC = u;
    }

    void setData(T* d)
    {
        data = d;
    }

    void setDataAt(int index, T d)
    {
        data[index] = d;
        //printf("data[%d]: %f\n", data[index]);
    }

    int getIndex(int i, int j, int k)
    {
        return i + Nx * j + Nx * Ny * k;
    }

    glm::vec3 getIndexPos(int i, int j, int k)
    {
        return LLC + glm::vec3(i * dx, j * dy, k * dz);
    }

    glm::vec3 getLLC()
    {
        return LLC;
    }

    glm::vec3 getURC()
    {
        return URC;
    }

    T* getData()
    {
        return data;
    }

    void getDimensions(int* arr)
    {
        arr[0] = Nx;
        arr[1] = Ny;
        arr[2] = Nz;
    } //expects an int array of length 3 : int[3]

    void stampField(Field<T>* f)
    {
        for(int k = 0; k < Nz; k++)
        {
            for(int j = 0; j < Ny; j++)
            {
                for(int i = 0; i < Nx; i++)
                {
                    int idx = getIndex(i, j, k);
                    setDataAt(idx, f->eval(getIndexPos(i, j, k)));
                }
            }
        }
    }

    void stampNoise(Field<T>* f, mparticle* p)
    {
        for(int k = 0; k < Nz; k++)
        {
            for(int j = 0; j < Ny; j++)
            {
#pragma omp parallel for
                for(int i = 0; i < Nx; i++)
                {
                    int idx = getIndex(i, j, k);
                    glm::vec3 x = getIndexPos(i, j, k);
                    float nv = std::abs(f->eval(x));
                    float len = glm::length(x - p->pos)/p->pscale;
                    float ff;
                    if(len < 1) ff = 1 - len;
                    else ff = 0;
                    nv *= std::pow(ff, p->fade);

                    if(nv > data[idx]) setDataAt(idx, nv);
                }
            }
        }
    }

    T trilerp(const glm::vec3& x)
    {
        glm::vec3 Xg = x - LLC;
        int i = Xg.x / dx;
        int j = Xg.y / dy;
        int k = Xg.z / dz;
        float u = Xg.x - i * dx;
        float v = Xg.y - j * dy;
        float w = Xg.z - k * dz;
        T c000, c001, c010, c011, c100, c101, c110, c111, c00, c01, c10, c11, c0, c1;
        int index = getIndex(i, j, k);
        c000 = data[index];
        index = getIndex(i, j, k + 1);
        c001 = data[index];
        index = getIndex(i, j + 1, k);
        c010 = data[index];
        index = getIndex(i, j + 1, k + 1);
        c011 = data[index];
        index = getIndex(i + 1, j, k);
        c100 = data[getIndex(i + 1, j, k)];
        index = getIndex(i + 1, j, k + 1);
        c101 = data[getIndex(i + 1, j, k + 1)];
        index = getIndex(i + 1, j + 1, k);
        c110 = data[index];
        index = getIndex(i + 1, j + 1, k + 1);
        c111 = data[index];
        c00 = c000 * (1 - u) + c100 * u;
        c01 = c001 * (1 - u) + c101 * u;
        c10 = c010 * (1 - u) + c110 * u;
        c11 = c011 * (1 - u) + c111 * u;
        c0 = c00 * (1 - v) + c10 * v;
        c1 = c01 * (1 - v) + c11 * v;
        return c0 * (1 - w) + c1 * w;
    }

private:
    glm::vec3 LLC;
    glm::vec3 URC;
    int Nx, Ny, Nz;
    float dx, dy, dz;
    T* data;
};

#endif // GRID_H
