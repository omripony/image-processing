#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
#define _USE_MATH_DEFINES
#include <math.h> 
#include "ImProcInPlainC.h"
using namespace std;
unsigned char img2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
void Second_assignment()
{
    const int numIllusion = 100;
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



}
void createWhitesIllusion(unsigned char img[][NUMBER_OF_COLUMNS], int numberOfBlackHorizontalStrips, unsigned char grayLevelOfGrayBars) {
    InitializeImage(img);
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

    StoreGrayImageAsGrayBmpFile(img, "white_illusion.bmp");
}
