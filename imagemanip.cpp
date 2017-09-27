#include "imagemanip.h"

Imagemanip::Imagemanip()
{
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

void Imagemanip::initScreen(int width, int height)
{
    screen.width = width;
    screen.height = height;
    screen.unitbytes = RGBA;
    screen.size = width * height * screen.unitbytes;
    screen.data = new unsigned char[sizeof(unsigned char) * screen.size];
    memset(screen.data, 0 , (sizeof(unsigned char) * screen.size));
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
