#include <iostream>
#include <fstream>
#include <cmath>
#include "ImProcInPlainC.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void AffineTransformNearestNeighbor(unsigned char src[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS], unsigned char dst[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS])
{
    double Sx = 1.13;
    double Sy = 2.13;
    int moveLeft = -40;
    int x = 20;
    int y = 10;
    double theta = -17.0 * M_PI / 180.0;

    double cos_theta = cos(theta);
    double sin_theta = sin(theta);

    // Affine transformation matrix components
    double a11 = Sx * cos_theta;
    double a12 = -Sx * sin_theta;
    double a21 = Sy * sin_theta;
    double a22 = Sy * cos_theta;

    int newRow, newCol;
    double tempRow, tempCol;

    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            // Apply the transformation: translate, rotate, scale, translate back
            tempCol = col + moveLeft; // Move 40 pixels left
            tempRow = row;

            // Rotation and scaling around point (20, 10)
            double transformedCol = a11 * (tempCol - x) + a12 * (tempRow - y) + x;
            double transformedRow = a21 * (tempCol - x) + a22 * (tempRow - y) + y;

            // Nearest neighbor interpolation: round to nearest integer
            newCol = static_cast<int>(round(transformedCol));
            newRow = static_cast<int>(round(transformedRow));

            // Copy pixel value if within bounds
            if (newCol >= 0 && newCol < NUMBER_OF_COLUMNS && newRow >= 0 && newRow < NUMBER_OF_ROWS)
            {
                dst[row][col] = src[newRow][newCol];
            }
            else
            {
                dst[row][col] = 0; // Assign black color to pixels out of bounds
            }
        }
    }
}

void AffineTransformBilinear(unsigned char src[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS], unsigned char dst[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS])
{
    double Sx = 1.13;
    double Sy = 2.13;
    int moveLeft = -40;
    int x = 20;
    int y = 10;
    double theta = -17.0 * M_PI / 180.0;

    double cos_theta = cos(theta);
    double sin_theta = sin(theta);

    // Affine transformation matrix components
    double a11 = Sx * cos_theta;
    double a12 = -Sx * sin_theta;
    double a21 = Sy * sin_theta;
    double a22 = Sy * cos_theta;

    double tempRow, tempCol;
    int newRow, newCol;

    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            // Apply the transformation: translate, rotate, scale, translate back
            tempCol = col + moveLeft; // Move 40 pixels left
            tempRow = row;

            // Rotation and scaling around point (20, 10)
            double transformedCol = a11 * (tempCol - x) + a12 * (tempRow - y) + x;
            double transformedRow = a21 * (tempCol - x) + a22 * (tempRow - y) + y;

            // Calculate the coordinates of the four nearest pixels
            int x0 = static_cast<int>(floor(transformedCol));
            int x1 = x0 + 1;
            int y0 = static_cast<int>(floor(transformedRow));
            int y1 = y0 + 1;

            // Ensure the coordinates are within the bounds of the source image
            if (x0 >= 0 && x1 < NUMBER_OF_COLUMNS && y0 >= 0 && y1 < NUMBER_OF_ROWS)
            {
                // Calculate the distances
                double dx = transformedCol - x0;
                double dy = transformedRow - y0;

                // Get the values of the four nearest pixels
                double I00 = src[y0][x0];
                double I10 = src[y0][x1];
                double I01 = src[y1][x0];
                double I11 = src[y1][x1];

                // Perform bilinear interpolation
                double I0 = I00 * (1 - dx) + I10 * dx;
                double I1 = I01 * (1 - dx) + I11 * dx;
                double I = I0 * (1 - dy) + I1 * dy;

                // Assign the interpolated value to the destination image
                dst[row][col] = static_cast<unsigned char>(round(I));
            }
            else
            {
                dst[row][col] = 0; // Assign black color to pixels out of bounds
            }
        }
    }
}

void main()
{
    unsigned char GrayImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
    unsigned char NearestNeighborImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
    unsigned char BilinearImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

    LoadGrayImageFromGrayBmpFile(GrayImage, "grayImage13.bmp");
    AffineTransformNearestNeighbor(GrayImage, NearestNeighborImage);
    StoreGrayImageAsGrayBmpFile(NearestNeighborImage, "A31.bmp");

    LoadGrayImageFromGrayBmpFile(GrayImage, "grayImage13.bmp");
    AffineTransformBilinear(GrayImage, BilinearImage);
    StoreGrayImageAsGrayBmpFile(BilinearImage, "A32.bmp");

    WaitForUserPressKey();
}
