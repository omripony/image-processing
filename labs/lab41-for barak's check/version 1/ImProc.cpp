#include <stdio.h> // for printf
#include <conio.h> // for getch

#include <iostream> // for cin cout
#include <fstream>  // For file IO
using namespace std; // Explain some day 

// This needed to do Math calculations
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h> 

// BMP Library
#include "ImProcInPlainC.h"

#define BLACKVAL 0
#define WHITEVAL 255

void Contrast_Brightness_change(unsigned char image[][NUMBER_OF_COLUMNS], float c, float b)
{
    unsigned char* ptrToPixels = image[0];
    for (int pixel = 0; pixel < NUMBER_OF_COLUMNS * NUMBER_OF_ROWS; pixel++, ptrToPixels++)
        *ptrToPixels = c * (*ptrToPixels) + b; // c- contrast b -brightness
}

int uniform_distribution(int rangeLow, int rangeHigh) {
    double myRand = rand() / (1.0 + RAND_MAX);
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

void add_dots_to_pic(unsigned char image[][NUMBER_OF_COLUMNS], int num_of_dots, int color)
{
    unsigned char* ptrToPixels = image[0];
    for (int i = 0; i < num_of_dots; i++)
    {
        int pix_place = uniform_distribution(0, NUMBER_OF_ROWS * NUMBER_OF_COLUMNS);
        *(ptrToPixels + pix_place) = color;
    }
}

void Add_Random_white_Lines(unsigned char image[][NUMBER_OF_COLUMNS], int number_Of_Lines, int line_length, int line_width)
{
    for (int i = 0; i < number_Of_Lines; i++)
    {
        unsigned char* ptrToPixels = image[0];

        // Calculate the valid range for pixel placement
        int validRowStart = line_length;
        int validRowEnd = NUMBER_OF_ROWS - line_length - 1;
        int validColStart = 0;
        int validColEnd = NUMBER_OF_COLUMNS - line_width - 1;

        // Ensure the random position keeps the line within boundaries
        int randomRow = uniform_distribution(validRowStart, validRowEnd);
        int randomCol = uniform_distribution(validColStart, validColEnd);

        int pix_place = randomRow * NUMBER_OF_COLUMNS + randomCol;
        int randomWidth = uniform_distribution(1, line_width);

        for (int j = 0; j < randomWidth; j++)
        {
            for (int k = 0; k < line_length; k++)
            {
                int currentRow = randomRow - k;
                int currentCol = randomCol + j;
                if (currentRow >= 0 && currentRow < NUMBER_OF_ROWS && currentCol >= 0 && currentCol < NUMBER_OF_COLUMNS)
                {
                    *(ptrToPixels + currentRow * NUMBER_OF_COLUMNS + currentCol) = 255;
                }
            }
        }
    }
}



unsigned char MedianValue(unsigned char medianBuffer[], int medianBufferSize)
{
    int middle = (medianBufferSize + 1) / 2;
    unsigned char temp;
    for (int i = 0; i < middle; i++)
    {
        for (int j = 1; j < medianBufferSize - 1; j++)
        {
            if (medianBuffer[j] > medianBuffer[j - 1])
            {
                temp = medianBuffer[j];
                medianBuffer[j] = medianBuffer[j - 1];
                medianBuffer[j - 1] = temp;
            }
        }
    }
    return medianBuffer[middle];
}

void DoMedianFiltration(unsigned char src[][NUMBER_OF_COLUMNS], unsigned char dest[][NUMBER_OF_COLUMNS],
                        unsigned char medianBuffer[], int filterHalfWidth, int filterHalfHeight)
{
    int medianBufferSize = (2 * filterHalfWidth + 1) * (2 * filterHalfHeight + 1);
    unsigned char* ptrToPixels = dest[0];
    for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; i++) { *ptrToPixels++ = 0; }
    for (int row = filterHalfHeight; row < NUMBER_OF_ROWS - filterHalfHeight; row++)
    {
        for (int column = filterHalfWidth; column < NUMBER_OF_COLUMNS - filterHalfWidth; column++)
        {
            int counter = 0;
            for (int y = -filterHalfHeight; y <= filterHalfHeight; y++)
            {
                for (int x = -filterHalfWidth; x <= filterHalfWidth; x++)
                {
                    medianBuffer[counter++] = src[row + y][column + x];
                }
            }
            dest[row][column] = MedianValue(medianBuffer, medianBufferSize);
        }
    }
}

void count_num_blk_wht_pix(unsigned char image[][NUMBER_OF_COLUMNS], const char name[],
                            int filterHalfWidth, int filterHalfHeight, int numFilters)
{
    int black_pix = 0;
    int white_pix = 0;

    // Calculate the effective edge thickness
    int edgeThickness = numFilters * max(filterHalfWidth, filterHalfHeight);

    unsigned char* ptrToPixels = image[0];
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            unsigned char pixelValue = *(ptrToPixels + row * NUMBER_OF_COLUMNS + col);
            bool isEdgePixel = (row < edgeThickness || row >= NUMBER_OF_ROWS - edgeThickness ||
                                col < edgeThickness || col >= NUMBER_OF_COLUMNS - edgeThickness);

            if (!isEdgePixel)
            {
                if (pixelValue == BLACKVAL) black_pix++;
                else if (pixelValue == WHITEVAL) white_pix++;
            }
            else if (pixelValue == WHITEVAL)
            {
                white_pix++;
            }
        }
    }
    cout << "in pic: " << name << " number of black pixels is: " << black_pix;
    cout << " number of white pixels is: " << white_pix;
    cout << endl;
}



int main()
{
    srand(time(0));

    unsigned char buffer_3X3[9] = {};
    unsigned char buffer_5X5[25] = {};
    unsigned char buffer_7X7[49] = {};
    unsigned char GrayImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
    unsigned char dst[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

    LoadGrayImageFromTrueColorBmpFile(GrayImage, "clouds.bmp");
    StoreGrayImageAsGrayBmpFile(GrayImage, "clouds1.bmp"); // without contrast and brightness
    Contrast_Brightness_change(GrayImage, 0.92, 8);
    StoreGrayImageAsGrayBmpFile(GrayImage, "A411.bmp");

    count_num_blk_wht_pix(GrayImage, "A411.bmp", 0, 0, 0); // No filters applied yet

    add_dots_to_pic(GrayImage, 70, WHITEVAL);
    add_dots_to_pic(GrayImage, 40, BLACKVAL);
    Add_Random_white_Lines(GrayImage, 6, 30, 3);
    StoreGrayImageAsGrayBmpFile(GrayImage, "A412.bmp");

    count_num_blk_wht_pix(GrayImage, "A412.bmp", 0, 0, 0); // No filters applied yet

    DoMedianFiltration(GrayImage, dst, buffer_3X3, 1, 1);
    StoreGrayImageAsGrayBmpFile(dst, "A413.bmp");
    count_num_blk_wht_pix(dst, "3x3 Filter", 1, 1, 1); // One 3x3 filter applied

    DoMedianFiltration(GrayImage, dst, buffer_5X5, 2, 2);
    StoreGrayImageAsGrayBmpFile(dst, "A414.bmp");
    count_num_blk_wht_pix(dst, "5x5 Filter", 2, 2, 2); // Two filters applied

    DoMedianFiltration(GrayImage, dst, buffer_7X7, 3, 3);
    StoreGrayImageAsGrayBmpFile(dst, "A415.bmp");
    count_num_blk_wht_pix(dst, "7x7 Filter", 3, 3, 3); // Three filters applied

    WaitForUserPressKey();
    return 0;
}
