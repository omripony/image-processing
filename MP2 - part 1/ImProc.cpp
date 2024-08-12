#include <stdio.h> // for printf
#include <conio.h> // for getch
#include <iostream> // for cin cout
#include <fstream>  // For file IO
#include <cmath> // For math calculations

// BMP Library
#include "ImProcInPlainC.h"

using namespace std;

void PrepareGaussianFilter(double filter[], int filter_size, double sigma);
void DoGaussianFiltration(unsigned char* src, double* filter, int filter_size);

// Declare global variables
unsigned char src[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
static unsigned char temp_dest[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS] = { 0 }; // Static array, initialized to zero

int main()
{
    cout << "Gaussian Filter" << endl;

    // Apply a 5x5 Gaussian filter
    const int FILTER_HALF_SIZE_5 = 2;
    const int FILTER_SIZE_5 = 2 * FILTER_HALF_SIZE_5 + 1;
    double filter_5[FILTER_SIZE_5];

    //LoadGrayImageFromTrueColorBmpFile(src, "2603-7596-8509.bmp");  // Load the image
    //PrepareGaussianFilter(filter_5, FILTER_SIZE_5, 1);   // Create the filter 5x5
    //DoGaussianFiltration(&src[0][0], filter_5, FILTER_SIZE_5);  // Perform the filtration
    //StoreGrayImageAsGrayBmpFile(src, "Tim1LPF5c.bmp");  // Save the blurred image with 5x5 filter

    LoadGrayImageFromGrayBmpFile(src, "Tim2.bmp");  // Load the image
    PrepareGaussianFilter(filter_5, FILTER_SIZE_5, 1);   // Create the filter 5x5
    DoGaussianFiltration(&src[0][0], filter_5, FILTER_SIZE_5);  // Perform the filtration
    StoreGrayImageAsGrayBmpFile(src, "Tim2LPF5c.bmp");  // Save the blurred image with 5x5 filter

    // Apply a 7x7 Gaussian filter
    const int FILTER_HALF_SIZE_7 = 3;
    const int FILTER_SIZE_7 = 2 * FILTER_HALF_SIZE_7 + 1;
    double filter_7[FILTER_SIZE_7];

    //LoadGrayImageFromTrueColorBmpFile(src, "2603-7596-8509.bmp");  // Reload the original image
    //PrepareGaussianFilter(filter_7, FILTER_SIZE_7, 1);
    //DoGaussianFiltration(&src[0][0], filter_7, FILTER_SIZE_7);  // Perform the filtration
    //StoreGrayImageAsGrayBmpFile(src, "Tim1LPF7c.bmp");  // Save the blurred image with 7x7 filter

    LoadGrayImageFromGrayBmpFile(src, "Tim2.bmp");  // Load the image
    PrepareGaussianFilter(filter_7, FILTER_SIZE_7, 1);   // Create the filter 7x7
    DoGaussianFiltration(&src[0][0], filter_7, FILTER_SIZE_7);  // Perform the filtration
    StoreGrayImageAsGrayBmpFile(src, "Tim2LPF7c.bmp");  // Save the blurred image with 7x7 filter


    WaitForUserPressKey();
    return 0;
}

void PrepareGaussianFilter(double filter[], int filter_size, double sigma)
{
    int half_size = (filter_size - 1) / 2;
    double summa = 0;
    for (int i = 0; i < filter_size; i++)
    {
        double temp = (double)(i - half_size) / sigma;
        temp = exp(-temp * temp);
        filter[i] = temp;
        summa += temp;
    }
    for (int i = 0; i < filter_size; i++)
    {
        filter[i] /= summa;
    }
}

void DoGaussianFiltration(unsigned char* src, double* filter, int filter_size)  //filteration using convolution - x direction and then y direction
{
    int half_size = (filter_size - 1) / 2;
    double summa;

    memset(temp_dest, 0, sizeof(temp_dest));  //    // Reinitialize temp_dest to zero before each filtration - This ensures that
                                                   //  there are no residual values from previous operations, which could affect the results.

    // x direction
    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int column = half_size; column < NUMBER_OF_COLUMNS - half_size; column++) {
            summa = 0;
            for (int x = -half_size; x <= half_size; x++) {
                summa += filter[half_size + x] * (double)*(src + row * NUMBER_OF_COLUMNS + (column + x));
            }
            if (summa < 0) summa = 0;
            if (summa > 255) summa = 255;
            *(temp_dest[0] + row * NUMBER_OF_COLUMNS + column) = (unsigned char)summa;
        }
    }

    // Clear Source !!!
    for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; i++) {
        src[i] = 0;
    }

    // y direction
    for (int row = half_size; row < NUMBER_OF_ROWS - half_size; row++) {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
            summa = 0;
            for (int y = -half_size; y <= half_size; y++) {
                summa += filter[half_size + y] * *(temp_dest[0] + (row + y) * NUMBER_OF_COLUMNS + column);
            }
            if (summa < 0) summa = 0;
            if (summa > 255) summa = 255;
            *(src + row * NUMBER_OF_COLUMNS + column) = (unsigned char)summa;
        }
    }
}
