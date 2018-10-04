#include "volumerenderer.h"
#include <random>

volumerenderer::volumerenderer()
{

}

void volumerenderer::initColBuf(int w, int h)
{
    colBuf = new color[w * h];
}

void volumerenderer::passToDisplay()
{
    float* b = disp->getBuf();
    int width = disp->getWidth();
    int height = disp->getHeight();
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            int index = (j * width + i) * disp->getNumChannels();
            b[index] = colBuf[j * width + i].r;
            b[index + 1] = colBuf[j * width + i].g;
            b[index + 2] = colBuf[j * width + i].b;
            b[index + 3] = colBuf[j * width + i].a;
        }
    }
}

void volumerenderer::setCamera(camera *c)
{
    cam = c;
}

void volumerenderer::setDisplayBuf(Buffer2D *d)
{
    disp = d;
    initColBuf(d->getWidth(), d->getHeight());
}

void volumerenderer::setScalarFields(const Field<float> *fields, int len)
{
    scalarFields = fields;
    numSFields = len;
}

void volumerenderer::setColorFields(const Field<color> *fields, int len)
{
    colorFields = fields;
    numCFields = len;
}

void volumerenderer::setLights(VolumeLight *l, int len)
{
    lights = l;
    numLights = len;
}

void volumerenderer::setTCoeff(float t)
{
    Kt = t;
}

void volumerenderer::setAA(int aa)
{
    AA = aa;
}

void volumerenderer::setMarchSize(float s)
{
    marchSize = s;
}

void volumerenderer::setBoundingBox(bbox *b)
{
    boundingBox = b;
}

void volumerenderer::renderFrame()
{
    glm::vec3 Xc = cam->getPos();
    glm::vec3 Nc = glm::normalize(cam->getLookDir());
    glm::vec3 Vc = glm::normalize(cam->getUpDir());
    glm::vec3 Uc = glm::cross(Vc, Nc);
    float Fc = cam->getFOV();
    float Snear = cam->getNear();
    float Sfar = cam->getFar();
    int Nu = disp->getWidth();
    int Nv = disp->getHeight();
    float ratio = (float)Nu/(float)Nv;
    std::random_device rd1{};
    std::random_device rd2{};
    std::mt19937 gen1(rd1());
    std::mt19937 gen2(rd2());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    //loop through pixels.
    for(int j = 0; j < Nv; j++)
    {
#pragma omp parallel for
        for(int i = 0; i < Nu; i++)
        {
            float Ui = (-1 + 2 * (((float)i + dis(gen1)) / ((float)Nu - 1.0))) * std::tan(Fc/2); //dis(gen) for AA
            float Vj = (-1 + 2 * (((float)j + dis(gen2)) / ((float)Nv - 1.0))) * (std::tan(Fc/2) / ratio); //dis(gen) for AA
            glm::vec3 Qij = Ui * Uc + Vj * Vc;
            glm::vec3 Nij = glm::normalize(Qij + Nc); //direction of ray for pixel ij.
            colBuf[j * Nu + i] = castRayMarch(Xc, Nij, Snear, Sfar, dis(gen1));
        }
        printf("Percentage Frame Done: %f\n", (j * (float)Nu) / ((float)Nu * Nv));
    }

}

color volumerenderer::castRayMarch(glm::vec3 Xc, glm::vec3 Np, float Snear, float Sfar, float rand)
{
    isect hits;
    color Lp = color(0.0f); // accumulated color
    float T = 1; // transmissivity
    if(checkBoundingBox(Xc, Np, boundingBox, Snear, Sfar, hits))
    {
        int numSteps = (hits.t1 - hits.t0) / marchSize;
        for(int i = 0; i < numSteps; i++)
        {
            glm::vec3 samplePoint = Xc + Np * hits.t0 + marchSize * (i + rand) * Np;//rand for AA
            float deltaT = std::exp(-Kt * marchSize * scalarFields[0].eval(samplePoint));
            if(deltaT != 1)
            {
                int x = 0;
            }
            Lp += calculateLights(samplePoint) * (1 - deltaT) * T;
            T *= deltaT;
        }
    }
    Lp.a = 1-T;
    return Lp;
}

