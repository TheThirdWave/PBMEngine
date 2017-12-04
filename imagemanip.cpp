#include "imagemanip.h"

Imagemanip::Imagemanip()
{
}

Imagemanip::Imagemanip(image* i)
{
    screen.width = i->width;
    screen.height = i->height;
    screen.unitbytes = i->unitbytes;
    screen.size = i->size;
    screen.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memcpy(screen.data, i->data, sizeof(unsigned char) * screen.size);
    foreground = glm::vec3(0, 0, 0);
    background = glm::vec3(1.0f, 1.0f, 1.0f);
    funcNum = 0;
}

Imagemanip::Imagemanip(int width, int height)
{
    screen.width = width;
    screen.height = height;
    screen.unitbytes = RGBA;
    screen.size = width * height * screen.unitbytes;
    screen.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memset(screen.data, 0, (sizeof(unsigned char) * screen.size));
    foreground = glm::vec3(0, 0, 0);
    background = glm::vec3(1.0f, 1.0f, 1.0f);
    funcNum = 0;
}

Imagemanip::~Imagemanip()
{
    delete screen.data;
}

void Imagemanip::setScreen(image* i)
{
    screen.width = i->width;
    screen.height = i->height;
    screen.unitbytes = i->unitbytes;
    screen.size = i->size;
    screen.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memcpy(screen.data, i->data, sizeof(unsigned char) * screen.size);
}

void Imagemanip::setKernel(int w, int h)
{
    kern = Kernel(w, h);
    kern.setWeights(1.0f / (kern.height * kern.width));
}

void Imagemanip::setKernelValues(float v)
{
    kern.setWeights(v);
}

void Imagemanip::setKernelValuesF(Function2D * func)
{
    kern.setFuncWeights(func);
}

void Imagemanip::setKernelValuesEF(Function2D * func)
{
    kern.setFExactWeightsAvg(func);
}

void Imagemanip::initScreen(int width, int height)
{
    screen.width = width;
    screen.height = height;
    screen.unitbytes = RGBA;
    screen.size = width * height * screen.unitbytes;
    screen.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memset(screen.data, 0 , (sizeof(unsigned char) * screen.size));
    filterScreen.width = width;
    filterScreen.height = height;
    filterScreen.unitbytes = RGBA;
    filterScreen.size = width * height * screen.unitbytes;
    filterScreen.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memset(filterScreen.data, 0 , (sizeof(unsigned char) * screen.size));
    foreground = glm::vec3(0, 0, 0);
    background = glm::vec3(1.0f, 1.0f, 1.0f);
    funcNum = 0;
}

void Imagemanip::initScreen(image* i)
{
    screen.width = i->width;
    screen.height = i->height;
    screen.unitbytes = i->unitbytes;
    screen.size = i->size;
    screen.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memcpy(screen.data, i->data, (sizeof(unsigned char) * screen.size));
    filterScreen.width = screen.width;
    filterScreen.height = screen.height;
    filterScreen.unitbytes = RGBA;
    filterScreen.size = screen.size;
    filterScreen.data = new unsigned char[sizeof(unsigned char) * filterScreen.size];
    memset(filterScreen.data, 0 , (sizeof(unsigned char) * filterScreen.size));
    foreground = glm::vec3(0, 0, 0);
    background = glm::vec3(1.0f, 1.0f, 1.0f);
    funcNum = 0;
}

void Imagemanip::setBackground(char r, char g, char b)
{
    background.r = r;
    background.g = g;
    background.b = b;
}

void Imagemanip::setForeground(char r, char g, char b)
{
    foreground.r = r;
    foreground.g = g;
    foreground.b = b;
}

void Imagemanip::addFunction(Function2D* func)
{
    functions[funcNum] = func;
    functions[funcNum]->origPoint.x = screen.width - functions[funcNum]->origPoint.x;
    functions[funcNum]->origPoint.x *= screen.unitbytes;
    functions[funcNum]->origPoint.y *= screen.unitbytes;
    funcNum++;
}

int Imagemanip::getHeight()
{
    return screen.height;
}

int Imagemanip::getWidth()
{
    return screen.width;
}

image Imagemanip::getScreen()
{
    image img;
    img.width = screen.width;
    img.height = screen.height;
    img.unitbytes = screen.unitbytes;
    img.size = screen.size;
    img.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memcpy(img.data, screen.data, (sizeof(unsigned char) * screen.size));
    return img;
}

image* Imagemanip::getPtr()
{
    return &screen;
}

void Imagemanip::psychedelic(int count)
{
    while(count >= 0)
    {
        int rx = rand() % screen.width * screen.unitbytes;
        int ry = rand() % screen.height * screen.unitbytes;
        int gx = rand() % screen.width * screen.unitbytes;
        int gy = rand() % screen.height * screen.unitbytes;
        int bx = rand() % screen.width * screen.unitbytes;
        int by = rand() % screen.height * screen.unitbytes;

        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                //modulus 256 to keep things the size of a byte, I probably don't need to do it
                //'cause it truncates everything to put it into the char but whatever.
                char r = (int)sqrt(pow((x - rx), 2) + pow((y - ry), 2)) % 256;
                char g = (int)sqrt(pow((x - gx), 2) + pow((y - gy), 2)) % 256;
                char b = (int)sqrt(pow((x - bx), 2) + pow((y - by), 2)) % 256;
                screen.data[x + (y * screen.width)] = r;
                screen.data[(x + 1) + (y * screen.width)] = g;
                screen.data[(x + 2) + (y * screen.width)] = b;
                //the alpha is always at max for now.
                screen.data[(x + 3) + (y * screen.width)] = 255;
            }
        }
        count--;
    }
}

void Imagemanip::clearScreen()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            screen.data[x + (y * screen.width)] = background.r * 255;
            screen.data[(x + 1) + (y * screen.width)] = background.g * 255;
            screen.data[(x + 2) + (y * screen.width)] = background.b * 255;
            //the alpha is always at max for now.
            screen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
}

void Imagemanip::noise()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            screen.data[x + (y * screen.width)] = rand() % 255;
            screen.data[(x + 1) + (y * screen.width)] = rand() % 255;
            screen.data[(x + 2) + (y * screen.width)] = rand() % 255;
            //the alpha is always at max for now.
            screen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
}

void Imagemanip::emptyFunctions()
{
    funcNum = 0;
}

void Imagemanip::switchScreenPtrs()
{
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::fillScreen(int r, int g, int b)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            //modulus 256 to keep things the size of a byte, I probably don't need to do it
            //'cause it truncates everything to put it into the char but whatever.
            screen.data[x + (y * screen.width)] = r % 256;
            screen.data[(x + 1) + (y * screen.width)] = g % 256;
            screen.data[(x + 2) + (y * screen.width)] = b % 256;
            //the alpha is always at max for now.
            screen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
}

void Imagemanip::fillScreen(int r, int g, int b, int a)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            //modulus 256 to keep things the size of a byte, I probably don't need to do it
            //'cause it truncates everything to put it into the char but whatever.
            screen.data[x + (y * screen.width)] = r % 256;
            screen.data[(x + 1) + (y * screen.width)] = g % 256;
            screen.data[(x + 2) + (y * screen.width)] = b % 256;
            //the alpha is always at max for now.
            screen.data[(x + 3) + (y * screen.width)] = a % 256;
        }
    }
}


void Imagemanip::drawConvex()
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                bool flag = true;
                for(int i = 0; i < funcNum; i++)
                {
                    if(functions[i]->getRelative(glm::vec2(x,y)) > 0)
                    {
                        flag = false;
                        break;
                    }
                }
                if(flag == true)
                {
                    screen.data[x + (y * screen.width)] = foreground.r * 255;
                    screen.data[(x + 1) + (y * screen.width)] = foreground.g * 255;
                    screen.data[(x + 2) + (y * screen.width)] = foreground.b * 255;
                    //the alpha is always at max for now.
                    screen.data[(x + 3) + (y * screen.width)] = 255;
                }
            }
        }
    }
}

