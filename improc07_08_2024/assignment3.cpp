#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h> 
#include "ImProcInPlainC.h"
using namespace std;
unsigned char img3[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
void third_assigment() { 
    CreateTVtestImage(img3); 
    StoreGrayImageAsGrayBmpFile(img3, "tv_test_img.bmp");
}
double gaussian(double x, double mean, double std_dev) {
    return exp(-0.5 * pow((x - mean) / std_dev, 2.0)) / (std_dev * sqrt(2.0 * M_PI));
};
void CreateTVtestImage(unsigned char img[][NUMBER_OF_COLUMNS]) {
    // Initialize the image to a gray background
    InitializeImage(img);

    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++) {
            img[row][col] = 128;  // Set to gray
        }
    }

    

    // Draw central circular pattern with Gaussian distribution
    int centerX = NUMBER_OF_COLUMNS / 2;
    int centerY = NUMBER_OF_ROWS / 2;
    int radius = min(NUMBER_OF_COLUMNS, NUMBER_OF_ROWS) / 2;
    int stripWidth = 10;  // Width of each strip
    double std_dev = stripWidth / 10.0;  // Standard deviation for Gaussian distribution

    for (int angle = 0; angle < 360; angle += 1) {
        double radians = angle * M_PI / 180.0;
        int color = (angle / stripWidth) % 2 == 0 ? 0 : 255;
        for (int r = 0; r < radius; r++) {
            int x = centerX + static_cast<int>(r * cos(radians));
            int y = centerY + static_cast<int>(r * sin(radians));

            if ((x >= 0 && x < NUMBER_OF_COLUMNS) && (y >= 0 && y < NUMBER_OF_ROWS)) {
                int distance_from_center = r % stripWidth;
                double gaussian_value = gaussian(distance_from_center, stripWidth / 2.0, std_dev);
                int blended_color = static_cast<int>(color * gaussian_value + 128 * (1 - gaussian_value));
                img[y][x] = blended_color;
            }
        }
    }

    //// Add grayscale blocks at the corners and edges
    //int blockSize = 40;

    //// Corners
    //s2dPoint blackCorners[4] = {
    //    {0, 0},
    //    {NUMBER_OF_COLUMNS - blockSize, 0},
    //    {0, NUMBER_OF_ROWS - blockSize},
    //    {NUMBER_OF_COLUMNS - blockSize, NUMBER_OF_ROWS - blockSize}
    //};

    //for (auto& corner : blackCorners) {
    //    s2dPoint oppositeCorner = { corner.X + blockSize, corner.Y + blockSize };
    //    AddGrayRectangle(img, corner, oppositeCorner, 255, 0);
    //}

    //// White inner squares
    //int whiteInnerBlockSize = 20;
    //s2dPoint whiteInnerCorners[4] = {
    //    {blockSize / 2, blockSize / 2},
    //    {NUMBER_OF_COLUMNS - blockSize - whiteInnerBlockSize / 2, blockSize / 2},
    //    {blockSize / 2, NUMBER_OF_ROWS - blockSize - whiteInnerBlockSize / 2},
    //    {NUMBER_OF_COLUMNS - blockSize - whiteInnerBlockSize / 2, NUMBER_OF_ROWS - blockSize - whiteInnerBlockSize / 2}
    //};

    //for (auto& corner : whiteInnerCorners) {
    //    s2dPoint oppositeCorner = { corner.X + whiteInnerBlockSize, corner.Y + whiteInnerBlockSize };
    //    AddGrayRectangle(img, corner, oppositeCorner, 255, 255);
    //}

    //// Middle of edges (top, bottom, left, right)
    //s2dPoint edgeMiddleBlocks[4] = {
    //    {NUMBER_OF_COLUMNS / 2 - blockSize / 2, 0},
    //    {NUMBER_OF_COLUMNS / 2 - blockSize / 2, NUMBER_OF_ROWS - blockSize},
    //    {0, NUMBER_OF_ROWS / 2 - blockSize / 2},
    //    {NUMBER_OF_COLUMNS - blockSize, NUMBER_OF_ROWS / 2 - blockSize / 2}
    //};

    //for (auto& side : edgeMiddleBlocks) {
    //    s2dPoint oppositeSide = { side.X + blockSize, side.Y + blockSize };
    //    AddGrayRectangle(img, side, oppositeSide, 255, 0);
    //}

    //// White blocks on the sides
    //int whiteBlockSize = 20;
    //s2dPoint whiteBlocks[4] = {
    //    {NUMBER_OF_COLUMNS / 2 - whiteBlockSize / 2, whiteBlockSize / 2},
    //    {NUMBER_OF_COLUMNS / 2 - whiteBlockSize / 2, NUMBER_OF_ROWS - whiteBlockSize * 1.5},
    //    {whiteBlockSize / 2, NUMBER_OF_ROWS / 2 - whiteBlockSize / 2},
    //    {NUMBER_OF_COLUMNS - whiteBlockSize * 1.5, NUMBER_OF_ROWS / 2 - whiteBlockSize / 2}
    //};

    //for (auto& block : whiteBlocks) {
    //    s2dPoint oppositeBlock = { block.X + whiteBlockSize, block.Y + whiteBlockSize };
    //    AddGrayRectangle(img, block, oppositeBlock, 255, 255);
    //}
}