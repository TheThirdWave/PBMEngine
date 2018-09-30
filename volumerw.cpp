#include "volumerw.h"

VolumeRW::VolumeRW()
{

}

Grid<float>* VolumeRW::readScalarGrid(char *fname)
{
    float LLCx, LLCy, LLCz, URCx, URCy, URCz;
    int Nx, Ny, Nz;
    float* data;
    std::string hold;
    std::ifstream in;
    in.open(fname);
    if(in.is_open())
    {
        in >> LLCx >> LLCy >> LLCz >> URCx >> URCy >> URCz >> Nx >> Ny >> Nz;

        data = new float[Nx * Ny * Nz];
        std::getline(in, hold);
        while(!in.eof())
        {
            in.read((char*)data, sizeof(float) * Nx * Ny * Nz);
        }
        in.close();
        Grid<float>* g = new Grid<float>(glm::vec3(LLCx, LLCy, LLCz), glm::vec3(URCx, URCy, URCz), Nx, Ny, Nz);
        g->setData(data);
        return g;
    }
    else
    {
        printf("cannot open file %s", fname);
    }
    return NULL;
}

bool VolumeRW::writeScalarGrid(Grid<float>* dat, char* fname)
{
    glm::vec3 LLC = dat->getLLC();
    glm::vec3 URC = dat->getURC();
    int arr[3];
    dat->getDimensions(arr);
    int Nx = arr[0];
    int Ny = arr[1];
    int Nz = arr[2];

    std::ofstream out;
    out.open(fname);
    if(out.is_open())
    {
        out << LLC.x << " " << LLC.y << " " << LLC.z << "\n";
        out << URC.x << " " << URC.y << " " << URC.z << "\n";
        out << Nx << " " << Ny << " " << Nz << "\n";
        out.write((char*)dat->getData(), sizeof(float) * Nx * Ny * Nz);
        out.close();
    }
    else
    {
        printf("cannot open file %s", fname);
        return false;
    }
    return true;
}