void Imagemanip::drawUnion()
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                bool flag = false;
                for(int i = 0; i < funcNum; i++)
                {
                    if(functions[i]->getRelative(glm::vec2(x,y)) < 0)
                    {
                        flag = true;
                        break;
                    }
                }
                if(flag == true)
                {
                    screen.data[x + (y * screen.width)] = foreground.r * 255;
                    screen.data[(x + 1) + (y * screen.width)] = foreground.g * 255;
                    screen.data[(x + 2) + (y * screen.width)] = foreground.b * 255;
                    //the alpha is always at max for now.
                    screen.data[(x + 3) + (y * screen.width)] = 255;
                }
            }
        }
    }
}

void Imagemanip::drawStar()
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                int numTrue = 0;
                for(int i = 0; i < funcNum; i++)
                {
                    if(functions[i]->getRelative(glm::vec2(x,y)) <= 0)
                    {
                        numTrue++;
                    }
                }
                if(numTrue >= funcNum - 1)
                {
                    screen.data[x + (y * screen.width)] = foreground.r * 255;
                    screen.data[(x + 1) + (y * screen.width)] = foreground.g * 255;
                    screen.data[(x + 2) + (y * screen.width)] = foreground.b * 255;
                    //the alpha is always at max for now.
                    screen.data[(x + 3) + (y * screen.width)] = 255;
                }
            }
        }
    }
}

void Imagemanip::drawMod(int modulus)
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                int numTrue = 0;
                for(int i = 0; i < funcNum; i++)
                {
                    if(functions[i]->getRelative(glm::vec2(x,y)) <= 0)
                    {
                        numTrue++;
                    }
                }
                if(numTrue % modulus > 0)
                {
                    screen.data[x + (y * screen.width)] = foreground.r * 255;
                    screen.data[(x + 1) + (y * screen.width)] = foreground.g * 255;
                    screen.data[(x + 2) + (y * screen.width)] = foreground.b * 255;
                    //the alpha is always at max for now.
                    screen.data[(x + 3) + (y * screen.width)] = 255;
                }
            }
        }
    }
}


void Imagemanip::drawBlobby()
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                bool flag = false;
                for(int i = 0; i < funcNum; i++)
                {
                    if(functions[i]->getRelative(glm::vec2(x,y)) <= 0)
                    {
                        flag = true;
                        break;
                    }
                }
                if(flag == true)
                {
                    screen.data[x + (y * screen.width)] = foreground.r * 255;
                    screen.data[(x + 1) + (y * screen.width)] = foreground.g * 255;
                    screen.data[(x + 2) + (y * screen.width)] = foreground.b * 255;
                    //the alpha is always at max for now.
                    screen.data[(x + 3) + (y * screen.width)] = 255;
                }
            }
        }
    }
}

void Imagemanip::drawStarAA(int resolution)
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                int fractionTrue = 0;
                for(int u = 0; u < resolution; u++)
                {
                    for(int v = 0; v < resolution; v++)
                    {
                        int numTrue = 0;
                        float uf = u * (1.0f / resolution);
                        float vf = v *(1.0f / resolution);
                        uf = uf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        vf = vf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        for(int i = 0; i < funcNum; i++)
                        {
                            if(functions[i]->getRelative(glm::vec2((x + uf),(y + vf))) <= 0)
                            {
                                numTrue++;
                            }
                        }
                        if(numTrue >= funcNum - 1)
                        {
                            fractionTrue++;
                        }
                    }
                }
                if(fractionTrue > 0 && fractionTrue < (resolution * resolution))
                {
                    int u = 55;
                    u++;
                }
                if(fractionTrue == 0)
                {
                    int u = 55;
                    u++;
                }
                glm::vec3 interpolate = background - foreground;
                interpolate = interpolate * (1 - (float)fractionTrue/(resolution * resolution));
                interpolate = foreground + interpolate;
                screen.data[x + (y * screen.width)] = interpolate.r * 255;
                screen.data[(x + 1) + (y * screen.width)] = interpolate.g * 255;
                screen.data[(x + 2) + (y * screen.width)] = interpolate.b * 255;
                //the alpha is always at max for now.
                screen.data[(x + 3) + (y * screen.width)] = 255;
            }
        }
    }
}

void Imagemanip::drawConvexAA(int resolution)
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                int fractionTrue = 0;
                for(int u = 0; u < resolution; u++)
                {
                    for(int v = 0; v < resolution; v++)
                    {
                        bool flag = true;
                        float uf = u * (1.0f / resolution);
                        float vf = v *(1.0f / resolution);
                        uf = uf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        vf = vf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        for(int i = 0; i < funcNum; i++)
                        {
                            if(functions[i]->getRelative(glm::vec2((x + uf),(y + vf))) > 0)
                            {
                                flag = false;
                                break;
                            }
                        }
                        if(flag)
                        {
                            fractionTrue++;
                        }
                    }
                }
                if(fractionTrue > 0 && fractionTrue < (resolution * resolution))
                {
                    int u = 55;
                    u++;
                }
                if(fractionTrue == 0)
                {
                    int u = 55;
                    u++;
                }
                glm::vec3 interpolate = background - foreground;
                interpolate = interpolate * (1 - (float)fractionTrue/(resolution * resolution));
                interpolate = foreground + interpolate;
                screen.data[x + (y * screen.width)] = interpolate.r * 255;
                screen.data[(x + 1) + (y * screen.width)] = interpolate.g * 255;
                screen.data[(x + 2) + (y * screen.width)] = interpolate.b * 255;
                //the alpha is always at max for now.
                screen.data[(x + 3) + (y * screen.width)] = 255;
            }
        }
    }
}

void Imagemanip::drawModAA(int resolution, int modulus)
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                int fractionTrue = 0;
                for(int u = 0; u < resolution; u++)
                {
                    for(int v = 0; v < resolution; v++)
                    {
                        int numTrue = 0;
                        float uf = u * (1.0f / resolution);
                        float vf = v *(1.0f / resolution);
                        uf = uf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        vf = vf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        for(int i = 0; i < funcNum; i++)
                        {
                            if(functions[i]->getRelative(glm::vec2((x + uf),(y + vf))) <= 0)
                            {
                                numTrue++;
                            }
                        }
                        if(numTrue % modulus > 0)
                        {
                            fractionTrue++;
                        }
                    }
                }
                if(fractionTrue > 0 && fractionTrue < (resolution * resolution))
                {
                    int u = 55;
                    u++;
                }
                if(fractionTrue == 0)
                {
                    int u = 55;
                    u++;
                }
                glm::vec3 interpolate = background - foreground;
                interpolate = interpolate * (1 - (float)fractionTrue/(resolution * resolution));
                interpolate = foreground + interpolate;
                screen.data[x + (y * screen.width)] = interpolate.r * 255;
                screen.data[(x + 1) + (y * screen.width)] = interpolate.g * 255;
                screen.data[(x + 2) + (y * screen.width)] = interpolate.b * 255;
                //the alpha is always at max for now.
                screen.data[(x + 3) + (y * screen.width)] = 255;
            }
        }
    }
}

void Imagemanip::drawBlobbyAA(int resolution)
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                int fractionTrue = 0;
                for(int u = 0; u < resolution; u++)
                {
                    for(int v = 0; v < resolution; v++)
                    {
                        bool flag = false;
                        float uf = u * (1.0f / resolution);
                        float vf = v *(1.0f / resolution);
                        uf = uf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        vf = vf + ((1.0f/resolution) * (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
                        for(int i = 0; i < funcNum; i++)
                        {
                            if(functions[i]->getRelative(glm::vec2((x + uf),(y + vf))) <= 0)
                            {
                                flag = true;
                                break;
                            }
                        }
                        if(flag)
                        {
                            fractionTrue++;
                        }
                    }
                }
                if(fractionTrue > 0 && fractionTrue < (resolution * resolution))
                {
                    int u = 55;
                    u++;
                }
                if(fractionTrue == 0)
                {
                    int u = 55;
                    u++;
                }
                glm::vec3 interpolate = background - foreground;
                interpolate = interpolate * (1 - (float)fractionTrue/(resolution * resolution));
                interpolate = foreground + interpolate;
                screen.data[x + (y * screen.width)] = interpolate.r * 255;
                screen.data[(x + 1) + (y * screen.width)] = interpolate.g * 255;
                screen.data[(x + 2) + (y * screen.width)] = interpolate.b * 255;
                //the alpha is always at max for now.
                screen.data[(x + 3) + (y * screen.width)] = 255;
            }
        }
    }
}

