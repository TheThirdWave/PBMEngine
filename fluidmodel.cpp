#include "fluidmodel.h"

FluidModel::FluidModel()
{

}

FluidModel::FluidModel(Buffer2D *initBuf, Buffer2D* s)
{
    density.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    sDensity.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    tDensity.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    stDensity.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    velocity.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    charMap.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    forwardsMap.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    backwardsMap.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    err.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    pressure.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    obstruction.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    obstruction.setDataFloat(1);
    setObsBoundary();
    source = s;
    color = initBuf;
    hasSource = false;
    hasTSource = false;
    macCormack = false;
    gravity = 30.0;
    pLoops = 5;
    iopLoops = 5;
}

void FluidModel::init(Buffer2D *initBuf, Buffer2D* s)
{
    density.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    sDensity.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    tDensity.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    stDensity.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    velocity.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    charMap.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    forwardsMap.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    backwardsMap.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    err.init(initBuf->getWidth(), initBuf->getHeight(), 2, 1.0);
    pressure.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    obstruction.init(initBuf->getWidth(), initBuf->getHeight(), 1, 1.0);
    obstruction.setDataFloat(1);
    setObsBoundary();
    source = s;
    color = initBuf;
    hasSource = false;
    hasTSource = false;
    macCormack = false;
    gravity = 30.0;
    pLoops = 5;
    iopLoops = 5;
}

void FluidModel::runTimeStep(double timeStep)
{
    charMap.setToIndicies();

    //if we're doing log advection we need to get the fractions of a timestep that we're using.
    double dt = timeStep / pow(2, logLoops);

    //if macCormack set to true we run macCormack advection, if not Semi-Lagrangian.
    //we run the character map advection once no matter what.
    if(!macCormack) cMapSLAdvect(dt);
    else cMapMCAdvect(dt);

    //if the number of logLoops is > 0, we run log advection.
    int loops = 0;
    while(dt < timeStep)
    {
        if(!macCormack) cMapSLAdvect(dt);
        else cMapMCAdvect(dt);
        loops++;
        dt = dt * pow(2, loops);
    }

    //now we actually apply the advected characteristic map to the velocity and color maps.
    advection(timeStep);

    //if source has been inserted this frame we add it to the density.
    if(hasSource)
    {
        sources(timeStep);
        source->setDataFloat(0.0f);
        hasSource = false;
    }

    //calculate all forces.
    forces(timeStep);

    //work out the pressure stuff to make it actually look like a fluid.

    for(int j = 0; j < iopLoops; j++)
    {
        pressure.zeroOut();
        for(int i = 0; i < pLoops; i++)
        {
            calcPressure();
        }
        applyPressure();

        //enforce bounds last so density can't get by obstructions. (theoretically)
        enforceBounds();
    }
}

void FluidModel::advection(double timeStep)
{
    int width = density.getWidth();
    int height = density.getHeight();
    float* velGrid = velocity.getBuf();
    float* denGrid = density.getBuf();
    float* colGrid = color->getBuf();
    float* cMap = charMap.getBuf();

    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 iVec;
            glm::vec2 vel;
            glm::vec3 col;
            //Get the advected point from the characteristic map.
            int index = i + j * width;
            iVec.x = cMap[index * charMap.getNumChannels()];
            iVec.y = cMap[index * charMap.getNumChannels() + 1];
            if(iVec.y >= height || iVec.y < 0)
            {
                int x = x + 1;
            }

            //update the density and velocity from the point of advection.
            denGrid[index * density.getNumChannels()] = interpolateF(&density, iVec / density.getCellSize());
            vel = interpolate2Vec(&velocity, iVec / velocity.getCellSize());
            velGrid[index * velocity.getNumChannels()] = vel.x;
            velGrid[index * velocity.getNumChannels() + 1] = vel.y;
            col = interpolate3Vec(color, iVec / color->getCellSize());
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

    glm::vec2 up = glm::vec2(0.0f, -1.0f);
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 vel;
            int index = i + j * width;
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];

            vel += (-denGrid[index * density.getNumChannels()] * up * gravity) * (float)timeStep;
            velGrid[index * velocity.getNumChannels()] = vel.x;
            velGrid[index * velocity.getNumChannels() + 1] = vel.y;
        }
    }
}

void FluidModel::sources(double timeStep)
{
    int width = source->getWidth();
    int height = source->getHeight();
    float* denGrid = density.getBuf();
    float* sGrid = source->getBuf();
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            int index = i + j * width;
            denGrid[index * density.getNumChannels()] += sGrid[index * source->getNumChannels()];
        }
    }
}

