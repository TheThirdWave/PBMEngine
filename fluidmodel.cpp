#include "fluidmodel.h"

FluidModel::FluidModel()
{

}

FluidModel::FluidModel(Buffer2D *initBuf, Buffer2D* s)
{
    density.init(initBuf->getWidth(), initBuf->getHeight(), 1);
    velocity.init(initBuf->getWidth(), initBuf->getHeight(), 2);
    pressure.init(initBuf->getWidth(), initBuf->getHeight(), 1);
    source = s;
    color = initBuf;
    hasSource = false;
    gravity = 10.0;
}

void FluidModel::init(Buffer2D *initBuf, Buffer2D* s)
{
    density.init(initBuf->getWidth(), initBuf->getHeight(), 1);
    velocity.init(initBuf->getWidth(), initBuf->getHeight(), 2);
    pressure.init(initBuf->getWidth(), initBuf->getHeight(), 1);
    source = s;
    color = initBuf;
    hasSource = false;
    gravity = 10.0;
}

void FluidModel::runSLTimeStep(double timeStep)
{
    advection(timeStep);
    if(hasSource)
    {
        sources(timeStep);
        source->setDataFloat(0.0f);
        hasSource = false;
    }
    forces(timeStep);
}

void FluidModel::advection(double timeStep)
{
    int width = density.getWidth();
    int height = density.getHeight();
    float* velGrid = velocity.getBuf();
    float* denGrid = density.getBuf();
    float* colGrid = color->getBuf();
    glm::vec2 iVec;
    glm::vec2 vel;
    glm::vec3 col;
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            //make everything a vector because I want to.
            iVec.x = (float)i;
            iVec.y = (float)j;
            int index = i + j * width;

            //get the velocity at the point we're looping over so we know the point to advect from.
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];

            //subtract the velocity from the position of the point to get the point to advect from.
            iVec = iVec - vel;
            if(glm::length(vel) != 0)
            {
                int x = 1;
            }

            //update the density and velocity from the point of advection.
            denGrid[index * density.getNumChannels()] = interpolateF(&density, iVec);
            vel = interpolate2Vec(&velocity, iVec);
            velGrid[index * velocity.getNumChannels()] = vel.x;
            velGrid[index * velocity.getNumChannels() + 1] = vel.y;
            col = interpolate3Vec(color, iVec);
            colGrid[index * color->getNumChannels()] = col.r;
            colGrid[index * color->getNumChannels() + 1] = col.g;
            colGrid[index * color->getNumChannels() + 2] = col.b;
        }
    }
}

void FluidModel::forces(double timeStep)
{
    int width = velocity.getWidth();
    int height = velocity.getHeight();
    float* denGrid = density.getBuf();
    float* velGrid = velocity.getBuf();
    glm::vec2 vel;
    glm::vec2 up = glm::vec2(0.0f, -1.0f);
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            int index = i + j * width;
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];
            if(denGrid[index * density.getNumChannels()] > 0.0f)
            {
                int x = 1;
            }
            vel += (-denGrid[index * density.getNumChannels()] * up * gravity) * (float)timeStep;
            velGrid[index * velocity.getNumChannels()] = vel.x;
            velGrid[index * velocity.getNumChannels() + 1] = vel.y;
            int x = 1;
        }
    }
}

void FluidModel::sources(double timeStep)
{
    int width = source->getWidth();
    int height = source->getHeight();
    float* denGrid = density.getBuf();
    float* sGrid = source->getBuf();
    glm::vec2 iVec;
    glm::vec2 vel;
    glm::vec3 col;
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            int index = i + j * width;
            denGrid[index * density.getNumChannels()] += sGrid[index * source->getNumChannels()];
        }
    }
}

glm::vec2 FluidModel::interpolate2Vec(Buffer2D* buf, glm::vec2 vec)
{
    //it's a binlinear interpolation.
    float* rawBuf = buf->getBuf();
    int x1 = (int)vec.x;
    int y1 = (int)vec.y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    int idx = (x1 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d0;
    d0.x = rawBuf[idx];
    d0.y = rawBuf[idx + 1];
    idx = (x1 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d1;
    d1.x = rawBuf[idx];
    d1.y = rawBuf[idx + 1];
    idx = (x2 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d2;
    d2.x = rawBuf[idx];
    d2.y = rawBuf[idx + 1];
    idx = (x2 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d3;
    d3.x = rawBuf[idx];
    d3.y = rawBuf[idx + 1];
    glm::vec2 fx1 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d0 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d2;
    glm::vec2 fx2 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d1 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d3;
    glm::vec2 fF = (((float)y2 - vec.y)/((float)y2 - (float)y1)) * fx1 + ((vec.y - (float)y1)/((float)y2 - (float)y1)) * fx2;
    return fF;
}

glm::vec3 FluidModel::interpolate3Vec(Buffer2D* buf, glm::vec2 vec)
{
    //it's a binlinear interpolation.
    float* rawBuf = buf->getBuf();
    int x1 = (int)vec.x;
    int y1 = (int)vec.y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    int idx = (x1 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d0;
    d0.x = rawBuf[idx];
    d0.y = rawBuf[idx + 1];
    d0.z = rawBuf[idx + 2];
    idx = (x1 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d1;
    d1.x = rawBuf[idx];
    d1.y = rawBuf[idx + 1];
    d1.z = rawBuf[idx + 2];
    idx = (x2 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d2;
    d2.x = rawBuf[idx];
    d2.y = rawBuf[idx + 1];
    d2.z = rawBuf[idx + 2];
    idx = (x2 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d3;
    d3.x = rawBuf[idx];
    d3.y = rawBuf[idx + 1];
    d3.z = rawBuf[idx + 2];
    glm::vec3 fx1 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d0 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d2;
    glm::vec3 fx2 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d1 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d3;
    glm::vec3 fF = (((float)y2 - vec.y)/((float)y2 - (float)y1)) * fx1 + ((vec.y - (float)y1)/((float)y2 - (float)y1)) * fx2;
    return fF;
}

void FluidModel::setGravity(float f)
{
    gravity = f;
}

void FluidModel::setHasSource(bool t)
{
    hasSource = t;
}

float FluidModel::interpolateF(Buffer2D* buf, glm::vec2 vec)
{
    //it's a binlinear interpolation.
    float* rawBuf = buf->getBuf();
    int x1 = (int)vec.x;
    int y1 = (int)vec.y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    int idx = x1 + y1 * buf->getWidth();
    float d0 = rawBuf[idx];
    idx = x1 + y2 * buf->getWidth();
    float d1 = rawBuf[idx];
    idx = x2 + y1 * buf->getWidth();
    float d2 = rawBuf[idx];
    idx = x2 + y2 * buf->getWidth();
    float d3 = rawBuf[idx];
    float fx1 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d0 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d2;
    float fx2 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d1 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d3;
    float fF = (((float)y2 - vec.y)/((float)y2 - (float)y1)) * fx1 + ((vec.y - (float)y1)/((float)y2 - (float)y1)) * fx2;
    return fF;
}

Buffer2D* FluidModel::getSource()
{
    return source;
}





















