#include <stdio.h>    // for printf
#include <conio.h>    // for getch
#include <iostream>   // for cin, cout
#include <fstream>    // for file IO
#include <cmath>
#include <ctime>      // for random seed
#include <cstdlib>    // for rand
#include "ImProcInPlainC.h"

using namespace std;
unsigned char grayImage24_part2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char grayImg24_1[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char grayImg24[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char gauss[6][NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

void InitializeImage(unsigned char image[][NUMBER_OF_COLUMNS], int gray_level);
void DrawGaussian(unsigned char img[][NUMBER_OF_COLUMNS], int centerX, int centerY, float sigmaX, float sigmaY);
void blend(unsigned char img_src1[][NUMBER_OF_COLUMNS], unsigned char img_src2[][NUMBER_OF_COLUMNS], unsigned char img_dst[][NUMBER_OF_COLUMNS]);

int main() {
    srand((unsigned int)time(NULL)); // Initialize random seed

    lab24_assignment_part1();
    lab24_assignment_part2();
    // Wait for user to press a key
    

    return 0;
}

void InitializeImage(unsigned char image[][NUMBER_OF_COLUMNS], int gray_level) {
    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++) {
            image[row][col] = gray_level;
        }
    }
}

bool IsFarEnough(int centerX, int centerY, int previousCenters[][2], int count, int minDistance) {
    for (int i = 0; i < count; i++) {
        int dx = centerX - previousCenters[i][0];
        int dy = centerY - previousCenters[i][1];
        if (sqrt(dx * dx + dy * dy) < minDistance) {
            return false;
        }
    }
    return true;
}

void lab24_assignment_part1() {
    s2dPoint p1(0, 0);
    s2dPoint p2(NUMBER_OF_COLUMNS, NUMBER_OF_ROWS);

    InitializeImage(grayImg24_1, 0);

    int previousCenters[6][2];
    int minDistance = 50;

    // Create 6 Gaussian patterns with random parameters
    for (int i = 0; i < 6; i++) {
        int centerX, centerY;
        do {
            centerX = rand() % NUMBER_OF_COLUMNS;
            centerY = rand() % NUMBER_OF_ROWS;
        } while (!IsFarEnough(centerX, centerY, previousCenters, i, minDistance));

        previousCenters[i][0] = centerX;
        previousCenters[i][1] = centerY;

        float sigmaX = (rand() % 50) + 30; // Random sigmaX between 30 and 80
        float sigmaY = (rand() % 50) + 30; // Random sigmaY between 30 and 80

        DrawGaussian(gauss[i], centerX, centerY, sigmaX, sigmaY);

        // Store each Gaussian image separately
        char filename[20];
        sprintf(filename, "gauss%d.bmp", i + 1);
        StoreGrayImageAsGrayBmpFile(gauss[i], filename);
    }

    // Blend all Gaussian patterns together
    InitializeImage(grayImg24, 127); // Initialize the final image

    for (int i = 0; i < 6; i++) {
        blend(grayImg24, gauss[i], grayImg24);
    }

    // Blend the resulting Gaussian image with the initial spiral image
    blend(grayImg24, grayImg24_1, grayImg24);

    // Store images for checking
    StoreGrayImageAsGrayBmpFile(grayImg24, "Image241.bmp");
}

void DrawGaussian(unsigned char img[][NUMBER_OF_COLUMNS], int centerX, int centerY, float sigmaX, float sigmaY) {
    double a, b, c;
    unsigned char d;
    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
            a = (column - centerX) / (sigmaX * 2);
            b = (row - centerY) / (sigmaY * 2);
            c = 255.0 * exp(-a * a - b * b);
            d = static_cast<unsigned char>(c + 0.5);
            img[row][column] = d;
        }
    }
}

void DrawSpiral(unsigned char img[][NUMBER_OF_COLUMNS]) {
    InitializeImage(img, 255);

    float alfa = 0;
    int rad = 0;
    int centerX = NUMBER_OF_COLUMNS / 2;
    int centerY = NUMBER_OF_ROWS / 2;

    int maxRadius = std::min(centerX, centerY);
    float increment = M_PI / 20000;

    while (rad < maxRadius) {
        rad = static_cast<int>(3.5 * alfa);
        for (int i = 0; i < 3; i++) {
            int x = static_cast<int>(centerY + (rad + i) * sin(alfa));
            int y = static_cast<int>(centerX + (rad + i) * cos(alfa));
            if (x >= 0 && x < NUMBER_OF_ROWS && y >= 0 && y < NUMBER_OF_COLUMNS) {
                img[x][y] = 0;
            }
        }
        alfa += increment;
    }
}

void blend(unsigned char img_src1[][NUMBER_OF_COLUMNS], unsigned char img_src2[][NUMBER_OF_COLUMNS], unsigned char img_dst[][NUMBER_OF_COLUMNS]) {
    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
            img_dst[row][column] = max(img_src1[row][column], img_src2[row][column]) * 0.85;
        }
    }
}


void lab24_assignment_part2() {
    LoadGrayImageFromTrueColorBmpFile(grayImage24_part2, "tomato1.bmp");
}