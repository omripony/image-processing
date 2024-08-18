#include <stdio.h> 
#include <conio.h> 
#include <stdlib.h>
#include <time.h>
#include <iostream> 
#include <fstream>  
#include <vector>   // For storing the centers of Gaussians

using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>
#include "ImProcInPlainC.h"

#define myMAXCOLORS 256

struct ROI
{
    int top;
    int bottom;
    int left;
    int right;

    void ClipToBounds(int maxRows, int maxCols)
    {
        if (top < 0) top = 0;
        if (bottom > maxRows) bottom = maxRows;
        if (left < 0) left = 0;
        if (right > maxCols) right = maxCols;
        if (top >= bottom) bottom = top;
        if (left >= right) right = left;
    }
};


void DrawGaussian(unsigned char img[][NUMBER_OF_COLUMNS], int centerX, int centerY, float sigmaX, float sigmaY)
{
    double a, b, c;
    unsigned char d;
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
        {
            a = (column - centerX) / sigmaX;
            b = (row - centerY) / sigmaY;
            c = 255.0 * exp(-a * a - b * b);
            d = (unsigned char)(int)(c + 0.5);
            img[row][column] = d * (d > 10) + img[row][column] * (d <= 10);
        }
    }
}


void InitThresholdLUT(unsigned char* LUT, unsigned char Threshold, int B_Or_W)
{
    for (int i = 0; i < myMAXCOLORS; i++)
        LUT[i] = (255 - B_Or_W * 255) * (i > Threshold) + 255 * (i <= Threshold && B_Or_W);
}


void ImposeLUT(unsigned char GrayImage[][NUMBER_OF_COLUMNS], unsigned char* LUT)
{
    unsigned char* ptrToPixels = GrayImage[0];
    for (int pixel = 0; pixel < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; pixel++)
        *ptrToPixels++ = LUT[*ptrToPixels];
}

int ContElemInPicture(unsigned char GrayImage[][NUMBER_OF_COLUMNS], ROI MyROI)
{
    int CountElem = 0;
    unsigned char* PtrToPixel;
    MyROI.ClipToBounds(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);

    for (int row = MyROI.top; row < MyROI.bottom; row++)
    {
        PtrToPixel = GrayImage[0] + row * NUMBER_OF_COLUMNS + MyROI.left;
        for (int col = MyROI.left; col < MyROI.right; col++)
        {
            if (*PtrToPixel < 127)
            {
                CountElem++;
                *PtrToPixel = 50;
            }
            else
                *PtrToPixel = 200;
            PtrToPixel += 1;
        }
    }
    return CountElem;
}

/* Function to calculate distance between two points */
double CalculateDistance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

unsigned char ProccesIMG[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char gaussian[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char LUT[256];

void main()
{
    ROI MyROI;
    int X, Y, BPixelCountElem = 0, BPixelCount = 0;
    time_t t;

    vector<pair<int, int>> centers;  // To store the centers of Gaussians
    const int minDistance = 30; // Ensure sufficient distance between Gaussians
    const int margin = 30; // Margin from the borders

    srand((unsigned)time(&t));

    // Set background to black
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
            ProccesIMG[row][col] = 0;

    // Draw Gaussians ensuring no overlap and no clipping at edges
    for (int i = 0; i < 6; i++)
    {
        bool validPosition = false;
        int retries = 0;

        while (!validPosition && retries < 100)
        {
            X = margin + rand() % (NUMBER_OF_COLUMNS - 2 * margin);
            Y = margin + rand() % (NUMBER_OF_ROWS - 2 * margin);
            validPosition = true;

            // Check distance to all previously placed Gaussians
            for (const auto& center : centers)
            {
                if (CalculateDistance(X, Y, center.first, center.second) < minDistance)
                {
                    validPosition = false;
                    break;
                }
            }

            retries++;
        }

        centers.push_back({ X, Y });
        DrawGaussian(ProccesIMG, X, Y, 10, 10);
    }

    StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image241.bmp");

    // Set background to black for the gaussian image
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
            gaussian[row][col] = 0;

    InitThresholdLUT(LUT, 200, 1);
    DrawGaussian(gaussian, int(NUMBER_OF_ROWS / 2), int(NUMBER_OF_COLUMNS / 2), 10, 10);

    ImposeLUT(gaussian, LUT);
    MyROI.top = 0;
    MyROI.bottom = NUMBER_OF_ROWS;
    MyROI.left = 0;
    MyROI.right = NUMBER_OF_COLUMNS;
    BPixelCountElem = ContElemInPicture(gaussian, MyROI);

    ImposeLUT(ProccesIMG, LUT);
    BPixelCount = ContElemInPicture(ProccesIMG, MyROI);
    cout << "Found " << int(BPixelCount / BPixelCountElem + 0.5) << " gaussians" << endl;

    //******* part 2 of the program - counting objects in a given image **********

    LoadGrayImageFromTrueColorBmpFile(ProccesIMG, "Image242_color.bmp");
    //StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image242_gray.bmp");    //just for debuging - finding the perfect threshold

    InitThresholdLUT(LUT, 214, 0);
    ImposeLUT(ProccesIMG, LUT);
    StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image242.bmp");

    MyROI.top = 185;
    MyROI.bottom = 270;
    MyROI.left = 95;
    MyROI.right = 145;
    BPixelCountElem = ContElemInPicture(ProccesIMG, MyROI);

    MyROI.top = 0;
    MyROI.bottom = NUMBER_OF_ROWS;
    MyROI.left = 0;
    MyROI.right = NUMBER_OF_COLUMNS;
    BPixelCount = ContElemInPicture(ProccesIMG, MyROI);

   /// StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image242.bmp");
    cout << "Found " << int(BPixelCount / BPixelCountElem ) << " elements" << endl;

    WaitForUserPressKey();
}
