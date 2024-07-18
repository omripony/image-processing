#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
#define _USE_MATH_DEFINES
#include <math.h> 
#include "ImProcInPlainC.h"
using namespace std;
unsigned char img3[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

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
   
    third_assigment();
    WaitForUserPressKey();

    return 0;
}

void third_assigment() {
    CreateTVtestImage(img3);
    StoreGrayImageAsGrayBmpFile(img3, "tv_test_img.bmp");
}



void CreateTVtestImage(unsigned char img[][NUMBER_OF_COLUMNS])
{

    // Initialize the image to a gray background
    InitializeImage(img, 128);

    // Draw central circular pattern with Gaussian distribution
    int centerX = NUMBER_OF_COLUMNS / 2;
    int centerY = NUMBER_OF_ROWS / 2;
    int radius = min(NUMBER_OF_COLUMNS, NUMBER_OF_ROWS) / 2;
    for (double angle = 0; angle < 360; angle += 0.05) {
        double radians = angle * M_PI / 180.0;
        double sinValue = sin(radians * 100); // Sine function for smooth transition
        int color = static_cast<int>((sinValue + 1) * 127.5); // Scale to [0, 255]

        for (int r = 0; r < radius; r++) {
            int x = centerX + static_cast<int>(r * cos(radians));
            int y = centerY + static_cast<int>(r * sin(radians));

            if (x >= 0 && x < NUMBER_OF_COLUMNS && y >= 0 && y < NUMBER_OF_ROWS) {
                img[y][x] = color;
            }
        }
        for (int r = 0; r < radius / 8; r++) {
            int x = centerX + static_cast<int>(r * cos(radians));
            int y = centerY + static_cast<int>(r * sin(radians));

            if (x >= 0 && x < NUMBER_OF_COLUMNS && y >= 0 && y < NUMBER_OF_ROWS) {
                if (angle < 90 || (angle > 180 && angle < 270))
                    img[y][x] = 0;
                else img[y][x] = 255;
            }
        }
    }



    //// Add grayscale blocks at the corners and edges
    int blockSize = 100;

    // Corners
    s2dPoint blackCorners[4] = {
        {0, 0},
        {NUMBER_OF_COLUMNS - blockSize, 0},
        {0, NUMBER_OF_ROWS - blockSize},
        {NUMBER_OF_COLUMNS - blockSize, NUMBER_OF_ROWS - blockSize}
    };

    for (auto& corner : blackCorners) {
        s2dPoint oppositeCorner = { corner.X + blockSize, corner.Y + blockSize };
        AddGrayRectangle(img, corner, oppositeCorner, 0, 0);
    }

    //// White inner squares
    int whiteInnerBlockSize = blockSize / 4;
    s2dPoint whiteInnerCorners[4] = {
        {blockSize / 2 - whiteInnerBlockSize / 2, blockSize / 2 - whiteInnerBlockSize / 2},
        {NUMBER_OF_COLUMNS - blockSize / 2 - whiteInnerBlockSize / 2, blockSize / 2 - whiteInnerBlockSize / 2},
        {blockSize / 2 - whiteInnerBlockSize / 2, NUMBER_OF_ROWS - blockSize / 2 - whiteInnerBlockSize / 2},
        {NUMBER_OF_COLUMNS - blockSize / 2 - whiteInnerBlockSize / 2, NUMBER_OF_ROWS - blockSize / 2 - whiteInnerBlockSize / 2}
    };

    for (auto& corner : whiteInnerCorners) {
        s2dPoint oppositeCorner = { corner.X + whiteInnerBlockSize, corner.Y + whiteInnerBlockSize };
        AddGrayRectangle(img, corner, oppositeCorner, 0, 255);
    }
    int upperGrayS = blockSize * 3 / 4;
    //// Middle of edges (top, bottom, left, right)
    s2dPoint edgeMiddleBlocks[16] = {
        /*bottom right side only*/
        {NUMBER_OF_COLUMNS - upperGrayS,0 + blockSize + upperGrayS},
        {NUMBER_OF_COLUMNS - upperGrayS,0 + blockSize},
        {NUMBER_OF_COLUMNS - blockSize - upperGrayS,0},
        {NUMBER_OF_COLUMNS - blockSize - 2 * upperGrayS,0},
        /*bottom left side only*/
        {0 + blockSize + upperGrayS,0},
        {0 + blockSize ,0},
        {0  ,blockSize},
        {0  ,blockSize + upperGrayS},
        /*upper left side only*/
        {0  ,NUMBER_OF_ROWS - (blockSize + 2 * upperGrayS)},
        {0  ,NUMBER_OF_ROWS - (blockSize + upperGrayS)},
        {blockSize ,NUMBER_OF_ROWS - upperGrayS },
        {blockSize + upperGrayS,NUMBER_OF_ROWS - upperGrayS },
        /*upper right side only*/
        {NUMBER_OF_COLUMNS - blockSize - 2 * upperGrayS,NUMBER_OF_ROWS - upperGrayS },
        {NUMBER_OF_COLUMNS - blockSize - upperGrayS,NUMBER_OF_ROWS - upperGrayS },
        {NUMBER_OF_COLUMNS - upperGrayS,NUMBER_OF_ROWS - blockSize - upperGrayS},
        {NUMBER_OF_COLUMNS - upperGrayS,NUMBER_OF_ROWS - (blockSize + 2 * upperGrayS)}


    };


    int gray_level[16] = { 0 };
    for (int i = 0; i < 8; i++) {
        gray_level[i] = (i + 1) * 32;  // 8 levels of black (from 32 to 256)
    }
    for (int i = 8; i < 16; i++) {
        gray_level[i] = 255 - (i - 8) * 32;  // 8 levels of white (from 255 down to 32)
    }

    int i = 0;
    for (auto& side : edgeMiddleBlocks) {
        s2dPoint oppositeSide = { side.X + upperGrayS, side.Y + upperGrayS };
        AddGrayRectangle(img, side, oppositeSide, 0, gray_level[i]);
        i++;
    }

   
}
