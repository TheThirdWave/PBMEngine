#include "buffer2d.h"

Buffer2D::Buffer2D()
{
    buffer = NULL;
    width = 0;
    height = 0;
    channels = 0;
}

Buffer2D::Buffer2D(int w, int h, int ch, float cs)
{
    width = w;
    height = h;
    channels = ch;
    cellSize = cs;
    buffer = new float[width * height * channels];
    memset(buffer, 0.0f, sizeof(float)*width*height*channels);
}

Buffer2D::~Buffer2D()
{
    delete[] buffer;
}

float * Buffer2D::getBuf()
{
    return buffer;
}

int Buffer2D::getWidth()
{
    return width;
}

int Buffer2D::getHeight()
{
    return height;
}

int Buffer2D::getNumChannels()
{
    return channels;
}

int Buffer2D::getIndex(int x, int y)
{
    return (x + y * width) * channels;
}

float Buffer2D::getCellSize()
{
    return cellSize;
}

void Buffer2D::init(int w, int h, int ch, float cs)
{
    width = w;
    height = h;
    channels = ch;
    cellSize = cs;
    buffer = new float[width * height * channels];
    memset(buffer, 0, sizeof(float)*width*height*channels);
}

void Buffer2D::setDataFloat(float f)
{
    for(int i = 0; i < width * height * channels; i++)
    {
        buffer[i] = f;
    }
}

void Buffer2D::setDataBuffer(Buffer2D& buf)
{
    float* hold = buf.getBuf();
    width = buf.getWidth();
    height = buf.getHeight();
    channels = buf.getNumChannels();
    buffer = new float[width * height * channels];
    memcpy(buffer, hold, sizeof(float) * width * height * channels);
}

void Buffer2D::setToIndicies()
{
    for(int j = 0; j < height; j++)
    {
    #pragma omp parallel for
        for(int i = 0; i < width; i++)
        {
            glm::vec2 iVec;
            iVec.x = (float)i;
            iVec.y = (float)j;
            iVec = iVec * cellSize;
            int index = i + j * width;
            buffer[index * channels] = iVec.x;
            buffer[index * channels + 1] = iVec.y;
        }
    }
}

void Buffer2D::zeroOut()
{
    memset(buffer, 0, sizeof(float)*width*height*channels);
}

void Buffer2D::readImage(const char * fName)
{
    ImageInput* in = ImageInput::create(fName);
    if (! in) {
        fprintf(stderr, "Couldn't create ImageInput.\n");
        return;
    }
    ImageSpec spec;
    if(!in->open (fName, spec))
    {
        fprintf(stderr, "Couldn't open File.\n");
        return;
    }
    width = spec.width;
    height = spec.height;
    channels = spec.nchannels;
    cellSize = 1;
    float* pixels = new float[width*height*channels];
    buffer = new float[width*height*channels];
    in->read_image (TypeDesc::FLOAT, pixels);
    long index = 0;
    for( int j=0;j<height;j++)
    {
       for( int i=0;i<width;i++ )
       {
          for( int c=0;c<channels;c++ )
          {
             buffer[ (i + width*(height - j - 1))*channels + c ] = pixels[index++];
          }
       }
    }

    in->close ();
    delete in;
    delete[] pixels;
}

void Buffer2D::writeImage(const char * fName)
{
    float* pixels = new float[channels*width*height];
    long index = 0;
    for( int j=0;j<height;j++)
    {
       for( int i=0;i<width;i++ )
       {
          for( int c=0;c<channels;c++ )
          {
             pixels[ (i + width*(height - j - 1))*channels + c ] = buffer[index++];
          }
       }
    }

    ImageOutput *out = ImageOutput::create (fName);
    if( !out )
    {
       cout << "Not able to write an image to file " << fName << endl;
    }
    else
    {
       ImageSpec spec (width, height, channels, TypeDesc::FLOAT);
       spec.attribute("user", "CJ");
       spec.attribute("writer", "writeOIIOImage" );
       out->open (fName, spec);
       out->write_image (TypeDesc::FLOAT, pixels);
       out->close ();
       delete out;
    }
    delete[] pixels;
}

void Buffer2D::readPPM(const char * fName)
{
    ppmStuff d = ppm.readPPM(fName);
    width = d.width;
    height = d.height;
    channels = d.sampleWidth;
    cellSize = 1;
    buffer = new float[width * height * channels];
    for(int i = 0; i < width * height * channels; i++)
    {
        buffer[i] = (float)d.data[i] / 255.0;
    }
}

void Buffer2D::writePPM(const char * fName)
{
    ppmStuff d;
    d.width = width;
    d.height = height;
    d.sampleWidth = channels;
    d.data = new char[width * height * channels];
    for(int i = 0; i < width * height * channels; i++)
    {
        d.data[i] = buffer[i] * 255;
    }
    ppm.writePPM(fName, d);
}

glm::vec2 Buffer2D::makeVec2(int index)
{
    if(channels == 2) return glm::vec2(index * channels, index * channels + 1);
    else return glm::vec2(0.0f);
}

glm::vec3 Buffer2D::makeVec3(int index)
{
    if(channels == 3) return glm::vec3(index * channels, index * channels + 1, index * channels + 2);
    else return glm::vec3(0.0f);
}

glm::vec4 Buffer2D::makeVec4(int index)
{
    if(channels == 4) return glm::vec4(index * channels, index * channels + 1, index * channels + 2, index * channels + 3);
    else return glm::vec4(0.0f);
}
