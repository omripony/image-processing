#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
#define _USE_MATH_DEFINES
#include <math.h> 
#include "ImProcInPlainC.h"
using namespace std;

unsigned char img2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

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

    const int numIllusion = 10;
    unsigned char transparencies[numIllusion];
    unsigned char grayLevels[numIllusion];
    int numberOfBlackHorizontalStrips = 20;
    for (int i = 0; i < numIllusion; i++) {
        transparencies[i] = static_cast<unsigned char>((static_cast<float>(i) / numIllusion) * 255);
        grayLevels[i] = static_cast<unsigned char>((static_cast<float>(i) / numIllusion) * 255);

        createWhitesIllusion(img2
            , numberOfBlackHorizontalStrips, grayLevels[i]);
        char filename[30];
        sprintf(filename, "white_illusion%d.bmp", i);
        StoreGrayImageAsGrayBmpFile(img2, filename);

    }
    WaitForUserPressKey();


}
void createWhitesIllusion(unsigned char img[][NUMBER_OF_COLUMNS],
                          int numberOfBlackHorizontalStrips, unsigned char grayLevelOfGrayBars) {
    InitializeImage(img, 255);
    int stripHeight = NUMBER_OF_ROWS / numberOfBlackHorizontalStrips;
    for (int i = 0; i < NUMBER_OF_ROWS; i += stripHeight) {
        s2dPoint p1, p2;
        int narrowStripWidth = NUMBER_OF_COLUMNS / 6;
        if ((i / stripHeight) % 2 == 0) {
            // Add black strip
            p1.X = 0;
            p1.Y = i;
            p2.X = NUMBER_OF_COLUMNS;
            p2.Y = i + stripHeight;
            AddGrayRectangle(img, p1, p2, 0, 0);
            // Second narrow strip
            p1.X = 4 * narrowStripWidth;
            p1.Y = i;
            p2.X = 5 * narrowStripWidth;
            p2.Y = i + stripHeight;
            AddGrayRectangle(img, p1, p2, 100, grayLevelOfGrayBars);
        }
        else {
            // Add narrow gray strips within the white strip

            // First narrow strip
            p1.X = narrowStripWidth;
            p1.Y = i;
            p2.X = 2 * narrowStripWidth;
            p2.Y = i + stripHeight;
            AddGrayRectangle(img, p1, p2, 100, grayLevelOfGrayBars);
        }
    }  
}