void FluidModel::targetSource()
{
    int width = source->getWidth();
    int height = source->getHeight();
    float* denGrid = density.getBuf();
    float* sGrid = source->getBuf();
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            int index = i + j * width;
            denGrid[index * density.getNumChannels()] += sGrid[index * source->getNumChannels()];
        }
    }
}

void FluidModel::calcPressure()
{
    int width = pressure.getWidth();
    int height = pressure.getHeight();
    int size = width * height * pressure.getNumChannels();
    float* pGrid = pressure.getBuf();
    float* velGrid = velocity.getBuf();
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            int index = i + j * width;
            float curPressure = 0.0f;

            //get current pressure from adjacent pressures.
            for(int v = -1; v < 2; v += 2)
            {
                index = (i) + (j + v) * width;
                if(index * pressure.getNumChannels() >= 0 && index * pressure.getNumChannels() < size)
                {
                    curPressure += pGrid[index * pressure.getNumChannels()];
                }
            }
            for(int u = -1; u < 2 ; u += 2)
            {
                index = (i + u) + (j) * width;
                if(index * pressure.getNumChannels() >= 0 && index * pressure.getNumChannels() < size)
                {
                    curPressure += pGrid[index * pressure.getNumChannels()];
                }
            }

            //add divergence to current pressure.
            index = i + j * width;
            float divergence = calcDivergence(i, j);
            pGrid[index * pressure.getNumChannels()] = (curPressure / 4.0) - (divergence * (pressure.getCellSize() * pressure.getCellSize()) / 4.0);
        }
    }
}

void FluidModel::applyPressure()
{
    int width = velocity.getWidth();
    int height = velocity.getHeight();
    int size = width * height * pressure.getNumChannels();
    float* pGrid = pressure.getBuf();
    float* velGrid = velocity.getBuf();
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            int index = i + j * width;
            glm::vec2 curVel;
            glm::vec2 curP;
            curVel.x = velGrid[index * velocity.getNumChannels()];
            curVel.y = velGrid[index * velocity.getNumChannels() + 1];

            //get pressure differential along the x-axis.
            int index1 = (i + 1) + j * width;
            int index2 = (i - 1) + j * width;
            if(index1 * pressure.getNumChannels() >= size) curP.x = -pGrid[index2 * pressure.getNumChannels()] / (2.0 * pressure.getCellSize());
            else if(index2 * pressure.getNumChannels() < 0) curP.x = pGrid[index1 * pressure.getNumChannels()] / (2.0 * pressure.getCellSize());
            else curP.x = (pGrid[index1 * pressure.getNumChannels()] - pGrid[index2 * pressure.getNumChannels()]) / (2.0 * pressure.getCellSize());

            //get pressure differential along the y-axis.
            index1 = i + (j + 1) * width;
            index2 = i + (j - 1) * width;
            if(index1 * pressure.getNumChannels() >= size) curP.y = -pGrid[index2 * pressure.getNumChannels()] / (2.0 * pressure.getCellSize());
            else if(index2 * pressure.getNumChannels() < 0) curP.y = pGrid[index1 * pressure.getNumChannels()] / (2.0 * pressure.getCellSize());
            else curP.y = (pGrid[index1 * pressure.getNumChannels()] - pGrid[index2 * pressure.getNumChannels()]) / (2.0 * pressure.getCellSize());

            //calculate new velocity.
            curVel = curVel - curP;
            velGrid[index * velocity.getNumChannels()] = curVel.x;
            velGrid[index * velocity.getNumChannels() + 1] = curVel.y;
        }
    }
}

void FluidModel::enforceBounds()
{
    int width = velocity.getWidth();
    int height = velocity.getHeight();
    int size = width * height * velocity.getNumChannels();
    float* obsGrid = obstruction.getBuf();
    float* velGrid = velocity.getBuf();
    for(int j = 0; j < height; j++)
    {
     #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            int index = i + j * width;
            glm::vec2 curVel;
            curVel.x = velGrid[index * velocity.getNumChannels()];
            curVel.y = velGrid[index * velocity.getNumChannels() + 1];
            curVel = curVel * obsGrid[index * obstruction.getNumChannels()];
            velGrid[index * velocity.getNumChannels()] = curVel.x;
            velGrid[index * velocity.getNumChannels() + 1] = curVel.y;
        }
    }
}

