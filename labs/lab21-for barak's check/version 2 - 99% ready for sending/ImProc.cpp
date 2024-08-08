#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <cmath>

// BMP Library
#include "ImProcInPlainC.h"

#define PIXEL_VALUES (256)
#define PIXEL_MINVAL (0)
#define PIXEL_MAXVAL (255)

using namespace std;

// Declare Gray Image
unsigned char GrayImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char LUT[PIXEL_VALUES];

void Create_LUT(unsigned char* LUT, double c, double b)
{
    for (unsigned i = 0; i <= PIXEL_MAXVAL; i++)
    {
        double val = c * i + b;
        if (val > PIXEL_MAXVAL)
            *(LUT + i) = PIXEL_MAXVAL;
        else if (val < PIXEL_MINVAL)
            *(LUT + i) = PIXEL_MINVAL;
        else
            *(LUT + i) = val;
    }
}

void ApplyLUTonGrayImage(unsigned char* img, unsigned char* LUT, int rows, int cols)
{
    unsigned char* pixelPtr = img;
    for (int i = 0; i < rows * cols; ++i, ++pixelPtr)
    {
        *pixelPtr = LUT[*pixelPtr];
    }
}

void Spoil_good_looking_Image(unsigned char* img, int rows, int cols)
{
    // from short ID => ABCD=1950 (BARAK'S ID) and EFGH=2210 (OMRI'S ID)

    double c = 0.442;  // c=0.22+0.023*B+0.003*C => ABCD=1950 ==> B=9,C=5 ==> c=0.442
    double b = 30;     // 23+3*F+G => EFGH=2210 ==> F=2,G=1 ==> b=30

    Create_LUT(LUT, c, b);
    ApplyLUTonGrayImage(img, LUT, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
}

void CreateMinMaxLUT(unsigned char* LUT, unsigned char min, unsigned char max)
{
    double c = 255.0 / (max - min);
    double b = -c * min;
    Create_LUT(LUT, c, b);
}

void Min_Max_Algorithm(unsigned char* img, int rows, int cols)
{
    unsigned char min = PIXEL_MAXVAL;
    unsigned char max = PIXEL_MINVAL;
    unsigned char* pixelPtr = img;

    for (int i = 0; i < rows * cols; ++i, ++pixelPtr)
    {
        if (*pixelPtr < min)
            min = *pixelPtr;
        if (*pixelPtr > max)
            max = *pixelPtr;
    }

    CreateMinMaxLUT(LUT, min, max);
    ApplyLUTonGrayImage(img, LUT, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
}

int main()
{
    LoadGrayImageFromTrueColorBmpFile(GrayImage, "A20.bmp");

    Spoil_good_looking_Image(&GrayImage[0][0], NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);

    StoreGrayImageAsGrayBmpFile(GrayImage, "A211.bmp");

    Min_Max_Algorithm(&GrayImage[0][0], NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);

    StoreGrayImageAsGrayBmpFile(GrayImage, "A212.bmp");

    WaitForUserPressKey();
    return 0;
}
