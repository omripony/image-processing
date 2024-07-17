/*
  First steps in Image Processing
  Creating simple Synthetic Images
  Using BMP Library
*/


#include <stdio.h> // for printf
#include <conio.h> // for getch

#include <iostream> // for cin cout
#include <fstream>  // For file IO
using namespace std; // Explain some day 

// This needed to do Math calculations
#define _USE_MATH_DEFINES
#include <math.h>

// BMP Library
#include "ImProcInPlainC.h"
#define PIXEL_VALUES (255)
#define PIXEL_MINVAL (0)
#define PIXEL_MAXVAL (255)

// Declare Gray Image
unsigned char GrayImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char LUT[PIXEL_VALUES];

void Create_LUT(unsigned char LUT[], double c, double b) {
	for (unsigned i = 0; i <= PIXEL_MAXVAL; i++) {
		double val = c * i + b;
		if (val > PIXEL_MAXVAL)
			LUT[i] = PIXEL_MAXVAL;
		else if (val < PIXEL_MINVAL)
			LUT[i] = PIXEL_MINVAL;
		else
			LUT[i] = (unsigned char)val;
	}
}

void ApplyLUTOnGrayImage(unsigned char* img, unsigned char LUT[], int rows, int cols) {
	for (int i = 0; i < rows * cols; i++) {
		*(img + i) = LUT[*(img + i)];
	}
}

void fill_hist_and_calc_int(unsigned char* img, unsigned hist[PIXEL_VALUES], unsigned IntOfHist[PIXEL_VALUES], int rows, int cols) {
	for (int i = 0; i < rows * cols; i++) {
		hist[*(img + i)] += 1;
	}
	IntOfHist[0] = hist[0];
	for (unsigned i = 1; i < PIXEL_VALUES; i++) {
		IntOfHist[i] = IntOfHist[i - 1] + hist[i];
	}
}

void FindMinMaxFromIntOfHist(unsigned IntOfHist[PIXEL_VALUES], unsigned char& min, unsigned char& max, double button, double upper) {
	double bottomcount = button * IntOfHist[PIXEL_VALUES - 1];
	double topcount = upper * IntOfHist[PIXEL_VALUES - 1];
	min = 0;
	max = 0;
	for (unsigned i = 0; i < PIXEL_VALUES; i++) {
		if (IntOfHist[i] < bottomcount)
			min = i;
		if (IntOfHist[i] < topcount)
			max = i;
	}
}

//Preparing Look-Up-Table
void create_precentage_lut(unsigned char* img, unsigned char LUT[], double button, double upper, int rows, int cols) {
	unsigned hist[PIXEL_VALUES] = { 0 };
	unsigned IntOfHist[PIXEL_VALUES] = { 0 };
	fill_hist_and_calc_int(img, hist, IntOfHist, rows, cols);

	unsigned char min = 0, max = 0;
	FindMinMaxFromIntOfHist(IntOfHist, min, max, button, upper);
	double c = 255. / (max - min);
	double b = -c * min;
	Create_LUT(LUT, c, b);
}

//In this function we insert 25%-75% & 5%-95% values:
void precentage_algorithm(unsigned char* img, double button, double upper, int rows, int cols) {
	create_precentage_lut(img, LUT, button, upper, rows, cols);
	ApplyLUTOnGrayImage(img, LUT, rows, cols);
}

//Final stage, excute
void main() {
	LoadGrayImageFromGrayBmpFile(GrayImage, "A211.bmp");
	precentage_algorithm(&GrayImage[0][0], 0.25, 0.75, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A221.bmp");

	LoadGrayImageFromGrayBmpFile(GrayImage, "A211.bmp");
	precentage_algorithm(&GrayImage[0][0], 0.05, 0.95, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A222.bmp");

	LoadGrayImageFromGrayBmpFile(GrayImage, "A2111.bmp");
	precentage_algorithm(&GrayImage[0][0], 0.25, 0.75, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A2211.bmp");

	LoadGrayImageFromGrayBmpFile(GrayImage, "A2111.bmp");
	precentage_algorithm(&GrayImage[0][0], 0.05, 0.95, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A2221.bmp");

	WaitForUserPressKey();
}