void FluidModel::cMapSLAdvect(double timeStep)
{
    int width = charMap.getWidth();
    int height = charMap.getHeight();
    float* cMap = charMap.getBuf();
    float* velGrid = velocity.getBuf();

    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 iVec;
            glm::vec2 vel;
            int index = i + j * width;
            //Get the point in space represented by the index.ZZZ
            iVec.x = cMap[index * charMap.getNumChannels()];
            iVec.y = cMap[index * charMap.getNumChannels() + 1];

            //get the velocity at the point we're looping over so we know the point to advect from.
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];

            //subtract the velocity from the position of the point to get the point to advect from.
            iVec = iVec - vel * (float)timeStep;
            cMap[index * charMap.getNumChannels()] = iVec.x;
            cMap[index * charMap.getNumChannels() + 1] = iVec.y;
        }
    }
}

void FluidModel::cMapMCAdvect(double timeStep)
{
    int width = charMap.getWidth();
    int height = charMap.getHeight();
    float* cMap = charMap.getBuf();
    float* fMap = forwardsMap.getBuf();
    float* bMap = backwardsMap.getBuf();
    float* error = err.getBuf();
    float* velGrid = velocity.getBuf();

    //first we advect forwards using semi-lagrangian, we store the forwards map in charMap2
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 iVec;
            glm::vec2 vel;
            int index = i + j * width;
            //Get the point in space represented by the index.
            iVec.x = cMap[index * charMap.getNumChannels()];
            iVec.y = cMap[index * charMap.getNumChannels() + 1];

            //get the velocity at the point we're looping over so we know the point to advect from.
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];

            //subtract the velocity from the position of the point to get the point to advect from.
            iVec = iVec - vel * (float)timeStep;
            fMap[index * forwardsMap.getNumChannels()] = iVec.x;
            fMap[index * forwardsMap.getNumChannels() + 1] = iVec.y;
        }
    }
    //now we advect backwards using semi-lagrangian, we store the backwards map in charMap2
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 iVec;
            glm::vec2 vel;
            //Get the point in space represented by the index.
            iVec.x = (float)i;
            iVec.y = (float)j;
            iVec = iVec * backwardsMap.getCellSize();
            int index = i + j * width;

            //get the velocity at the point we're looping over so we know the point to advect from.
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];

            //add the velocity to the point in space and pass that into charMap2. If we weren't using grids
            //(and thus interpolation) this would just return our original point.  As it is, however, the
            //point we get back is slightly off.
            iVec = iVec + vel * (float)timeStep;
            iVec = interpolate2Vec(&forwardsMap, iVec / forwardsMap.getCellSize());
            bMap[index * backwardsMap.getNumChannels()] = iVec.x;
            bMap[index * backwardsMap.getNumChannels() + 1] = iVec.y;
        }
    }
    //now we get the error between our backwards advection and the actual point we're advecting.
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 iVec;
            glm::vec2 vel;
            //Get the point in space represented by the index.
            iVec.x = (float)i;
            iVec.y = (float)j;
            iVec = iVec * err.getCellSize();
            int index = i + j * width;

            //get the velocity at the point we're looping over so we know the point to advect from.
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];

            //subtract the forward advection from the actual point to get the error.
            iVec = (iVec - interpolate2Vec(&backwardsMap, iVec / err.getCellSize())) / 2.0f;
            error[index * err.getNumChannels()] = iVec.x;
            error[index * err.getNumChannels() + 1] = iVec.y;
        }
    }
    //Now we add the error to the forward map to get a better approximate advection.
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 iVec;
            glm::vec2 vel;
            //Get the point in space represented by the index.
            iVec.x = (float)i;
            iVec.y = (float)j;
            iVec = iVec * charMap.getCellSize();
            int index = i + j * width;

            //get the velocity at the point we're looping over so we know the point to advect from.
            vel.x = velGrid[index * velocity.getNumChannels()];
            vel.y = velGrid[index * velocity.getNumChannels() + 1];

            //subtract the velocity from the position of the point to get the point to advect from.
            iVec = interpolate2Vec(&forwardsMap, iVec / forwardsMap.getCellSize()) + interpolate2Vec(&err, iVec / err.getCellSize());
            cMap[index * charMap.getNumChannels()] = iVec.x;
            cMap[index * charMap.getNumChannels() + 1] = iVec.y;
        }
    }
}

