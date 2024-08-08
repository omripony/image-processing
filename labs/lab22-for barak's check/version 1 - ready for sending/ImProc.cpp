#include <stdio.h> // for printf
#include <conio.h> // for getch

#include <iostream> // for cin cout
#include <fstream>  // For file IO
using namespace std; // Explain some day 

// This needed to do Math calculations
#define _USE_MATH_DEFINES
#include <math.h>

// BMP Library
#include "ImProcInPlainC.h"
#define PIXEL_VALUES (256)  // PIXEL_VALUES should be 256 for proper indexing
#define PIXEL_MINVAL (0)
#define PIXEL_MAXVAL (255)

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

void ApplyLUTOnGrayImage(unsigned char* img, unsigned char* LUT, int rows, int cols)
{
    for (int i = 0; i < rows * cols; i++)
    {
        *(img + i) = *(LUT + *(img + i));
    }
}

void fill_hist_and_calc_int(unsigned char* img, unsigned* hist, unsigned* IntOfHist, int rows, int cols)
{
    for (int i = 0; i < rows * cols; i++)
    {
        hist[*(img + i)] += 1;
    }
    IntOfHist[0] = hist[0];
    for (unsigned i = 1; i < PIXEL_VALUES; i++)
    {
        *(IntOfHist + i) = *(IntOfHist + i - 1) + *(hist + i);
    }
}

void FindMinMaxFromIntOfHist(unsigned* IntOfHist, unsigned char& min, unsigned char& max, double bottom, double upper)
{
    double bottomCount = bottom * (*(IntOfHist + PIXEL_MAXVAL));
    double topCount = upper * (*(IntOfHist + PIXEL_MAXVAL));
    min = 0;
    max = 0;
    for (unsigned i = 0; i < PIXEL_VALUES; i++)
    {
        if (*(IntOfHist + i) < bottomCount)
            min = i;
        if (*(IntOfHist + i) < topCount)
            max = i;
    }
}

// Preparing Look-Up-Table
void create_percentage_lut(unsigned char* img, unsigned char* LUT, double bottom, double upper, int rows, int cols)
{
    unsigned hist[PIXEL_VALUES] = { 0 };
    unsigned IntOfHist[PIXEL_VALUES] = { 0 };
    fill_hist_and_calc_int(img, hist, IntOfHist, rows, cols);

    unsigned char min = 0, max = 0;
    FindMinMaxFromIntOfHist(IntOfHist, min, max, bottom, upper);
    double c = 255.0 / (max - min);
    double b = -c * min;
    Create_LUT(LUT, c, b);
}

// In this function we insert 20%-80% & 5%-95% values:
void percentage_algorithm(unsigned char* img, double bottom, double upper, int rows, int cols)
{
    create_percentage_lut(img, LUT, bottom, upper, rows, cols);
    ApplyLUTOnGrayImage(img, LUT, rows, cols);
}

// Final stage, execute
int main()
{
    LoadGrayImageFromGrayBmpFile(GrayImage, "A211.bmp");
    percentage_algorithm(&GrayImage[0][0], 0.2, 0.8, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    StoreGrayImageAsGrayBmpFile(GrayImage, "A221.bmp");

    LoadGrayImageFromGrayBmpFile(GrayImage, "A211.bmp");
    percentage_algorithm(&GrayImage[0][0], 0.05, 0.95, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    StoreGrayImageAsGrayBmpFile(GrayImage, "A222.bmp");

    WaitForUserPressKey();
    return 0;
}
