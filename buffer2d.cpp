#include "buffer2d.h"

Buffer2D::Buffer2D()
{
    buffer = NULL;
    width = 0;
    height = 0;
    channels = 0;
}

Buffer2D::Buffer2D(int w, int h, int ch)
{
    width = w;
    height = h;
    channels = ch;
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

void Buffer2D::init(int w, int h, int ch)
{
    width = w;
    height = h;
    channels = ch;
    buffer = new float[width * height * channels];
    memset(buffer, 0, sizeof(float)*width*height*channels);
}

void Buffer2D::setDataFloat(float f)
{
    memset(buffer, f, sizeof(float)*width*height*channels);
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
