#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
#define _USE_MATH_DEFINES
#include <math.h> 
#include "ImProcInPlainC.h"
using namespace std;
unsigned char img[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
void First_Assignemnt()
{
    InitializeImage(img);
    // Define points for rectangles
    const int numRectangles = 12;
    s2dPoint points[numRectangles][2];
    unsigned char transparencies[numRectangles];
    unsigned char grayLevels[numRectangles];

    for (int i = 0; i < numRectangles; i++) {
        transparencies[i] = static_cast<unsigned char>((static_cast<float>(i) / numRectangles) * 255);
        grayLevels[i] = static_cast<unsigned char>((static_cast<float>(i) / numRectangles) * 255);
    }

    int width = NUMBER_OF_COLUMNS / (numRectangles + 1);
    int height = NUMBER_OF_ROWS / (numRectangles + 1);

    for (int i = 0; i < numRectangles; i++) {

        points[i][0].X = NUMBER_OF_COLUMNS - i * width;
        points[i][0].Y = NUMBER_OF_ROWS - (i + 1) * height;
        points[i][1].X = NUMBER_OF_COLUMNS - (i + 1) * width;
        points[i][1].Y = NUMBER_OF_ROWS - i * height;

    }

    // Add rectangles to the image and save each step
    for (int i = 0; i < numRectangles; i++) {
        AddGrayRectangle(img, points[i][0], points[i][1], transparencies[i], grayLevels[i]);
        // Save the image after each rectangle is added
        char filename[20];
        if (i % 2)
        {
            sprintf(filename, "grayImg_step%d.bmp", i / 2 + 1);
            StoreGrayImageAsGrayBmpFile(img, filename);
        }
    }

    // Save the final image
    StoreGrayImageAsGrayBmpFile(img, "grayImg_final.bmp");

}