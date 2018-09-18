#include "volumerenderer.h"

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

void volumerenderer::setTCoeff(float t)
{
    Kt = t;
}

void volumerenderer::setMarchSteps(int s)
{
    numMarchSteps = s;
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
    printf("frame data:\n");
    printf("Xc = (%f, %f, %f) ", Xc.x, Xc.y, Xc.z);
    printf("Nc = (%f, %f, %f) ", Nc.x, Nc.y, Nc.z);
    printf("Vc = (%f, %f, %f) ", Vc.x, Vc.y, Vc.z);
    printf("Uc = (%f, %f, %f)\n", Uc.x, Uc.y, Uc.z);

    //loop through pixels.
    for(int j = 0; j < Nv; j++)
    {
        for(int i = 0; i < Nu; i++)
        {
            float Ui = (-1 + 2 * ((float)i / ((float)Nu - 1.0))) * std::tan(Fc/2); //Add AA here
            float Vj = (-1 + 2 * ((float)j / ((float)Nv - 1.0))) * (std::tan(Fc/2) / ratio); //Add AA here.
            glm::vec3 Qij = Ui * Uc + Vj * Vc;
            glm::vec3 Nij = glm::normalize(Qij + Nc); //direction of ray for pixel ij.
            printf("pixData: ");
            printf("Qij = (%f, %f, %f) ", Qij.x, Qij.y, Qij.z);
            printf("Nij = (%f, %f, %f)\n", Nij.x, Nij.y, Nij.z);
            colBuf[j * Nu + i] = castRayMarch(Xc, Nij, Snear, Sfar);
        }
    }

}

color volumerenderer::castRayMarch(glm::vec3 Xc, glm::vec3 Np, float Snear, float Sfar)
{
    float stepDist = (Sfar - Snear) / numMarchSteps;
    color Lp = color(0.0f); // accumulated color
    float T = 1; // transmissivity
    for(int i = 0; i < numMarchSteps; i++)
    {
        glm::vec3 samplePoint = Xc + Np * Snear + stepDist * i * Np;// Add AA here.
        float deltaT = std::exp(-Kt*scalarFields[0].eval(samplePoint));
        printf("marchData: ");
        printf("samplePoint = (%f, %f, %f)\n", samplePoint.x, samplePoint.y, samplePoint.z);
        Lp += colorFields[0].eval(samplePoint) * (1 - deltaT) * T;
        T *= deltaT;
    }
    Lp.a = 1-T;
    return Lp;
}
