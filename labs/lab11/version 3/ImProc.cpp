#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
using namespace std;  // Explanation below

#define _USE_MATH_DEFINES
#include <math.h>     // For mathematical calculations

// BMP Library
#include "ImProcInPlainC.h"


void AddGrayRectangle(unsigned char image[][NUMBER_OF_COLUMNS], s2dPoint A, s2dPoint B1, unsigned char transparency, unsigned char grayLevel) {
  
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

// Declare Gray Image
unsigned char img[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

int main() {
    // Initialize gray image background to white (=255)
    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++) {
            img[row][col] = 255;
        }
    }

    // Define points for rectangles
    const int numRectangles = 6;
    s2dPoint points[numRectangles][2] = {
    {{-20, 239}, {50, 150}},     // Stand-alone
    {{70, 170}, {150, 100}},   // Stand-alone
    {{190, 120}, {319, 0}},   // Stand-alone
    {{100, 220}, {300, 200}},   // overlap
    {{200, 230}, {315, 210}},   // overlap
    {{210, 238}, {290, 195}},   // overlap

    };
    unsigned char transparencies[numRectangles] = { 50, 100, 150, 20, 150, 100 };
    unsigned char grayLevels[numRectangles] = { 50, 100, 150, 20, 150, 100 };

    //// Add rectangles to the image 
    for (int i = 0; i < numRectangles; i++) {
        AddGrayRectangle(img, points[i][0], points[i][1], transparencies[i], grayLevels[i]);
    }
    StoreGrayImageAsGrayBmpFile(img, "grayImg_11.bmp");
   return 0;
}
