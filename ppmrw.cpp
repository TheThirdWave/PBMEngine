#include "ppmrw.h"

ppmrw::ppmrw()
{

}

ppmStuff ppmrw::readPPM(const char *filename)
{
    std::string hold;
    int width;
    int height;
    int sampleWidth;
    char* data;
    ppmStuff ret;
    std::ifstream in;
    in.open(filename);
    if(in.is_open())
    {
        std::getline(in, hold);
        if(hold.compare("P6") == 0)
        {
            in >> width >> height;
            in >> sampleWidth;
            if(sampleWidth > 256) sampleWidth = 2 * 3;
            else sampleWidth = 1 * 3;
            data = new char[width * height * sampleWidth];
            std::getline(in, hold);
            while(!in.eof())
            {
                in.read(data, width * height * sampleWidth);
            }
            ret.width = width;
            ret.height = height;
            ret.sampleWidth = sampleWidth;
            ret.data = data;
            in.close();
        }
        else
        {
            printf("wrong file type.");
        }
    }
    else
    {
        printf("cannot open file %s", filename);
    }
    return ret;
}

bool ppmrw::writePPM(const char *filename, ppmStuff& i)
{
    std::ofstream out;
    out.open(filename);
    if(out.is_open())
    {
        out << "P6\n";
        out << i.width << " " << i.height << "\n";
        out << "255\n";
        out.write(i.data, i.width * i.height * i.sampleWidth);
        out.close();
    }
    else
    {
        printf("cannot open file %s", filename);
    }
    return true;
}