color volumerenderer::calculateLights(glm::vec3 Xc)
{
    color fColor = color(color(0.0f, 0.0f, 0.0f, 0.0f));
    float Tl;
    for(int i = 0; i < numLights; i++)
    {
        //glm::vec3 Ll = lights[i].getPos() - Xc;
        //glm::vec3 Nl = glm::normalize(Ll);
        //float Sl = glm::length(Ll);
        //Tl = std::exp(-Kt * lightMarch(Xc, Nl, Sl));
        Tl = std::exp(-Kt * lights[i].evalDSM(Xc));
        fColor += lights[i].getColor() * Tl;
    }
    return fColor * colorFields[0].eval(Xc);
}

float volumerenderer::lightMarch(glm::vec3 Xc, glm::vec3 Nl, float Sl)
{
    int nSteps = Sl / marchSize;
    float dsm = 0;
    glm::vec3 x;
    for(int i = 0; i < nSteps; i++)
    {
        x = Xc + Nl * marchSize * (float)i;
        float rho = scalarFields[0].eval(x);
        if(rho <= 0) break;
        dsm += rho * marchSize;
    }
    return dsm;
}

bool volumerenderer::checkBoundingBox(glm::vec3& Xc, glm::vec3& Np, bbox* b, float Snear, float Sfar, isect &hitPoints)
{
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    float divx = 1 / Np.x;
    if(divx >= 0)
    {
        tmin = (b->LLC.x - Xc.x) * divx;
        tmax = (b->URC.x - Xc.x) * divx;
    }
    else
    {
        tmin = (b->URC.x - Xc.x) * divx;
        tmax = (b->LLC.x - Xc.x) * divx;
    }
    float divy = 1 / Np.y;
    if(divy >= 0)
    {
        tymin = (b->LLC.y - Xc.y) * divy;
        tymax = (b->URC.y - Xc.y) * divy;
    }
    else
    {
        tymin = (b->URC.y - Xc.y) * divy;
        tymax = (b->LLC.y - Xc.y) * divy;
    }
    if((tmin > tymax) || (tymin > tmax)) return false;
    if(tymin > tmin) tmin = tymin;
    if(tymax < tmax) tmax = tymax;
    float divz = 1 / Np.z;
    if(divz >= 0)
    {
        tzmin = (b->LLC.z - Xc.z) * divz;
        tzmax = (b->URC.z - Xc.z) * divz;
    }
    else
    {
        tzmin = (b->URC.z - Xc.z) * divz;
        tzmax = (b->LLC.z - Xc.z) * divz;
    }
    if((tmin > tzmax) || (tmin > tmax)) return false;
    if(tzmin > tmin) tmin = tzmin;
    if(tzmax < tmax) tmax = tzmax;
    if(!((tmin < Sfar) && (tmax > Snear))) return false;
    hitPoints.t0 = tmin;
    hitPoints.t1 = tmax;
    return true;
}

void volumerenderer::calcDSM(Grid<float>& g, glm::vec3 pos)
{
    int arr[3];
    g.getDimensions(arr);
    int Nx = arr[0];
    int Ny = arr[1];
    int Nz = arr[2];
    for(int k = 0; k < Nz; k++)
    {
        for(int j = 0; j < Ny; j++)
        {
            #pragma omp parallel for
            for(int i = 0; i < Nx; i++)
            {
                glm::vec3 x = g.getIndexPos(i, j, k);
                glm::vec3 Li = pos - x;
                glm::vec3 Ni = glm::normalize(Li);
                float Si = glm::length(Li);
                float rhoish = lightMarch(x, Ni, Si);
                g.setDataAt(g.getIndex(i, j, k), rhoish);
            }
        }
        printf("Percentage DSM Done: %f\n", (k * (float)Nx * (float)Ny) / ((float)Nx * Ny * Nz));
    }
}


