void FluidModel::setObsBoundary()
{
    int width = obstruction.getWidth();
    int height = obstruction.getHeight();
    int size = width * height * obstruction.getNumChannels();
    float* obsGrid = obstruction.getBuf();

 #pragma omp parallel for
    for(int i = 0; i < width; i++)
    {
        int j = 0;
        int index = i + j * width;
        obsGrid[index * obstruction.getNumChannels()] = 0;
        j = 1;
        index = i + j * width;
        obsGrid[index * obstruction.getNumChannels()] = 0;
        j = 2;
        index = i + j * width;
        obsGrid[index * obstruction.getNumChannels()] = 0;
        j = 3;
        index = i + j * width;
        obsGrid[index * obstruction.getNumChannels()] = 0;
    }
#pragma omp parallel for
   for(int i = 0; i < width; i++)
   {
       int j = height - 1;
       int index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       j = height - 2;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       j = height - 3;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       j = height - 4;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
   }
#pragma omp parallel for
   for(int j = 0; j < height; j++)
   {
       int i = 0;
       int index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       i = 1;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       i = 2;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       i = 3;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
   }
#pragma omp parallel for
   for(int j = 0; j < height; j++)
   {
       int i = width - 1;
       int index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       i = width - 2;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       i = width - 3;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
       i = width - 4;
       index = i + j * width;
       obsGrid[index * obstruction.getNumChannels()] = 0;
   }
}

void FluidModel::fastBlur(Buffer2D *in, Buffer2D *out)
{

}

float FluidModel::calcDivergence(int i, int j)
{
    int index;
    int size = velocity.getWidth() * velocity.getHeight() * velocity.getNumChannels();
    float* velGrid = velocity.getBuf();
    float horz, horzprev, horznext;
    float vert, vertprev, vertnext;

    index = ((i + 1) + j * velocity.getWidth()) * velocity.getNumChannels();
    if(index < 0 || index >= size)
    {
        horznext = 0.0f;
    }
    else horznext = velGrid[index];
    index = ((i - 1) + j * velocity.getWidth()) * velocity.getNumChannels();
    if(index < 0 || index >= size) horzprev = 0.0f;
    else horzprev = velGrid[index];
    horz = (horznext - horzprev) / (2 * velocity.getCellSize());

    index = (i + (j + 1) * velocity.getWidth()) * velocity.getNumChannels();
    if(index < 0 || index >= size) vertnext = 0.0f;
    else vertnext = velGrid[index + 1];
    index = (i + (j - 1) * velocity.getWidth()) * velocity.getNumChannels();
    if(index < 0 || index >= size) vertprev = 0.0f;
    else vertprev = velGrid[index + 1];
    vert = (vertnext - vertprev) / (2 * velocity.getCellSize());

    return horz + vert;
}

float FluidModel::interpolateF(Buffer2D* buf, glm::vec2 vec)
{
    //it's a bilinear interpolation.
    float* rawBuf = buf->getBuf();
    int size = buf->getWidth() * buf->getHeight() * buf->getNumChannels();
    int x1 = (int)vec.x;
    int y1 = (int)vec.y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    int idx = (x1 + y1 * buf->getWidth()) * buf->getNumChannels();
    float d0;
    if(idx < 0 || idx >= size) d0 = 0;
    else d0 = rawBuf[idx];
    idx = (x1 + y2 * buf->getWidth()) * buf->getNumChannels();
    float d1;
    if(idx < 0 || idx >= size) d1 = 0;
    else d1 = rawBuf[idx];
    idx = (x2 + y1 * buf->getWidth()) * buf->getNumChannels();
    float d2;
    if(idx < 0 || idx >= size) d2 = 0;
    else d2 = rawBuf[idx];
    idx = (x2 + y2 * buf->getWidth()) * buf->getNumChannels();
    float d3;
    if(idx < 0 || idx >= size) d3 = 0;
    else d3 = rawBuf[idx];
    float fx1 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d0 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d2;
    float fx2 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d1 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d3;
    float fF = (((float)y2 - vec.y)/((float)y2 - (float)y1)) * fx1 + ((vec.y - (float)y1)/((float)y2 - (float)y1)) * fx2;
    return fF;
}

