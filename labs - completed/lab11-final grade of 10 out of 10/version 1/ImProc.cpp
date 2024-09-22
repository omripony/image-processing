#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
using namespace std;  // Explanation below

#define _USE_MATH_DEFINES
#include <math.h>     // For mathematical calculations

// BMP Library
#include "ImProcInPlainC.h"

bool checkValidation(s2dPoint p1, s2dPoint p2, unsigned char image[][NUMBER_OF_COLUMNS]) {
    int top = max(p1.Y, p2.Y);
    int bottom = min(p1.Y, p2.Y);
    int left = min(p1.X, p2.X);
    int right = max(p1.X, p2.X);
    if ((0 > p1.X || NUMBER_OF_COLUMNS < p1.X) || (0 > p1.Y || NUMBER_OF_ROWS < p1.Y)) {
        printf("Out of boundaries\n");
        return false;
    }
    printf("Place in boundaries\n");
    for (int row = top; row < bottom; row++) {
        for (int col = left; col < right; col++) {
            if (image[row][col] != 255) {
                printf("This place is occupied\n");
                return false;
            }
        }
    }
    return true;
}

// Adds a rectangle with specified points, gray color, and transparency
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
    {{0, 240}, {50, 150}},     // Stand-alone
    {{70, 170}, {150, 100}},   // Stand-alone
    {{190, 120}, {320, 0}},   // Stand-alone
    {{100, 220}, {300, 200}},   // overlap
    {{200, 230}, {315, 210}},   // overlap
    {{210, 238}, {290, 195}},   // overlap

    };
    unsigned char transparencies[numRectangles] = { 50, 100, 150, 20, 150, 100 };
    unsigned char grayLevels[numRectangles] = { 50, 100, 150, 20, 150, 100 };

    // Add rectangles to the image and save each step
    for (int i = 0; i < numRectangles; i++) {
        AddGrayRectangle(img, points[i][0], points[i][1], transparencies[i], grayLevels[i]);
        // Save the image after each rectangle is added
        char filename[20];
        sprintf_s(filename, "grayImg_step%d.bmp", i + 1);
        StoreGrayImageAsGrayBmpFile(img, filename);
    }

    // Save the final image
    StoreGrayImageAsGrayBmpFile(img, "grayImg_final.bmp");

    // Wait for user to press a key
    WaitForUserPressKey();

    return 0;
}