void Imagemanip::drawShaded()
{
    if(funcNum > 0)
    {
        for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
        {
            for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
            {
                glm::vec3 interpolate;
                glm::vec2 pt = glm::vec2((float)x, (float)y);
                for(int i = 0; i < funcNum; i++)
                {
                    float cDist = glm::length(functions[i]->origPoint - pt);
                    float nDist = glm::length(functions[i]->normal);
                    float proportion = (cDist / nDist);
                    if(proportion > 1) proportion = 1;
                    else if(proportion < 1)
                    {
                        int u = 55;
                        u++;
                    }
                    proportion = 1 - proportion;

                    interpolate = background - foreground;
                    interpolate = interpolate * (proportion);

                    interpolate = foreground + interpolate;
                }
                screen.data[x + (y * screen.width)] = std::min(255, (int)(screen.data[x + (y * screen.width)] + interpolate.r * 255));
                screen.data[(x + 1) + (y * screen.width)] = std::min(255, (int)(screen.data[(x + 1) + (y * screen.width)] + interpolate.g * 255));
                screen.data[(x + 2) + (y * screen.width)] = std::min(255, (int)(screen.data[(x + 2) + (y * screen.width)] + interpolate.b * 255));
                //the alpha is always at max for now.
                screen.data[(x + 3) + (y * screen.width)] = 255;
            }
        }
    }
}

