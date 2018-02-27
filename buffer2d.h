#ifndef BUFFER2D_H
#define BUFFER2D_H

#include <OpenImageIO/imageio.h>

using namespace std;
OIIO_NAMESPACE_USING

class Buffer2D
{
private:
    float* buffer;
    int width, height, channels;
    float cellSize;
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
    void readImage(const char*);
    void writeImage(const char*);

};

#endif // BUFFER2D_H
