#ifndef _ReaderWriter_
#define _ReaderWriter_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <png.h>

#include "structpile.h"

class ReaderWriter
{
private:
    int width[MAX_PICTURES];
    int height[MAX_PICTURES];
	png_byte color_type;
	png_byte bit_depth;
    png_bytep *(row_pointers[MAX_PICTURES]);
    image rawImage[MAX_PICTURES];
    int imgPtr;

    png_bytep* createRowPointers(int);

public:
	ReaderWriter(void);
    int openPNG(char*);
    int openPPM(char*);
    int addImage(image);
    bool writePNG(char*, int);
    bool writePPM(char*, int);
    image* getImgPtr(int);
    unsigned char* getImgRaw(int);
    int getImgHeight(int);
    int getImgWidth(int);
    void helloworld(void);
};

#endif