void Imagemanip::simpleBlur()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            float agR = 0.0f;
            float agG = 0.0f;
            float agB = 0.0f;
            for(int i = 0; i < kern.height; i++)
            {
                for(int j = 0; j < kern.width; j++)
                {
                    int xk = x + ((j - kern.wr) * screen.unitbytes);
                    int yk = y + ((i - kern.hr) * screen.unitbytes);
                    xk = abs(xk);
                    yk = abs(yk);
                    if(!(xk < 0 || xk > screen.width * screen.unitbytes || yk < 0 || yk > screen.height * screen.unitbytes))
                    {
                        agR += screen.data[xk + (yk * screen.width)] * kern.weights[i][j];
                        agG += screen.data[(xk + 1) + (yk * screen.width)] * kern.weights[i][j];
                        agB += screen.data[(xk + 2) + (yk * screen.width)] * kern.weights[i][j];
                    }
                }
            }

            filterScreen.data[x + (y * screen.width)] = agR;
            filterScreen.data[(x + 1) + (y * screen.width)] = agG;
            filterScreen.data[(x + 2) + (y * screen.width)] = agB;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::motionBlur(Function2D* func)
{
    kern.setFExactWeightsAvg(func);
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            float agR = 0.0f;
            float agG = 0.0f;
            float agB = 0.0f;
            for(int i = 0; i < kern.height; i++)
            {
                for(int j = 0; j < kern.width; j++)
                {
                    int xk = x + ((j - kern.wr) * screen.unitbytes);
                    int yk = y + ((i - kern.hr) * screen.unitbytes);
                    if(!(xk < 0 || xk > screen.width * screen.unitbytes || yk < 0 || yk > screen.height * screen.unitbytes))
                    {
                        agR += screen.data[xk + (yk * screen.width)] * kern.weights[i][j];
                        agG += screen.data[(xk + 1) + (yk * screen.width)] * kern.weights[i][j];
                        agB += screen.data[(xk + 2) + (yk * screen.width)] * kern.weights[i][j];
                    }
                }
            }

            filterScreen.data[x + (y * screen.width)] = agR;
            filterScreen.data[(x + 1) + (y * screen.width)] = agG;
            filterScreen.data[(x + 2) + (y * screen.width)] = agB;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::maskedMBlur(Imagemanip * vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            float agR = 0.0f;
            float agG = 0.0f;
            float agB = 0.0f;
            image* mask = &vMask->screen;
            glm::vec2 v = glm::vec2(-(mask->data[(x + 1) + (y * mask->width)] - 128), mask->data[(x) + (y * mask->width)] - 128);
            if(glm::length(v) == 0)
            {
                kern.makeIdentity();
            }
            else
            {
                glm::vec2 nv = glm::normalize(v);
                LineFunction l = LineFunction(nv, glm::vec2(0.0f, 0.0f));
                kern.setFExactWeightsAvg(&l);
            }
            for(int i = 0; i < kern.height; i++)
            {
                for(int j = 0; j < kern.width; j++)
                {
                    int xk = x + ((j - kern.wr) * screen.unitbytes);
                    int yk = y + ((i - kern.hr) * screen.unitbytes);
                    if(!(xk < 0 || xk > screen.width * screen.unitbytes || yk < 0 || yk > screen.height * screen.unitbytes))
                    {
                        agR += screen.data[xk + (yk * screen.width)] * kern.weights[i][j];
                        agG += screen.data[(xk + 1) + (yk * screen.width)] * kern.weights[i][j];
                        agB += screen.data[(xk + 2) + (yk * screen.width)] * kern.weights[i][j];
                    }
                }
            }

            filterScreen.data[x + (y * screen.width)] = agR;
            filterScreen.data[(x + 1) + (y * screen.width)] = agG;
            filterScreen.data[(x + 2) + (y * screen.width)] = agB;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::emboss()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            float agR = 0.0f;
            float agG = 0.0f;
            float agB = 0.0f;
            float agRY = 0.0f;
            float agGY = 0.0f;
            float agBY = 0.0f;
            float agRNY = 0.0f;
            float agGNY = 0.0f;
            float agBNY = 0.0f;
            float agRX = 0.0f;
            float agGX = 0.0f;
            float agBX = 0.0f;
            float agRNX = 0.0f;
            float agGNX = 0.0f;
            float agBNX = 0.0f;

            int xk = x - (kern.wr * screen.unitbytes);
            int xnk = x + (kern.wr * screen.unitbytes);
            if(xk < 0) xk = xnk;
            if(xnk > screen.width * screen.unitbytes) xnk = xk;

            int yk = y - (kern.hr * screen.unitbytes);
            int ynk = y + (kern.hr * screen.unitbytes);
            if(yk < 0) yk = ynk;
            if(ynk > screen.height * screen.unitbytes) ynk = yk;

            agRX += screen.data[xk + (y * screen.width)] * kern.weights[0][kern.hr + 1];
            agGX += screen.data[(xk + 1) + (y * screen.width)] * kern.weights[0][kern.hr + 1];
            agBX += screen.data[(xk + 2) + (y * screen.width)] * kern.weights[0][kern.hr + 1];
            agRNX += screen.data[xnk + (y * screen.width)] * kern.weights[kern.width-1][kern.hr + 1];
            agGNX += screen.data[(xnk + 1) + (y * screen.width)] * kern.weights[kern.width-1][kern.hr + 1];
            agBNX += screen.data[(xnk + 2) + (y * screen.width)] * kern.weights[kern.width-1][kern.hr + 1];

            agRY += screen.data[x + (yk * screen.width)] * kern.weights[kern.wr +1][0];
            agGY += screen.data[(x + 1) + (yk * screen.width)] * kern.weights[kern.wr +1][0];
            agBY += screen.data[(x + 2) + (yk * screen.width)] * kern.weights[kern.wr +1][0];
            agRNY += screen.data[x + (ynk * screen.width)] * kern.weights[kern.wr +1][kern.height - 1];
            agGNY += screen.data[(x + 1) + (ynk * screen.width)] * kern.weights[kern.wr +1][kern.height - 1];
            agBNY += screen.data[(x + 2) + (ynk * screen.width)] * kern.weights[kern.wr +1][kern.height - 1];

            agR = abs(agRX - agRNX) + abs(agRY - agRNY)/2;
            agG = abs(agGX - agGNX) + abs(agGY - agGNY)/2;
            agB = abs(agBX - agBNX) + abs(agBY - agBNY)/2;


            filterScreen.data[x + (y * screen.width)] = agR;
            filterScreen.data[(x + 1) + (y * screen.width)] = agG;
            filterScreen.data[(x + 2) + (y * screen.width)] = agB;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::dilation()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec3 rgb(1.0f);
            glm::vec3 hsv(0.0f);
            glm::vec3 holdHSV(0.0f);

            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb = rgb / 255.0f;
            hsv = rgbtohsv(rgb);

            for(int i = 0; i < kern.height; i++)
            {
                for(int j = 0; j < kern.width; j++)
                {

                    int xk = x + ((j - kern.wr) * screen.unitbytes);
                    int yk = y + ((i - kern.hr) * screen.unitbytes);
                    xk = abs(xk);
                    yk = abs(yk);
                    if(!(xk < 0 || xk > screen.width * screen.unitbytes || yk < 0 || yk > screen.height * screen.unitbytes))
                    {
                        rgb.r = screen.data[xk + (yk * screen.width)] * kern.weights[i][j];
                        rgb.g = screen.data[(xk + 1) + (yk * screen.width)] * kern.weights[i][j];
                        rgb.b = screen.data[(xk + 2) + (yk * screen.width)] * kern.weights[i][j];
                        rgb = rgb / 255.0f;
                        holdHSV = rgbtohsv(rgb);
                        if(hsv.z < holdHSV.z) hsv.z = holdHSV.z;
                    }
                }
            }

            rgb = hsvtorgb(hsv);

            filterScreen.data[x + (y * screen.width)] = rgb.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = rgb.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = rgb.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::erosion()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec3 rgb(1.0f);
            glm::vec3 hsv(0.0f);
            glm::vec3 holdHSV(0.0f);

            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb = rgb / 255.0f;
            hsv = rgbtohsv(rgb);

            for(int i = 0; i < kern.height; i++)
            {
                for(int j = 0; j < kern.width; j++)
                {

                    int xk = x + ((j - kern.wr) * screen.unitbytes);
                    int yk = y + ((i - kern.hr) * screen.unitbytes);
                    xk = abs(xk);
                    yk = abs(yk);
                    if(!(xk < 0 || xk > screen.width * screen.unitbytes || yk < 0 || yk > screen.height * screen.unitbytes))
                    {
                        rgb.r = screen.data[xk + (yk * screen.width)] * kern.weights[i][j];
                        rgb.g = screen.data[(xk + 1) + (yk * screen.width)] * kern.weights[i][j];
                        rgb.b = screen.data[(xk + 2) + (yk * screen.width)] * kern.weights[i][j];
                        rgb = rgb / 255.0f;
                        holdHSV = rgbtohsv(rgb);
                        if(hsv.z > holdHSV.z) hsv.z = holdHSV.z;
                    }
                }
            }

            rgb = hsvtorgb(hsv);

            filterScreen.data[x + (y * screen.width)] = rgb.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = rgb.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = rgb.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::maskedDilation(Imagemanip* vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec3 rgb(1.0f);
            glm::vec3 hsv(0.0f);
            glm::vec3 holdHSV(0.0f);

            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb = rgb / 255.0f;
            hsv = rgbtohsv(rgb);

            image* mask = &vMask->screen;
            glm::vec2 v = glm::vec2(-(mask->data[(x + 1) + (y * mask->width)] - 128), mask->data[(x) + (y * mask->width)] - 128);
            if(glm::length(v) == 0)
            {
                kern.makeIdentity();
            }
            else
            {
                glm::vec2 nv = glm::normalize(v);
                LineFunction l = LineFunction(nv, glm::vec2(0.0f, 0.0f));
                kern.setFExactWeights(&l);
            }

            for(int i = 0; i < kern.height; i++)
            {
                for(int j = 0; j < kern.width; j++)
                {

                    int xk = x + ((j - kern.wr) * screen.unitbytes);
                    int yk = y + ((i - kern.hr) * screen.unitbytes);
                    xk = abs(xk);
                    yk = abs(yk);
                    if(!(xk < 0 || xk > screen.width * screen.unitbytes || yk < 0 || yk > screen.height * screen.unitbytes) && kern.weights[i][j] != 0)
                    {
                        rgb.r = screen.data[xk + (yk * screen.width)] * kern.weights[i][j];
                        rgb.g = screen.data[(xk + 1) + (yk * screen.width)] * kern.weights[i][j];
                        rgb.b = screen.data[(xk + 2) + (yk * screen.width)] * kern.weights[i][j];
                        rgb = rgb / 255.0f;
                        holdHSV = rgbtohsv(rgb);
                        if(hsv.z < holdHSV.z) hsv.z = holdHSV.z;
                    }
                }
            }

            rgb = hsvtorgb(hsv);

            filterScreen.data[x + (y * screen.width)] = rgb.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = rgb.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = rgb.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::maskedErosion(Imagemanip* vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec3 rgb(1.0f);
            glm::vec3 hsv(0.0f);
            glm::vec3 holdHSV(0.0f);

            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb = rgb / 255.0f;
            hsv = rgbtohsv(rgb);

            image* mask = &vMask->screen;
            glm::vec2 v = glm::vec2(-(mask->data[(x + 1) + (y * mask->width)] - 128), mask->data[(x) + (y * mask->width)] - 128);
            if(glm::length(v) == 0)
            {
                kern.makeIdentity();
            }
            else
            {
                glm::vec2 nv = glm::normalize(v);
                LineFunction l = LineFunction(nv, glm::vec2(0.0f, 0.0f));
                kern.setFExactWeights(&l);
            }

            for(int i = 0; i < kern.height; i++)
            {
                for(int j = 0; j < kern.width; j++)
                {

                    int xk = x + ((j - kern.wr) * screen.unitbytes);
                    int yk = y + ((i - kern.hr) * screen.unitbytes);
                    xk = abs(xk);
                    yk = abs(yk);
                    if(!(xk < 0 || xk > screen.width * screen.unitbytes || yk < 0 || yk > screen.height * screen.unitbytes) && kern.weights[i][j] != 0)
                    {
                        rgb.r = screen.data[xk + (yk * screen.width)] * kern.weights[i][j];
                        rgb.g = screen.data[(xk + 1) + (yk * screen.width)] * kern.weights[i][j];
                        rgb.b = screen.data[(xk + 2) + (yk * screen.width)] * kern.weights[i][j];
                        rgb = rgb / 255.0f;
                        holdHSV = rgbtohsv(rgb);
                        if(hsv.z > holdHSV.z) hsv.z = holdHSV.z;
                    }
                }
            }

            rgb = hsvtorgb(hsv);

            filterScreen.data[x + (y * screen.width)] = rgb.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = rgb.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = rgb.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::alphaLayer(Imagemanip* vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec4 rgb(1.0f);
            glm::vec4 rgb2(1.0f);
            image* mask = &(vMask->screen);

            //get rgba values
            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb.a = screen.data[(x + 3) + (y * screen.width)];
            rgb = rgb / 255.0f;

            rgb2.r = mask->data[x + (y * screen.width)];
            rgb2.g = mask->data[(x + 1) + (y * screen.width)];
            rgb2.b = mask->data[(x + 2) + (y * screen.width)];
            rgb2.a = mask->data[(x + 3) + (y * screen.width)];
            rgb2 = rgb2 / 255.0f;


            //premultiply rgba values
            rgb.r = rgb.r * rgb.a;
            rgb.g = rgb.g * rgb.a;
            rgb.b = rgb.b * rgb.a;

            rgb2.r = rgb2.r * rgb2.a;
            rgb2.g = rgb2.g * rgb2.a;
            rgb2.b = rgb2.b * rgb2.a;

            //calculate this screen over vMask screen
            rgb.r = rgb2.r + rgb.r * (1 - rgb2.a);
            rgb.g = rgb2.g + rgb.g * (1 - rgb2.a);
            rgb.b = rgb2.b + rgb.b * (1 - rgb2.a);
            rgb.a = rgb2.a + rgb.a * (1 - rgb2.a);



            filterScreen.data[x + (y * screen.width)] = (rgb.r / rgb.a) * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = (rgb.g / rgb.a) * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = (rgb.b / rgb.a) * 255;
            //the alpha is always at max for now.
            if(rgb.a * 255 > 255) filterScreen.data[(x + 3) + (y * screen.width)] = 255;
            else filterScreen.data[(x + 3) + (y * screen.width)] = rgb.a * 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::multiplyLayer(Imagemanip* vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec4 rgb(1.0f);
            glm::vec4 rgb2(1.0f);
            image* mask = &vMask->screen;

            //get rgba values
            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb.a = screen.data[(x + 3) + (y * screen.width)];
            rgb = rgb / 255.0f;

            rgb2.r = mask->data[x + (y * screen.width)];
            rgb2.g = mask->data[(x + 1) + (y * screen.width)];
            rgb2.b = mask->data[(x + 2) + (y * screen.width)];
            rgb2.a = mask->data[(x + 3) + (y * screen.width)];
            rgb2 = rgb2 / 255.0f;

            //perform layer multiplication
            rgb2.r = rgb.r * rgb2.r;
            rgb2.g = rgb.g * rgb2.g;
            rgb2.b = rgb.b * rgb2.b;

            //premultiply rgba values
            rgb.r = rgb.r * rgb.a;
            rgb.g = rgb.g * rgb.a;
            rgb.b = rgb.b * rgb.a;

            rgb2.r = rgb2.r * rgb2.a;
            rgb2.g = rgb2.g * rgb2.a;
            rgb2.b = rgb2.b * rgb2.a;

            //calculate this screen over vMask screen
            rgb = rgb2 + rgb * (1 - rgb2.a);
            rgb.a = rgb2.a + rgb.a * (1 - rgb2.a);



            filterScreen.data[x + (y * screen.width)] = (rgb.r / rgb.a) * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = (rgb.g / rgb.a) * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = (rgb.b / rgb.a) * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = rgb.a * 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::subtractionLayer(Imagemanip* vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec4 rgb(1.0f);
            glm::vec4 rgb2(1.0f);
            image* mask = &vMask->screen;

            //get rgba values
            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb.a = screen.data[(x + 3) + (y * screen.width)];
            rgb = rgb / 255.0f;

            rgb2.r = mask->data[x + (y * screen.width)];
            rgb2.g = mask->data[(x + 1) + (y * screen.width)];
            rgb2.b = mask->data[(x + 2) + (y * screen.width)];
            rgb2.a = mask->data[(x + 3) + (y * screen.width)];
            rgb2 = rgb2 / 255.0f;

            //perform layer subtraction
            rgb2.r = abs(rgb2.r - rgb.r);
            rgb2.g = abs(rgb2.g - rgb.g);
            rgb2.b = abs(rgb2.b - rgb.b);

            //premultiply rgba values
            rgb.r = rgb.r * rgb.a;
            rgb.g = rgb.g * rgb.a;
            rgb.b = rgb.b * rgb.a;

            rgb2.r = rgb2.r * rgb2.a;
            rgb2.g = rgb2.g * rgb2.a;
            rgb2.b = rgb2.b * rgb2.a;

            //calculate this screen over vMask screen
            rgb = rgb2 + rgb * (1 - rgb2.a);
            rgb.a = rgb2.a + rgb.a * (1 - rgb2.a);



            filterScreen.data[x + (y * screen.width)] = (rgb.r / rgb.a) * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = (rgb.g / rgb.a) * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = (rgb.b / rgb.a) * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = rgb.a * 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::minLayer(Imagemanip* vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec4 rgb(1.0f);
            glm::vec4 rgb2(1.0f);
            glm::vec3 hsv, hsv2;
            image* mask = &vMask->screen;

            //get rgba values
            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb.a = screen.data[(x + 3) + (y * screen.width)];
            rgb = rgb / 255.0f;

            hsv = rgbtohsv(glm::vec3(rgb.r, rgb.g, rgb.b));

            rgb2.r = mask->data[x + (y * screen.width)];
            rgb2.g = mask->data[(x + 1) + (y * screen.width)];
            rgb2.b = mask->data[(x + 2) + (y * screen.width)];
            rgb2.a = mask->data[(x + 3) + (y * screen.width)];
            rgb2 = rgb2 / 255.0f;

            hsv2 = rgbtohsv(glm::vec3(rgb2.r, rgb2.g, rgb2.b));

            //get the minimum color for each channel
            if(hsv2.z > hsv.z)
            {
                rgb2.r = rgb.r;
                rgb2.g = rgb.g;
                rgb2.b = rgb.b;
            }

            //premultiply rgba values
            rgb.r = rgb.r * rgb.a;
            rgb.g = rgb.g * rgb.a;
            rgb.b = rgb.b * rgb.a;

            rgb2.r = rgb2.r * rgb2.a;
            rgb2.g = rgb2.g * rgb2.a;
            rgb2.b = rgb2.b * rgb2.a;

            //calculate this screen over vMask screen
            rgb = rgb2 + rgb * (1 - rgb2.a);
            rgb.a = rgb2.a + rgb.a * (1 - rgb2.a);



            filterScreen.data[x + (y * screen.width)] = (rgb.r / rgb.a) * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = (rgb.g / rgb.a) * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = (rgb.b / rgb.a) * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = rgb.a * 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::maxLayer(Imagemanip* vMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec4 rgb(1.0f);
            glm::vec4 rgb2(1.0f);
            glm::vec3 hsv, hsv2;
            image* mask = &vMask->screen;

            //get rgba values
            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb.a = screen.data[(x + 3) + (y * screen.width)];
            rgb = rgb / 255.0f;

            hsv = rgbtohsv(glm::vec3(rgb.r, rgb.g, rgb.b));

            rgb2.r = mask->data[x + (y * screen.width)];
            rgb2.g = mask->data[(x + 1) + (y * screen.width)];
            rgb2.b = mask->data[(x + 2) + (y * screen.width)];
            rgb2.a = mask->data[(x + 3) + (y * screen.width)];
            rgb2 = rgb2 / 255.0f;

            hsv2 = rgbtohsv(glm::vec3(rgb2.r, rgb2.g, rgb2.b));

            //get the minimum color for each channel
            if(hsv2.z < hsv.z)
            {
                rgb2.r = rgb.r;
                rgb2.g = rgb.g;
                rgb2.b = rgb.b;
            }

            //premultiply rgba values
            rgb.r = rgb.r * rgb.a;
            rgb.g = rgb.g * rgb.a;
            rgb.b = rgb.b * rgb.a;

            rgb2.r = rgb2.r * rgb2.a;
            rgb2.g = rgb2.g * rgb2.a;
            rgb2.b = rgb2.b * rgb2.a;

            //calculate this screen over vMask screen
            rgb = rgb2 + rgb * (1 - rgb2.a);
            rgb.a = rgb2.a + rgb.a * (1 - rgb2.a);



            filterScreen.data[x + (y * screen.width)] = (rgb.r / rgb.a) * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = (rgb.g / rgb.a) * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = (rgb.b / rgb.a) * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = rgb.a * 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::composite(Imagemanip * outerMask, Imagemanip * innerMask)
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec4 rgb(1.0f);
            glm::vec4 rgb2(1.0f);
            glm::vec4 rgb3(1.0f);
            image* mask = &outerMask->screen;
            image* mask2 = &innerMask->screen;

            //get rgba values
            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb.a = screen.data[(x + 3) + (y * screen.width)];
            rgb = rgb / 255.0f;

            rgb2.r = mask->data[x + (y * screen.width)];
            rgb2.g = mask->data[(x + 1) + (y * screen.width)];
            rgb2.b = mask->data[(x + 2) + (y * screen.width)];
            rgb2.a = mask->data[(x + 3) + (y * screen.width)];
            rgb2 = rgb2 / 255.0f;

            rgb3.r = mask2->data[x + (y * screen.width)];
            rgb3.g = mask2->data[(x + 1) + (y * screen.width)];
            rgb3.b = mask2->data[(x + 2) + (y * screen.width)];
            rgb3.a = mask2->data[(x + 3) + (y * screen.width)];
            rgb3 = rgb3 / 255.0f;

            if(rgb2.a == 1) rgb.a = 0;
            if(rgb3.a == 1) rgb.a = 1;
            if(rgb2.a != 1 && rgb3.a != 1)
            {
                int rx;
                int ry;
                int radius = 200;
                glm::vec4 holdVec(0.0f);
                float holdAlpha;
                int samples = 0;
                int goodSamples = 0;
                while(samples < 200)
                {
                    rx = x + ((rand() % radius) - radius / 2);
                    if(rx < 0) rx = 0;
                    else if(rx > screen.width) rx = screen.width;
                    ry = y + ((rand() % radius) - radius / 2);
                    if(ry < 0) ry = 0;
                    else if(ry > screen.height) ry = 0;
                    holdAlpha = innerMask->screen.data[(rx + 3) + (ry * screen.width)];
                    holdAlpha = holdAlpha / 255.0f;
                    if(holdAlpha == 1)
                    {
                        holdVec.r += screen.data[rx + (ry * screen.width)];
                        holdVec.g += screen.data[(rx + 1) + (ry * screen.width)];
                        holdVec.b += screen.data[(rx + 2) + (ry * screen.width)];
                        holdVec.a += screen.data[(rx + 3) + (ry * screen.width)];
                        holdVec = holdVec / 255.0f;
                        goodSamples++;
                    }
                    samples++;
                }
                holdVec = holdVec / (float)goodSamples;

                float max = holdVec.r;
                float difference;
                if(max < holdVec.g) max = holdVec.g;
                if(max < holdVec.b) max = holdVec.b;
                if(max == holdVec.r)
                {
                    difference = std::abs(rgb.r - holdVec.r);
                }
                else if (max == holdVec.g)
                {
                    difference = std::abs(rgb.g - holdVec.g);
                }
                else if (max == holdVec.b)
                {
                    difference = std::abs(rgb.b - holdVec.b);
                }
                rgb.a = 1 - difference;
            }


            filterScreen.data[x + (y * screen.width)] = (rgb.r) * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = (rgb.g) * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = (rgb.b) * 255;

            filterScreen.data[(x + 3) + (y * screen.width)] = rgb.a * 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::oDither(int pow)
{
    int mult = std::pow(2, pow);
    int size = 4 * (mult * mult);
    int side = (float)std::sqrt(size);
    int** pseudoKern = beyesMat(pow);
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec3 rgb(1.0f);
            glm::vec3 hsv(0.0f);

            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb = rgb / 255.0f;
            hsv = rgbtohsv(rgb);

            if(hsv.z * size < pseudoKern[(y / screen.unitbytes) % side][(x / screen.unitbytes) % side])
            {
                rgb.r = 0;
                rgb.g = 0;
                rgb.b = 0;
            }
            else
            {
                rgb.r = 1.0f;
                rgb.g = 1.0f;
                rgb.b = 1.0f;
            }

            filterScreen.data[x + (y * screen.width)] = rgb.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = rgb.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = rgb.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::cDither(int r)
{
    Kernel sphere(r, r);
    SphereFunction f;
    f.setPoint(glm::vec2(r / 2.0f));
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes * sphere.height)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes * sphere.width)
        {
            float average = 0;
            for(int u = 0; u < sphere.height; u++)
            {
                for(int v = 0; v < sphere.width; v++)
                {
                    int yu = y + (u * screen.unitbytes);
                    int xv = x + (v * screen.unitbytes);
                    if(yu < screen.height * screen.unitbytes && xv < screen.width * screen.unitbytes)
                    {
                        glm::vec3 rgb(1.0f);
                        glm::vec3 hsv(0.0f);

                        rgb.r = screen.data[xv + (yu * screen.width)];
                        rgb.g = screen.data[(xv + 1) + (yu * screen.width)];
                        rgb.b = screen.data[(xv + 2) + (yu * screen.width)];
                        rgb = rgb / 255.0f;
                        hsv = rgbtohsv(rgb);
                        average += hsv.z;
                    }
                }
            }
            average = average / (sphere.width * sphere.height);
            f.setNormal(glm::vec2(average * r, 0.0f));
            sphere.setFExact(&f);

            for(int u = 0; u < sphere.height; u++)
            {
                for(int v = 0; v < sphere.width; v++)
                {
                    int yu = y + (u * screen.unitbytes);
                    int xv = x + (v * screen.unitbytes);

                    if(yu < screen.height * screen.unitbytes && xv < screen.width * screen.unitbytes)
                    {
                         filterScreen.data[xv + (yu * screen.width)] = sphere.weights[u][v] * 255;
                         filterScreen.data[(xv + 1) + (yu * screen.width)] = sphere.weights[u][v] * 255;
                         filterScreen.data[(xv + 2) + (yu * screen.width)] = sphere.weights[u][v] * 255;
                    }
                }
            }
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::fsDither()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            glm::vec3 rgb(1.0f);
            glm::vec3 rgbn(1.0f);
            glm::vec3 l(1.0f);
            glm::vec3 rd(1.0f);
            glm::vec3 d(1.0f);
            glm::vec3 ld(1.0f);
            float err;
            glm::vec3 hsv(0.0f);

            rgb.r = screen.data[x + (y * screen.width)];
            rgb.g = screen.data[(x + 1) + (y * screen.width)];
            rgb.b = screen.data[(x + 2) + (y * screen.width)];
            rgb = rgb / 255.0f;
            hsv = rgbtohsv(rgb);

            if(hsv.z < 0.5)
            {
                rgbn.r = 0;
                rgbn.g = 0;
                rgbn.b = 0;
            }
            else
            {
                rgbn.r = 1;
                rgbn.g = 1;
                rgbn.b = 1;
            }

            err = (hsv.z) - (rgbn.r);

            if( x / screen.unitbytes < screen.width - 1)
            {
                l.r = screen.data[(x + screen.unitbytes) + (y * screen.width)];
                l.g = screen.data[((x + 1) + screen.unitbytes) + (y * screen.width)];
                l.b = screen.data[((x + 2) + screen.unitbytes) + (y * screen.width)];
                l = l / 255.0f;
            }
            if( y / screen.unitbytes < screen.height - 1)
            {
                if( x / screen.unitbytes > 0)
                {
                    rd.r = screen.data[(x - screen.unitbytes) + ((y + screen.unitbytes) * screen.width)];
                    rd.g = screen.data[((x + 1) - screen.unitbytes) + ((y + screen.unitbytes) * screen.width)];
                    rd.b = screen.data[((x + 2) - screen.unitbytes) + ((y + screen.unitbytes) * screen.width)];
                    rd = rd / 255.0f;
                }
                d.r = screen.data[(x) + ((y + screen.unitbytes) * screen.width)];
                d.g = screen.data[(x + 1) + ((y + screen.unitbytes) * screen.width)];
                d.b = screen.data[(x + 2) + ((y + screen.unitbytes) * screen.width)];
                d = d / 255.0f;
                if( x / screen.unitbytes < screen.width - 1)
                {
                    ld.r = screen.data[(x + screen.unitbytes) + ((y + screen.unitbytes) * screen.width)];
                    ld.g = screen.data[((x + 1) + screen.unitbytes) + ((y + screen.unitbytes) * screen.width)];
                    ld.b = screen.data[((x + 2) + screen.unitbytes) + ((y + screen.unitbytes) * screen.width)];
                    ld = ld / 255.0f;
                }
            }

            if(glm::length(l) > 0) l = glm::normalize(l) * (glm::length(l) + err * (7.0f / 16.0f));
            else l = glm::vec3(0.0f) + err * (7.0f / 16.0f);
            //if(glm::length(l) < 0) l = glm::vec3(0.0f);
            //if(glm::length(l) > 1) l = glm::normalize(l);
            if(glm::length(rd) > 0) rd = glm::normalize(rd) * (glm::length(rd) + err * (3.0f / 16.0f));
            else rd = glm::vec3(0.0f) + err * (3.0f / 16.0f);
            //if(glm::length(rd) < 0) rd = glm::vec3(0.0f);
            //if(glm::length(rd) > 1) rd = glm::normalize(rd);
            if(glm::length(d) > 0) d = glm::normalize(d) * (glm::length(d) + err * (5.0f / 16.0f));
            else d = glm::vec3(0.0f) + err * (5.0f / 16.0f);
            //if(glm::length(d) < 0) d = glm::vec3(0.0f);
            //if(glm::length(d) > 1) d = glm::normalize(d);
            if(glm::length(ld) > 0) ld = glm::normalize(ld) * (glm::length(ld) + err * (1.0f / 16.0f));
            else ld = glm::vec3(0.0f) + err * (1.0f / 16.0f);
            //if(glm::length(ld) < 0) ld = glm::vec3(0.0f);
            //if(glm::length(ld) > 1) ld = glm::normalize(ld);

            if( x / screen.unitbytes < screen.width - 1)
            {
                screen.data[(x + screen.unitbytes) + (y * screen.width)] = l.r * 255;
                screen.data[((x + 1) + screen.unitbytes) + (y * screen.width)] = l.g * 255;
                screen.data[((x + 2) + screen.unitbytes) + (y * screen.width)] = l.b * 255;
            }
            if( y / screen.unitbytes < screen.height - 1)
            {
                if( x / screen.unitbytes > 0)
                {
                    screen.data[(x - screen.unitbytes) + ((y + screen.unitbytes) * screen.width)] = rd.r * 255;
                    screen.data[((x + 1) - screen.unitbytes) + ((y + screen.unitbytes) * screen.width)] = rd.g * 255;
                    screen.data[((x + 2) - screen.unitbytes) + ((y + screen.unitbytes) * screen.width)] = rd.b * 255;
                }
                screen.data[(x) + ((y + screen.unitbytes) * screen.width)] = d.r * 255;
                screen.data[(x + 1) + ((y + screen.unitbytes) * screen.width)] = d.g * 255;
                screen.data[(x + 2) + ((y + screen.unitbytes) * screen.width)] = d.b * 255;
                if( x / screen.unitbytes < screen.width - 1)
                {
                    screen.data[(x + screen.unitbytes) + ((y + screen.unitbytes) * screen.width)] = ld.r * 255;
                    screen.data[((x + 1) + screen.unitbytes) + ((y + screen.unitbytes) * screen.width)] = ld.g * 255;
                    screen.data[((x + 2) + screen.unitbytes) + ((y + screen.unitbytes) * screen.width)] = ld.b * 255;
                }
            }

            filterScreen.data[x + (y * screen.width)] = rgbn.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = rgbn.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = rgbn.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

void Imagemanip::makeNormal(Imagemanip * hMap)
{
    image* hScreen = hMap->getPtr();
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            float dX;
            float dY;
            glm::vec3 N;

            //get the normal by calculating the derivative of the height at the current point (get the difference of the surrounding points on the height map)
            if((x - 1) < 0) dX = 0 - hScreen->data[(x + hScreen->unitbytes) + (y * hScreen->width)];
            else if((x + hScreen->unitbytes) > hScreen->width * hScreen->unitbytes) dX = hScreen->data[(x - hScreen->unitbytes) + (y * hScreen->width)] - 0;
            else
            {
                dX = hScreen->data[(x - hScreen->unitbytes) + ((y) * hScreen->width)] - hScreen->data[(x + hScreen->unitbytes) + ((y) * hScreen->width)];
            }
            if((y - 1)  * hScreen->width < 0) dY = 0 - hScreen->data[x + ((y + hScreen->unitbytes) * hScreen->width)];
            else if((y + hScreen->unitbytes) * hScreen->width > hScreen->height * hScreen->width * hScreen->unitbytes) dY = hScreen->data[x + ((y - hScreen->unitbytes) * hScreen->width)] - 0;
            else
            {
                dY = hScreen->data[x + ((y - hScreen->unitbytes) * hScreen->width)] - hScreen->data[x + ((y + hScreen->unitbytes) * hScreen->width)];
            }

            //N.r = dX;
            //N.g = dY;
            N.r = (dX + 255) / 2.0f;
            N.g = (-dY + 255) / 2.0f;
            N.b = 255 - (std::abs(N.x) + std::abs(N.y)) / 2;

            filterScreen.data[x + (y * screen.width)] = N.r;
            filterScreen.data[(x + 1) + (y * screen.width)] = N.g;
            filterScreen.data[(x + 2) + (y * screen.width)] = N.b;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

//assumes that the image in screen is a height map (i.e. for individual pixels, the rgb values are the same)(i.e. everything is shades of gray)
void Imagemanip::diffLight(Imagemanip* nMap, Imagemanip* hMap, glm::vec3 l)
{
    glm::vec3 N = glm::vec3(0.0f);
    glm::vec3 pL = glm::vec3(0.0f);
    glm::vec3 pS = glm::vec3(0.0f);
    glm::vec3 I = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 C;
    image* nScreen = nMap->getPtr();
    image* hScreen = hMap->getPtr();

    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {

            //get point being shaded.
            pS.x = ((float)x / nScreen->unitbytes) + 0.5f;
            pS.y = ((float)y / nScreen->unitbytes) + 0.5f;
            pS.z = hScreen->data[x + (y * nScreen->width)];

            //get the vector pointing from the shaded point to the light source.
            pL = l - pS;
            pL = glm::normalize(pL);

            N.x = (nScreen->data[x + (y * nScreen->width)] - 127);
            N.y = -(nScreen->data[(x + 1) + (y * nScreen->width)] - 127);
            N.z = nScreen->data[(x + 2) + (y * nScreen->width)];
            N = glm::normalize(N);

            float cos = glm::dot(pL,N);
            if(cos < 0) C = glm::vec3(0.0f);
            else C = glm::vec3(cos);

            filterScreen.data[x + (y * screen.width)] = C.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = C.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = C.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

//assumes that the image in screen is a height map (i.e. for individual pixels, the rgb values are the same)(i.e. everything is shades of gray)
void Imagemanip::specLight(Imagemanip* nMap, Imagemanip* hMap, glm::vec3 l, float angle)
{
    image* nScreen = nMap->getPtr();
    image* hScreen = hMap->getPtr();
    float kA, kD, kS;
    kD = 0.5f;
    kS = 0.5f;
    glm::vec3 N = glm::vec3(0.0f);
    glm::vec3 pL = glm::vec3(0.0f);
    glm::vec3 pS = glm::vec3(0.0f);
    glm::vec3 pE = glm::vec3(nScreen->width / 2, nScreen->height / 2, 256.0f);
    glm::vec3 I = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 R;
    glm::vec3 C;

    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {

            //get point being shaded.
            pS.x = ((float)x / nScreen->unitbytes) + 0.5f;
            pS.y = ((float)y / nScreen->unitbytes) + 0.5f;
            pS.z = hScreen->data[x + (y * nScreen->width)];

            //get the vector pointing from the shaded point to the light source.
            pL = l - pS;
            pL = glm::normalize(pL);

            N.x = (nScreen->data[x + (y * nScreen->width)] - 127);
            N.y = -(nScreen->data[(x + 1) + (y * nScreen->width)] - 127);
            N.z = nScreen->data[(x + 2) + (y * nScreen->width)];
            N = glm::normalize(N);

            float cos1 = glm::dot(pL,N);
            if(cos1 < 0) C = glm::vec3(0.0f);
            else C = glm::vec3(cos1 * kD);

            //I = pE - pS;
            //I = glm::normalize(I);
            R = -I + ((glm::dot(I, N) * N) * 2.0f);
            R = glm::normalize(R);
            float cos2 = glm::dot(pL, R);
            float angCos = std::cos(angle);
            if(cos2 >= angCos)
            {
                cos2 = ((cos2 - angCos) / (1 - angCos));
                C += glm::vec3(cos2 * kS);
            }

            filterScreen.data[x + (y * screen.width)] = C.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = C.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = C.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

/*void Imagemanip::light(Imagemanip* nMap, glm::vec3 l)
{
    glm::vec3 N = glm::vec3(0.0f);
    glm::vec3 pL = glm::vec3(0.0f);
    glm::vec3 pS = glm::vec3(0.0f);
    glm::vec3 I = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 C;

    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            int dX;
            int dY;

            //get point being shaded.
            pS.x = ((float)x / screen.unitbytes) + 0.5f;
            pS.y = ((float)y / screen.unitbytes) + 0.5f;
            pS.z = screen.data[x + (y * screen.width)] / 255.0f;

            //get the vector pointing from the shaded point to the light source.
            pL = l - pS;
            pL = glm::normalize(pL);

            //get the normal by calculating the derivative of the height at the current point (get the difference of the surrounding points on the height map)
            if(x - screen.unitbytes < 0) dX = 0 - screen.data[(x + screen.unitbytes) + (y * screen.width)];
            else if(x + screen.unitbytes > screen.width * screen.unitbytes) dX = screen.data[(x - screen.unitbytes) + (y * screen.width)] - 0;
            else dX = screen.data[x + ((y - screen.unitbytes) * screen.width)] - screen.data[x + ((y + screen.unitbytes) * screen.width)];
            if((y - screen.unitbytes) * screen.width < 0) dY = 0 - screen.data[x + ((y + screen.unitbytes) * screen.width)];
            else if((y + screen.unitbytes) * screen.width > screen.height * screen.unitbytes) dY = screen.data[x + ((y - screen.unitbytes) * screen.width)] - 0;
            else dY = screen.data[x + ((y - screen.unitbytes) * screen.width)] - screen.data[x + ((y + screen.unitbytes) * screen.width)];

            N.x = dX / 255.0f;
            N.y = dY / 255.0f;
            N.z = 1.0f - (std::abs(N.x) + std::abs(N.y)) / 2;

            float cos = glm::dot(pL,N);
            if(cos < 0) C = glm::vec3(0.0f);
            else C = glm::vec3(cos);

            filterScreen.data[x + (y * screen.width)] = C.r * 255;
            filterScreen.data[(x + 1) + (y * screen.width)] = C.g * 255;
            filterScreen.data[(x + 2) + (y * screen.width)] = C.b * 255;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}*/

int** Imagemanip::beyesMat(int pow)
{
    int mult = std::pow(2, pow);
    int prevMult = std::pow(2, pow - 1);
    int size = 4 * (mult * mult);
    int prevSize = 4 *(prevMult * prevMult);
    int side = std::sqrt(size);
    int** cur;
    int** smaller;

    cur = new int*[side];
    for(int i = 0; i < side; i++)
    {
        cur[i] = new int[side];
    }
    if(pow > 0)
    {
        smaller = beyesMat(pow - 1);
        for(int j = 0; j < 2 ; j++)
        {
            for(int i = 0; i < 2 ; i++)
            {
                int add;
                if(j == 0 && i == 0) add = 0;
                else if(j == 0 && i == 1) add = 2;
                else if(j == 1 && i == 0) add = 3;
                else add = 1;
                for(int u = 0; u < side / 2; u++)
                {
                    for(int v = 0; v < side / 2; v++)
                    {
                        cur[u + (side / 2 * j)][v + (side / 2 * i)] = (smaller[u][v] * (size / prevSize)) + add;
                    }
                }
            }
        }
        return cur;
    }
    else
    {
        cur[0][0] = 0.0;
        cur[0][1] = 2.0;
        cur[1][0] = 3.0;
        cur[1][1] = 1.0;
        return cur;
    }
}


void Imagemanip::bdlpf()
{
    for(int y = 0; y < screen.height * screen.unitbytes; y += screen.unitbytes)
    {
        for(int x = 0; x < screen.width * screen.unitbytes; x += screen.unitbytes)
        {
            float agR = 0.0f;
            float agG = 0.0f;
            float agB = 0.0f;
            float agRY = 0.0f;
            float agGY = 0.0f;
            float agBY = 0.0f;
            float agRNY = 0.0f;
            float agGNY = 0.0f;
            float agBNY = 0.0f;
            float agRX = 0.0f;
            float agGX = 0.0f;
            float agBX = 0.0f;
            float agRNX = 0.0f;
            float agGNX = 0.0f;
            float agBNX = 0.0f;

            int xk = x - (kern.wr * screen.unitbytes);
            int xnk = x + (kern.wr * screen.unitbytes);
            if(xk < 0) xk = xnk;
            if(xnk > screen.width * screen.unitbytes) xnk = xk;

            int yk = y - (kern.hr * screen.unitbytes);
            int ynk = y + (kern.hr * screen.unitbytes);
            if(yk < 0) yk = ynk;
            if(ynk > screen.height * screen.unitbytes) ynk = yk;

            agRX += screen.data[xk + (y * screen.width)] * kern.weights[0][kern.hr + 1];
            agGX += screen.data[(xk + 1) + (y * screen.width)] * kern.weights[0][kern.hr + 1];
            agBX += screen.data[(xk + 2) + (y * screen.width)] * kern.weights[0][kern.hr + 1];
            agRNX += screen.data[xnk + (y * screen.width)] * kern.weights[kern.width-1][kern.hr + 1];
            agGNX += screen.data[(xnk + 1) + (y * screen.width)] * kern.weights[kern.width-1][kern.hr + 1];
            agBNX += screen.data[(xnk + 2) + (y * screen.width)] * kern.weights[kern.width-1][kern.hr + 1];

            agRY += screen.data[x + (yk * screen.width)] * kern.weights[kern.wr +1][0];
            agGY += screen.data[(x + 1) + (yk * screen.width)] * kern.weights[kern.wr +1][0];
            agBY += screen.data[(x + 2) + (yk * screen.width)] * kern.weights[kern.wr +1][0];
            agRNY += screen.data[x + (ynk * screen.width)] * kern.weights[kern.wr +1][kern.height - 1];
            agGNY += screen.data[(x + 1) + (ynk * screen.width)] * kern.weights[kern.wr +1][kern.height - 1];
            agBNY += screen.data[(x + 2) + (ynk * screen.width)] * kern.weights[kern.wr +1][kern.height - 1];

            agR = abs(agRX - agRNX) + abs(agGX - agGNX) + abs(agBX - agBNX)/3;
            agG = abs(agGY - agGNY) + abs(agRY - agRNY) + (agBY + agBNY)/3;


            filterScreen.data[x + (y * screen.width)] = ((int)agR + 128) % 256;
            filterScreen.data[(x + 1) + (y * screen.width)] = ((int)agG + 128) % 256;
            //the alpha is always at max for now.
            filterScreen.data[(x + 3) + (y * screen.width)] = 255;
        }
    }
    Imagemanip test = Imagemanip(&filterScreen);
    maskedDilation(&test);
}

void Imagemanip::flipScreens()
{
    unsigned char* hold = screen.data;
    screen.data = filterScreen.data;
    filterScreen.data = hold;
}

glm::vec3 Imagemanip::rgbtohsv(glm::vec3 rgb)
{
    float max = std::max(rgb.r, rgb.g);
    max = std::max(max, rgb.b);
    float min = std::min(rgb.r, rgb.g);
    min = std::min(min, rgb.b);

    float v = max;
    float s;
    float h;
    float delta;

    if(v == 0)
    {
        s = 0;
        h = 0;
    }
    else
    {
        delta = max - min;
        s = delta / max;

        if(delta == 0) h = 0;
        else
        {
            if(rgb.r == max) h = (rgb.g - rgb.b) / delta;
            else if (rgb.g == max) h = 2.0 + (rgb.b - rgb.r) / delta;
            else h = 4.0 + (rgb.r - rgb.g) / delta;
            h = h * 60.0;
            if(h < 0) h = h + 360.0;
        }
    }

    return glm::vec3(h, s, v);
}

glm::vec3 Imagemanip::hsvtorgb(glm::vec3 hsv)
{
    double nh, t, opp, back, forward;
    int i;
    glm::vec3 rgb;

    nh = hsv.x / 60.0;
    i = (int)nh;
    t = nh - i;
    opp = hsv.z * (1.0 - hsv.y);
    back = hsv.z * (1.0 - (hsv.y * t));
    forward = hsv.z * (1.0 - (hsv.y * (1.0 - t)));

    switch(i)
    {
    case 0:
        rgb.r = hsv.z;
        rgb.g = forward;
        rgb.b = opp;
        break;
    case 1:
        rgb.r = back;
        rgb.g = hsv.z;
        rgb.b = opp;
        break;
    case 2:
        rgb.r = opp;
        rgb.g = hsv.z;
        rgb.b = forward;
        break;
    case 3:
        rgb.r = opp;
        rgb.g = back;
        rgb.b = hsv.z;
        break;
    case 4:
        rgb.r = forward;
        rgb.g = opp;
        rgb.b = hsv.z;
        break;
    case 5:
    default:
        rgb.r = hsv.z;
        rgb.g = opp;
        rgb.b = back;
        break;
    }
    return rgb;
}






















