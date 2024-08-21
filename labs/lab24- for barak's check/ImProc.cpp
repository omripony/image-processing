#include <stdio.h> 
#include <conio.h> 
#include <stdlib.h>
#include <time.h>
#include <iostream> 
#include <fstream>  
#include <vector>   // For storing the centers of Gaussians
#include <math.h>   // For math functions like sqrt and exp

using namespace std;

#define _USE_MATH_DEFINES
#include "ImProcInPlainC.h"

#define myMAXCOLORS 256

struct ROI
{
    int top;
    int bottom;
    int left;
    int right;

    void ClipToBounds(int maxRows, int maxCols)
    {
        if (top < 0) top = 0;
        if (bottom > maxRows) bottom = maxRows;
        if (left < 0) left = 0;
        if (right > maxCols) right = maxCols;
        if (top >= bottom) bottom = top;
        if (left >= right) right = left;
    }
};
int CalculateOtsuThreshold(unsigned char GrayImage[][NUMBER_OF_COLUMNS])
{
    int histogram[256] = { 0 };
    int total_pixels = NUMBER_OF_ROWS * NUMBER_OF_COLUMNS;

    // Step 1: Create a histogram of pixel intensities
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            histogram[GrayImage[row][col]]++;
        }
    }

    // Step 2: Calculate cumulative sums and cumulative means
    float sum = 0;
    for (int t = 0; t < 256; t++)
        sum += t * histogram[t];

    float sumB = 0;
    int wB = 0;
    int wF = 0;

    float varMax = 0;
    int threshold = 0;

    for (int t = 0; t < 256; t++)
    {
        wB += histogram[t];  // Weight Background
        if (wB == 0)
            continue;

        wF = total_pixels - wB;  // Weight Foreground
        if (wF == 0)
            break;

        sumB += (float)(t * histogram[t]);

        float mB = sumB / wB;  // Mean Background
        float mF = (sum - sumB) / wF;  // Mean Foreground

        // Step 3: Calculate Between Class Variance
        float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);

        // Step 4: Check if new maximum found
        if (varBetween > varMax)
        {
            varMax = varBetween;
            threshold = t;
        }
    }

    return threshold;
}

void InitThresholdLUT(unsigned char* LUT, unsigned char Threshold, int B_Or_W)
{
    for (int i = 0; i < myMAXCOLORS; i++)
        LUT[i] = (255 - B_Or_W * 255) * (i > Threshold) + 255 * (i <= Threshold && B_Or_W);
}

void ImposeLUT(unsigned char GrayImage[][NUMBER_OF_COLUMNS], unsigned char* LUT)
{
    unsigned char* ptrToPixels = GrayImage[0];
    for (int pixel = 0; pixel < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; pixel++)
        *ptrToPixels++ = LUT[*ptrToPixels];
}

void DrawGaussian(unsigned char img[][NUMBER_OF_COLUMNS], int centerX, int centerY, float sigmaX, float sigmaY)
{
    double a, b, c;
    unsigned char d;
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
        {
            a = (column - centerX) / sigmaX;
            b = (row - centerY) / sigmaY;
            c = 255.0 * exp(-a * a - b * b);
            d = (unsigned char)(int)(c + 0.5);
            img[row][column] = d * (d > 10) + img[row][column] * (d <= 10);
        }
    }
}


int CountElementsInROI(unsigned char GrayImage[][NUMBER_OF_COLUMNS], ROI MyROI)
{
    int CountElem = 0;
    unsigned char* PtrToPixel;
    MyROI.ClipToBounds(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);

    for (int row = MyROI.top; row < MyROI.bottom; row++)
    {
        PtrToPixel = GrayImage[0] + row * NUMBER_OF_COLUMNS + MyROI.left;
        for (int col = MyROI.left; col < MyROI.right; col++)
        {
            if (*PtrToPixel < 127)  // Thresholding to identify blackish objects
            {
                CountElem++;
                *PtrToPixel = 50;    // Mark visited areas
            }
            else
            {
                *PtrToPixel = 200;   // Mark non-object areas
            }
            PtrToPixel += 1;
        }
    }
    return CountElem;
}


