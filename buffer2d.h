#ifndef BUFFER2D_H
#define BUFFER2D_H

#include <OpenImageIO/imageio.h>
#include <../glm-0.9.9.1/glm/glm.hpp>

#include "ppmrw.h"

using namespace std;
OIIO_NAMESPACE_USING

class Buffer2D
{
private:
    float* buffer;
    int width, height, channels;
    float cellSize;
    ppmrw ppm;
public:
    Buffer2D();
    Buffer2D(int, int, int, float);
    ~Buffer2D();
    float* getBuf();
    int getWidth();
    int getHeight();
    int getNumChannels();
    float getCellSize();
    void init(int, int, int, float);
    void setDataFloat(float);
    void setDataBuffer(Buffer2D&);
    void setToIndicies();
    void zeroOut();
    void readImage(const char*);
    void writeImage(const char*);
    void readPPM(const char*);
    void writePPM(const char*);

    glm::vec2 makeVec2(int index);
    glm::vec3 makeVec3(int index);

};

#endif // BUFFER2D_H
