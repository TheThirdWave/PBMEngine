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

void Imagemanip::emptyFunctions()
{
    funcNum = 0;
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
    kern.setFuncWeights(func);
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






















