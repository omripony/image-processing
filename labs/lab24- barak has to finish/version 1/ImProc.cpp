#include <stdio.h> // for printf
#include <conio.h> // for getch
#include <stdlib.h>// for rand
#include <time.h>// time for seed
#include <iostream> // for cin, cout
#include <fstream>  // For file IO
#include <random>   // For better random number generation
#include <chrono>   // For seeding with current time

using namespace std;

// Math calculations
#define _USE_MATH_DEFINES
#include <math.h>

// BMP Library
#include "ImProcInPlainC.h"

#define myMAXCOLORS 256

// Struct for defining a region of interest (ROI)
struct ROI
{
	int top;
	int bottom;
	int left;
	int right;

	// Method to clip the ROI to the bounds of the image
	void ClipToBounds(int maxRows, int maxCols)
	{
		// Clip the ROI's top and bottom to [0, maxRows)
		if (top < 0) top = 0;
		if (bottom > maxRows) bottom = maxRows;

		// Clip the ROI's left and right to [0, maxCols)
		if (left < 0) left = 0;
		if (right > maxCols) right = maxCols;

		// Ensure ROI is valid (top < bottom and left < right)
		if (top >= bottom) bottom = top;
		if (left >= right) right = left;
	}
};

/* Adds a Gaussian distribution to the image at the given coordinates */
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
			img[row][column] = (d > 10) ? d : img[row][column];
		}
	}
}

// Initialize a lookup table (LUT) for thresholding
void InitThresholdLUT(unsigned char* LUT, unsigned char minVal, unsigned char maxVal)
{
	for (int i = 0; i < myMAXCOLORS; i++)
	{
		// If pixel value is within the range [minVal, maxVal], set to white (255)
		// Else, set to black (0)
		if (i >= minVal && i <= maxVal)
			LUT[i] = 255; // White
		else
			LUT[i] = 0;   // Black
	}
}

// Apply the LUT to the grayscale image
void ImposeLUT(unsigned char GrayImage[][NUMBER_OF_COLUMNS], unsigned char* LUT)
{
	unsigned char* ptrToPixels = GrayImage[0];
	for (int pixel = 0; pixel < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; pixel++)
		*ptrToPixels++ = LUT[*ptrToPixels];
}

// Count elements in the image based on the ROI
int ContElemInPicture(unsigned char GrayImage[][NUMBER_OF_COLUMNS], ROI MyROI)
{
	int elementCount = 0; // Variable to count elements
	unsigned char* pixelPtr;

	for (int row = MyROI.top; row < MyROI.bottom; row++)
	{
		pixelPtr = GrayImage[row] + MyROI.left;
		for (int col = MyROI.left; col < MyROI.right; col++, pixelPtr++)
		{
			if (*pixelPtr < 127)
			{
				elementCount++;
				*pixelPtr = 50; // Mark the element
			}
			else
			{
				*pixelPtr = 200; // Mark as background
			}
		}
	}

	return elementCount;
}

// Declare the gray image and example Gaussian image
unsigned char ProccesIMG[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char gaussian[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char LUT[256];

void main()
{
	ROI MyROI;
	int X, Y, BPixelCountElem = 0, BPixelCount = 0;
	// Define ROI for the whole image
	MyROI.top = 0;
	MyROI.bottom = NUMBER_OF_ROWS;
	MyROI.left = 0;
	MyROI.right = NUMBER_OF_COLUMNS;
	// Initialize random number generator with current time
	std::mt19937 rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

	// Create uniform distribution for X and Y
	std::uniform_int_distribution<int> distX(0, NUMBER_OF_COLUMNS - 1);
	std::uniform_int_distribution<int> distY(0, NUMBER_OF_ROWS - 1);

	// Initialize both images to black
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			ProccesIMG[row][col] = 0;
			gaussian[row][col] = 0;
		}

	// Draw Gaussian distributions in random locations
	for (int i = 0; i < 6; i++) {
		X = distX(rng); // Generate a random X coordinate
		Y = distY(rng); // Generate a random Y coordinate
		DrawGaussian(ProccesIMG, X, Y, 10, 10);
	}

	// Save the processed image
	StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image241.bmp");
	InitThresholdLUT(LUT, 200, 255);
	// Apply LUT on the Gaussian image
	ImposeLUT(gaussian, LUT);
	BPixelCountElem = ContElemInPicture(gaussian, MyROI);
	

	// Draw a central Gaussian
	DrawGaussian(gaussian, int(NUMBER_OF_ROWS / 2), int(NUMBER_OF_COLUMNS / 2), 10, 10);

	

	// Count the number of black pixels in the Gaussian imag
	// Initialize LUT for thresholding
	InitThresholdLUT(LUT, 200, 255);
	// Apply LUT on ProccesIMG
	ImposeLUT(ProccesIMG, LUT);
	StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image_lut.bmp");
	// Count the number of black pixels in ProccesIMG
	BPixelCount = ContElemInPicture(ProccesIMG, MyROI);

	// Output the number of Gaussian elements found
	cout << "Found " << int(BPixelCount / BPixelCountElem + 0.5) << " Gaussian elements." << endl;

	// Load a color image and convert to grayscale
	LoadGrayImageFromTrueColorBmpFile(ProccesIMG, "Image242_color.bmp");
	StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image242_gray.bmp");
	// Initialize LUT for thresholding the new image
	InitThresholdLUT(LUT, 60, 65);

	// Apply LUT on the processed image
	ImposeLUT(ProccesIMG, LUT);

	// Measure the size of elements in a specific ROI
	MyROI.top = 127;
	MyROI.bottom = 192;
	MyROI.left = 9;
	MyROI.right = 76;
	BPixelCountElem = ContElemInPicture(ProccesIMG, MyROI);

	// Count black pixels in the entire image
	MyROI.top = 0;
	MyROI.bottom = NUMBER_OF_ROWS;
	MyROI.left = 0;
	MyROI.right = NUMBER_OF_COLUMNS-10;
	BPixelCount = ContElemInPicture(ProccesIMG, MyROI);

	// Save the final processed image
	StoreGrayImageAsGrayBmpFile(ProccesIMG, "Image242.bmp");

	// Output the number of elements found in the processed image
	cout << "Found " << int(BPixelCount / BPixelCountElem + 0.5) << " elements in the processed image." << endl;

	// Wait for user input before exiting
	WaitForUserPressKey();
}

