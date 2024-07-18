#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
#define _USE_MATH_DEFINES
#include <math.h> 
#include "ImProcInPlainC.h"
using namespace std;
unsigned char grayImg14_1[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char grayImg14_2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char grayImg14[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];



int main() {

    forth_assignment();
    // Wait for user to press a key
    WaitForUserPressKey();

    return 0;
}
bool checkValidation(s2dPoint p1, s2dPoint p2, unsigned char image[][NUMBER_OF_COLUMNS])
{
    int top = max(p1.Y, p2.Y);
    int bottom = min(p1.Y, p2.Y);
    int left = min(p1.X, p2.X);
    int right = max(p1.X, p2.X);
    if ((0 > p1.X || NUMBER_OF_COLUMNS < p1.X) || (0 > p1.Y || NUMBER_OF_ROWS < p1.Y))
    {
        printf("Out of boundaries\n");
        return false;
    }
    printf("Place in boundaries\n");
    for (int row = top; row < bottom; row++)
    {
        for (int col = left; col < right; col++)
        {
            if (image[row][col] != 255)
            {
                printf("This place is occupied\n");
                return false;
            }
        }
    }
    return true;
}
void AddGrayRectangle(unsigned char image[][NUMBER_OF_COLUMNS], s2dPoint A, s2dPoint B1, unsigned char transparency, unsigned char grayLevel) {
    // Ensure coordinates are within bounds and place is not occupied
    if (!checkValidation(A, B1, image)) {
        return;
    }

    int top = max(A.Y, B1.Y);
    int bottom = min(A.Y, B1.Y);
    int left = min(A.X, B1.X);
    int right = max(A.X, B1.X);

    // Apply blending technique to the region of the rectangle
    for (int row = max(bottom, 0); row < min(top, NUMBER_OF_ROWS); row++) {
        for (int col = max(left, 0); col < min(right, NUMBER_OF_COLUMNS); col++) {
            image[row][col] = static_cast<unsigned char>(transparency * (image[row][col] / 255.0)
                + (255 - transparency) * (grayLevel / 255.0));
        }
    }
}
void InitializeImage(unsigned char image[][NUMBER_OF_COLUMNS], int gray_level) {
    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++) {
            image[row][col] = gray_level;
        }
    }
}

void forth_assignment()
{
    s2dPoint p1(0, 0);
    s2dPoint p2(NUMBER_OF_COLUMNS, NUMBER_OF_ROWS);


    AddGrayRectangle(grayImg14_1, p1, p2, 0, 255);
    DrawSpiral(grayImg14_1);

    // Create Gaussian pattern
    DrawGaussian(grayImg14_2, NUMBER_OF_COLUMNS / 2, NUMBER_OF_ROWS / 2, 120, 100);

    // Blend the images
    blend(grayImg14_1, grayImg14_2, grayImg14);

    // Store images for checking
    StoreGrayImageAsGrayBmpFile(grayImg14_1, "grayImage14_1.bmp");
    StoreGrayImageAsGrayBmpFile(grayImg14_2, "grayImage14_2.bmp");
    StoreGrayImageAsGrayBmpFile(grayImg14, "grayImage14.bmp");

    WaitForUserPressKey();
}

void DrawGaussian(unsigned char img[][NUMBER_OF_COLUMNS], int centerX, int centerY, float sigmaX, float sigmaY)
{

    double a, b, c;
    unsigned char d;
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
        {
            a = (column - centerX) / (sigmaX * 2);
            b = (row - centerY) / (sigmaY * 2);
            c = 255.0 * exp(-a * a - b * b);
            d = static_cast<unsigned char>(c + 0.5);
            img[row][column] = d;
        }
    }
}

void DrawSpiral(unsigned char img[][NUMBER_OF_COLUMNS])
{
    // Initialize the image with a white background
    InitializeImage(img, 255);

    float alfa = 0;
    int rad = 0;
    int centerX = NUMBER_OF_COLUMNS / 2;
    int centerY = NUMBER_OF_ROWS / 2;

    // Determine the maximum radius to ensure the spiral fits within the image boundaries
    int maxRadius = std::min(centerX, centerY);

    // Calculate the increment for alfa to control the density and length of the spiral
    float increment = M_PI / 20000;

    while (rad < maxRadius)
    {
        rad = static_cast<int>(3.5 * alfa);
        for (int i = 0; i < 3; i++)
        {
            int x = static_cast<int>(centerY + (rad + i) * sin(alfa));
            int y = static_cast<int>(centerX + (rad + i) * cos(alfa));
            if (x >= 0 && x < NUMBER_OF_ROWS && y >= 0 && y < NUMBER_OF_COLUMNS)
            {
                img[x][y] = 0;
            }
        }
        alfa += increment;
    }
}

void blend(unsigned char img_src1[][NUMBER_OF_COLUMNS], unsigned char img_src2[][NUMBER_OF_COLUMNS], unsigned char img_dst[][NUMBER_OF_COLUMNS])
{
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
        {
            img_dst[row][column] = static_cast<unsigned char>(0.5 * img_src1[row][column] + 0.5 * img_src2[row][column]);
        }
    }
}