glm::vec2 FluidModel::interpolate2Vec(Buffer2D* buf, glm::vec2 vec)
{
    //it's a binlinear interpolation.
    float* rawBuf = buf->getBuf();
    int size = buf->getWidth() * buf->getHeight() * buf->getNumChannels();
    int x1 = (int)vec.x;
    int y1 = (int)vec.y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    int idx = (x1 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d0;
    if(idx < 0 || idx >= size) d0 = glm::vec2(0.0f);
    else
    {
        d0.x = rawBuf[idx];
        d0.y = rawBuf[idx + 1];
    }
    idx = (x1 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d1;
    if(idx < 0 || idx >= size) d1 = glm::vec2(0.0f);
    else
    {
        d1.x = rawBuf[idx];
        d1.y = rawBuf[idx + 1];
    }
    idx = (x2 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d2;
    if(idx < 0 || idx >= size) d2 = glm::vec2(0.0f);
    else
    {
        d2.x = rawBuf[idx];
        d2.y = rawBuf[idx + 1];
    }
    idx = (x2 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec2 d3;
    if(idx < 0 || idx >= size) d2 = glm::vec2(0.0f);
    else
    {
        d3.x = rawBuf[idx];
        d3.y = rawBuf[idx + 1];
    }
    glm::vec2 fx1 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d0 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d2;
    glm::vec2 fx2 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d1 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d3;
    glm::vec2 fF = (((float)y2 - vec.y)/((float)y2 - (float)y1)) * fx1 + ((vec.y - (float)y1)/((float)y2 - (float)y1)) * fx2;
    return fF;
}

glm::vec3 FluidModel::interpolate3Vec(Buffer2D* buf, glm::vec2 vec)
{
    //it's a binlinear interpolation.
    float* rawBuf = buf->getBuf();
    int size = buf->getWidth() * buf->getHeight() * buf->getNumChannels();
    int x1 = (int)vec.x;
    int y1 = (int)vec.y;
    int x2 = x1 + 1;
    int y2 = y1 + 1;
    int idx = (x1 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d0;
    if(idx < 0 || idx >= size) d0 = glm::vec3(0.0f);
    else
    {
        d0.x = rawBuf[idx];
        d0.y = rawBuf[idx + 1];
        d0.z = rawBuf[idx + 2];
    }
    idx = (x1 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d1;
    if(idx < 0 || idx >= size) d1 = glm::vec3(0.0f);
    else
    {
    d1.x = rawBuf[idx];
    d1.y = rawBuf[idx + 1];
    d1.z = rawBuf[idx + 2];
    }
    idx = (x2 + y1 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d2;
    if(idx < 0 || idx >= size) d2 = glm::vec3(0.0f);
    else
    {
        d2.x = rawBuf[idx];
        d2.y = rawBuf[idx + 1];
        d2.z = rawBuf[idx + 2];
    }
    idx = (x2 + y2 * buf->getWidth()) * buf->getNumChannels();
    glm::vec3 d3;
    if(idx < 0 || idx >= size) d2 = glm::vec3(0.0f);
    else
    {
        d3.x = rawBuf[idx];
        d3.y = rawBuf[idx + 1];
        d3.z = rawBuf[idx + 2];
    }
    glm::vec3 fx1 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d0 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d2;
    glm::vec3 fx2 = (((float)x2 - vec.x)/((float)x2 - (float)x1)) * d1 + ((vec.x - (float)x1)/((float)x2 - (float)x1)) * d3;
    glm::vec3 fF = (((float)y2 - vec.y)/((float)y2 - (float)y1)) * fx1 + ((vec.y - (float)y1)/((float)y2 - (float)y1)) * fx2;
    return fF;
}

float FluidModel::getGravity()
{
    return gravity;
}

int FluidModel::getPLoops()
{
    return pLoops;
}

int FluidModel::getIOPLoops()
{
    return iopLoops;
}

int FluidModel::getLogLoops()
{
    return logLoops;
}

bool FluidModel::usingMacCormack()
{
    return macCormack;
}

void FluidModel::setGravity(float f)
{
    gravity = f;
}

void FluidModel::setHasSource(bool t)
{
    hasSource = t;
}

void FluidModel::setPLoops(int l)
{
    pLoops = l;
}

void FluidModel::setIOPLoops(int l)
{
    iopLoops = l;
}

void FluidModel::setLogLoops(int l)
{
    logLoops = l;
}

void FluidModel::setUsingMacCormack(bool t)
{
    macCormack = t;
}

void FluidModel::reset()
{
    density.zeroOut();
    velocity.zeroOut();
    pressure.zeroOut();
    charMap.zeroOut();
    backwardsMap.zeroOut();
    forwardsMap.zeroOut();
    sDensity.zeroOut();
    tDensity.zeroOut();
    stDensity.zeroOut();
    err.zeroOut();
    obstruction.setDataFloat(1.0);
    setObsBoundary();
    hasSource = false;
}


Buffer2D* FluidModel::getSource()
{
    return source;
}

Buffer2D* FluidModel::getDensity()
{
    return &density;
}

Buffer2D* FluidModel::getPressure()
{
    return &pressure;
}

Buffer2D* FluidModel::getObstruction()
{
    return &obstruction;
}

Buffer2D* FluidModel::getError()
{
    return &err;
}





















