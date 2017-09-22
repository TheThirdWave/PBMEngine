#include "CJRW.h"

#define BIT_DEPTH 8

using namespace std;

ReaderWriter::ReaderWriter()
{
    imgPtr = 0;
}

int ReaderWriter::openPNG(char* filename)
{
	FILE *fPointer = fopen(filename, "rb");
	if(!fPointer)
	{
		printf("File can't be opened\n");
         return -1;
	}

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
	{
		printf("couldn't create png_struct\n");
         return -1;
	}

	png_infop info = png_create_info_struct(png);
	if(!info)
	{
		printf("couldn't create png_info\n");
         return -1;
	}

	if(setjmp(png_jmpbuf(png)))
	{
		printf("some weird problem with setjmp that I don't know what it does.\n");
         return -1;
	}

	png_init_io(png, fPointer);
	png_read_info(png, info);

    width[imgPtr] 		= png_get_image_width(png, info);
    height[imgPtr]		= png_get_image_height(png, info);
    color_type	= png_get_color_type(png, info);
	bit_depth	= png_get_bit_depth(png, info);

    //set the info for the raw image datastruct
    rawImage[imgPtr].width = width[imgPtr];
    rawImage[imgPtr].height = height[imgPtr];
    rawImage[imgPtr].unitbytes = RGBA;
    rawImage[imgPtr].rowsize = new int[sizeof(int) * height[imgPtr]];


	if(bit_depth == 16)
		png_set_strip_16(png);

	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	//We ain't trying to compress stuff in this class so expand any monotone files
	//to the full byte if it's trying to be all efficient with the space.
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	//checks to see if there's any transparency information in the tRNS chunk
	//(the tRNS chunk in a PNG is the chunk that holds transparency information,
	// so basically we're just checking to see if a tRNS chunk exists)
	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	//these color types don't have an alpha value, so we fill that part in to stop the
	//library from doing weird stuff with the formatting, apparently.
	if(color_type == PNG_COLOR_TYPE_RGB ||
	   color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if(color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	 png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

    //set the actual size of the structures that are going to hold the image
    rawImage[imgPtr].size = 0;
    row_pointers[imgPtr] = new png_bytep [sizeof(png_bytep) * height[imgPtr]];
    for(int y = 0; y < height[imgPtr]; y++)
	{
        png_uint_32 hold = png_get_rowbytes(png,info);
        row_pointers[imgPtr][y] = new png_byte [hold];
        rawImage[imgPtr].size += hold;
        rawImage[imgPtr].rowsize[y] = hold;
	}

    png_read_image(png, row_pointers[imgPtr]);

    //pack the image into raw bytes
    int pointer = 0;
    rawImage[imgPtr].data = new unsigned char[sizeof(unsigned char*) * rawImage[imgPtr].size];
    for(int i = 0; i < rawImage[imgPtr].height; i++)
    {
        memcpy((void*)&rawImage[imgPtr].data[pointer], (void*)(row_pointers[imgPtr][i]), rawImage[imgPtr].rowsize[i]);
        pointer += rawImage[imgPtr].rowsize[i];
    }



	fclose(fPointer);
    return imgPtr++;
}

int ReaderWriter::openPPM(char* filename)
{
    ifstream reader;
    reader.open(filename);
    if(reader.is_open())
    {
        char buf[100];
        reader.getline(buf, 99);
        if(strcmp(buf, "CJ's PPM Format") == 0)
        {
            reader.getline(buf, 99);
            rawImage[imgPtr].width = stoi(buf, NULL, 10);
            reader.getline(buf, 99);
            rawImage[imgPtr].height = stoi(buf, NULL, 10);
            reader.getline(buf, 99);
            rawImage[imgPtr].size = stoi(buf, NULL, 10);
            reader.getline(buf, 99);
            rawImage[imgPtr].unitbytes = stoi(buf, NULL, 10);
            reader.getline(buf, 99);
            rawImage[imgPtr].data = new unsigned char[sizeof(unsigned char) * rawImage[imgPtr].size];
            int idx = 0;
            while(!reader.eof())
            {
                reader.get(buf, 3);
                rawImage[imgPtr].data[idx++] = stoi(buf, NULL, 16);
            }

        }
        else
        {
            fprintf(stderr, "Error: bad PPM format.\n");
        }
    }
    else
    {
        fprintf(stderr, "Error: Couldn't open PPM file.\n");
        return -1;
    }

    reader.close();
    return imgPtr++;
}

int ReaderWriter::addImage(image img)
{
    rawImage[imgPtr] = img;
    return imgPtr++;
}

image* ReaderWriter::getImgPtr(int ptr)
{
    return &rawImage[ptr];
}

unsigned char* ReaderWriter::getImgRaw(int ptr)
{
    return rawImage[ptr].data;
}

int ReaderWriter::getImgHeight(int ptr)
{
    return rawImage[ptr].height;
}

int ReaderWriter::getImgWidth(int ptr)
{
    return rawImage[ptr].width;
}

bool ReaderWriter::writePNG(char* filename, int ptr)
{
	int y;

	FILE *fPointer = fopen(filename, "wb");
	if(!fPointer)
	{
        fprintf(stderr, "couldn't open file\n");
		 return false;
	}

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
	{
        fprintf(stderr, "couldn't create png_struct in write\n");
		 return false;
	}

	png_infop info = png_create_info_struct(png);
	if(!info)
	{
        fprintf(stderr, "couldn't create png_info in write\n");
		 return false;
	}
	if(setjmp(png_jmpbuf(png)))
	{
        fprintf(stderr, "some weird problem with setjmp that I don't know what it does.\n");
		 return false;
	}

	png_init_io(png, fPointer);

	//This is basically all the metadata for the png we're writing.
    png_set_IHDR(png, info, rawImage[ptr].width, rawImage[ptr].height, BIT_DEPTH, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png, info);

    png_bytep* rps = createRowPointers(ptr);

    png_write_image(png, rps);
	png_write_end(png, NULL);

    /*for(int y = 0; y < height[ptr]; y++)
	{
        delete rps[y];
    }*/
    delete rps;

	fclose(fPointer);
	return true;
}

bool ReaderWriter::writePPM(char* filename, int ptr)
{
    FILE* fPointer = fopen(filename, "w");
    if(!fPointer)
    {
        fprintf(stderr, "Couldn't open file\n");
        return false;
    }
    int err = 0;
    err = fprintf(fPointer, "CJ's PPM Format\n");
    err = fprintf(fPointer, "%d\n", rawImage[ptr].width);
    err = fprintf(fPointer, "%d\n", rawImage[ptr].height);
    err = fprintf(fPointer, "%d\n", rawImage[ptr].size);
    err = fprintf(fPointer, "%d\n", rawImage[ptr].unitbytes);
    err = fprintf(fPointer, "255 255 255 255\n");
    for(int i = 0; i < rawImage[ptr].size; i++)
    {
        err = fprintf(fPointer, "%.2X", (int)rawImage[ptr].data[i]);
    }
    if(err < 0)
    {
        fprintf(stderr, "Could not write PPM file.\n");
        return false;
    }
    fclose(fPointer);
    return true;
}

png_bytep* ReaderWriter::createRowPointers(int idx)
{
    png_bytep* bps = new png_bytep [sizeof(png_bytep) * rawImage[idx].height];
    for(int y = 0; y < rawImage[idx].height; y++)
    {
        bps[y] = &rawImage[idx].data[y * rawImage[idx].width * rawImage[idx].unitbytes];
    }
    return bps;
}

void ReaderWriter::helloworld()
{
	printf("Hello World\n");
}
