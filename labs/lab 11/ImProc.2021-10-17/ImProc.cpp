#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
#define _USE_MATH_DEFINES
#include <math.h> 
#include "ImProcInPlainC.h"
using namespace std;
//unsigned char img[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char img[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
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



int main() {

    InitializeImage(img, 255);
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

    WaitForUserPressKey();

    return 0;
}