/* Function to calculate distance between two points */
double CalculateDistance(int x1, int y1, int x2, int y2)
{
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

// Declare your global variables for image storage
unsigned char ProccesIMG[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char gaussian[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char LUT[256];

// Function to measure the size of the largest element (width and height)
void mesure_elmetns_size(unsigned char* img, int threshold, int* max_X, int* max_Y)
{
    int currentWidth = 0;
    int currentHeight = 0;

    *max_X = 0;  // Reset max_X to 0
    *max_Y = 0;  // Reset max_Y to 0

    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            if (*(img + row * NUMBER_OF_COLUMNS + col) < threshold)  // If pixel is part of an element
            {
                // Measure width (size in X direction)
                currentWidth = 0;
                for (int i = col; i < NUMBER_OF_COLUMNS && *(img + row * NUMBER_OF_COLUMNS + i) < threshold; i++)
                {
                    currentWidth++;
                }

                // Measure height (size in Y direction)
                currentHeight = 0;
                for (int i = row; i < NUMBER_OF_ROWS && *(img + i * NUMBER_OF_COLUMNS + col) < threshold; i++)
                {
                    currentHeight++;
                }

                // Update max_X and max_Y if we found a larger element
                if (currentWidth > *max_X)
                    *max_X = currentWidth;

                if (currentHeight > *max_Y)
                    *max_Y = currentHeight;
            }
        }
    }
}


int FloodFill(unsigned char GrayImage[][NUMBER_OF_COLUMNS], int row, int col)
{
    if (row < 0 || row >= NUMBER_OF_ROWS || col < 0 || col >= NUMBER_OF_COLUMNS)
        return 0;  // Out of bounds

    if (GrayImage[row][col] != 0)  // If it's not an unprocessed element, stop
        return 0;

    // Mark the current pixel as visited
    GrayImage[row][col] = 100;

    // Recursively flood fill in all 4 directions and count the pixels
    int size = 1;
    size += FloodFill(GrayImage, row + 1, col);
    size += FloodFill(GrayImage, row - 1, col);
    size += FloodFill(GrayImage, row, col + 1);
    size += FloodFill(GrayImage, row, col - 1);

    return size;  // Return the size of the connected component
}

bool IsElementInROI_bool(unsigned char GrayImage[][NUMBER_OF_COLUMNS], ROI MyROI, int& element_size)
{
    MyROI.ClipToBounds(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    bool found_flag = false;

    for (int row = MyROI.top; row < MyROI.bottom; row++)
    {
        for (int col = MyROI.left; col < MyROI.right; col++)
        {
            if (GrayImage[row][col] == 0)  // If we find an unvisited element
            {
                element_size = FloodFill(GrayImage, row, col);  // Get the size of the element
                found_flag = true;
                return found_flag;
            }
        }
    }
    return found_flag;
}
void main()
{
    ROI MyROI;
    int BPixelCountElem = 0, BPixelCount = 0;
    time_t t;

    vector<pair<int, int>> centers;  // For storing Gaussian centers
    const int minDistance = 30;      // Minimum distance between Gaussian centers
    const int margin = 30;           // Margin from the edges

    srand((unsigned)time(&t));

    // Part 1: Process Gaussians (untouched)
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
            ProccesIMG[row][col] = 0;

    for (int i = 0; i < 6; i++)
    {
        bool validPosition = false;
        int retries = 0;
        int X, Y;

        while (!validPosition && retries < 100)
        {
            X = margin + rand() % (NUMBER_OF_COLUMNS - 2 * margin);
            Y = margin + rand() % (NUMBER_OF_ROWS - 2 * margin);
            validPosition = true;

            for (const auto& center : centers)
            {
                if (CalculateDistance(X, Y, center.first, center.second) < minDistance)
                {
                    validPosition = false;
                    break;
                }
            }
            retries++;
        }

        centers.push_back({ X, Y });
        DrawGaussian(ProccesIMG, X, Y, 10, 10);
    }

    StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image241.bmp");
    // Set background to black for the gaussian image
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
            gaussian[row][col] = 0;

    InitThresholdLUT(LUT, 200, 1);
    DrawGaussian(gaussian, int(NUMBER_OF_ROWS / 2), int(NUMBER_OF_COLUMNS / 2), 10, 10);

    ImposeLUT(gaussian, LUT);
    MyROI.top = 0;
    MyROI.bottom = NUMBER_OF_ROWS;
    MyROI.left = 0;
    MyROI.right = NUMBER_OF_COLUMNS;
    BPixelCountElem = CountElementsInROI(gaussian, MyROI);

    ImposeLUT(ProccesIMG, LUT);
    BPixelCount = CountElementsInROI(ProccesIMG, MyROI);
    cout << "Found " << int(BPixelCount / BPixelCountElem + 0.5) << " gaussians" << endl;


    // Part 2: Counting objects (tomatoes) in the given image
    LoadGrayImageFromTrueColorBmpFile(ProccesIMG, "Image242_color.bmp");

    // Automatically calculate the threshold using Otsu's method
    int threshold = CalculateOtsuThreshold(ProccesIMG);
    cout << "Calculated Threshold: " << threshold << endl;

    InitThresholdLUT(LUT, threshold, 0);  // Apply the calculated threshold
    ImposeLUT(ProccesIMG, LUT);
    StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image242.bmp");

    // Define ROI for element analysis and count the elements
    int counter_elements = 0;
    int element_size = 0;
    const int size_threshold = 100;  // Minimum size of an element to be considered valid (adjust as needed)
    MyROI.top = 0;
    MyROI.left = 0;

    // Process the image row by row, skipping previously marked areas
    for (int row = 0; row < NUMBER_OF_ROWS; row++)
    {
        for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
        {
            MyROI.top = row;
            MyROI.bottom = row + 1;
            MyROI.left = col;
            MyROI.right = col + 1;

            // Check if this pixel belongs to a new element
            if (ProccesIMG[row][col] == 0)  // Detect unprocessed element
            {
                if (IsElementInROI_bool(ProccesIMG, MyROI, element_size))
                {
                    // Only count the element if its size is greater than the threshold
                    if (element_size > size_threshold)
                    {
                        counter_elements++;
                    }
                }
            }
        }
    }

    cout << "Found " << counter_elements << " elements (tomatoes)" << endl;

    WaitForUserPressKey();
}