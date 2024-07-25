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
#define MAX_COLORS (256)
void apply_LUT(unsigned char img[][NUMBER_OF_COLUMNS], unsigned char LUT[]);
void CalcHistogram(unsigned char img[][NUMBER_OF_COLUMNS], int hist[]);
void CalcIntegralOfHistogram(int hist[], int IntOfHist[]);
// Declare Gray Image 
unsigned char GrayImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

void apply_LUT(unsigned char img[][NUMBER_OF_COLUMNS], unsigned char LUT[])
{
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			img[row][col] = LUT[img[row][col]];
		}
	}
}

void CalcHistogram(unsigned char img[][NUMBER_OF_COLUMNS], int hist[])
{
	for (int i = 0; i < MAX_COLORS; i++)
		hist[i] = 0;
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			hist[img[row][col]] += 1;
		}
	}
}
void CalcIntegralOfHistogram(int hist[], int IntOfHist[])
{
	IntOfHist[0] = hist[0];
	for (int i = 1; i < MAX_COLORS; i++)
		IntOfHist[i] = IntOfHist[i - 1] + hist[i];
}
void CalculateLutByHistEqu(int *IntOfHist,unsigned char LUT[])
{
	double factor = (double)((MAX_COLORS-1)) / ((double)NUMBER_OF_ROWS * (double)NUMBER_OF_COLUMNS);
	int temp;
	for (int i = 0; i < MAX_COLORS; i++)
	{
		temp = (int)(factor * (double)IntOfHist[i])+1;
		if (temp < 0)
			temp = 0;
		if (temp > 255)
			temp = 255;
		LUT[i] = (unsigned char)temp;
	}
}

void main()
{
	ofstream f3("hist2.csv");
	ofstream f ("intofhist2.csv");
	ofstream f2("LUT2.csv");
	LoadGrayImageFromGrayBmpFile(GrayImage, "A211.bmp"); //Change here for each image
	int hist[MAX_COLORS];
	int intOfHist[MAX_COLORS];
	unsigned char LUT[MAX_COLORS];

	CalcHistogram(GrayImage, hist);
	CalcIntegralOfHistogram(hist, intOfHist);
	CalculateLutByHistEqu(intOfHist, LUT);
	for (int i = 0; i < MAX_COLORS; i++)
	{
		f << intOfHist[i]<<'\n';
	}
	f.close();
	for (int i = 0; i < MAX_COLORS; i++)
	{
		f2 << (double)LUT[i] << '\n';
	}
	f2.close();
	apply_LUT(GrayImage, LUT);
	CalcHistogram(GrayImage, hist);
	for (int i = 0; i < MAX_COLORS; i++)
	{
		f3 << hist[i] << '\n';
	}
	f3.close();
	StoreGrayImageAsGrayBmpFile(GrayImage, "A230.bmp");
	
	LoadGrayImageFromGrayBmpFile(GrayImage, "A211.bmp");
	CalcHistogram(GrayImage, hist);
	CalcIntegralOfHistogram(hist, intOfHist);
	CalculateLutByHistEqu(intOfHist, LUT);
	apply_LUT(GrayImage, LUT);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A231.bmp");

	LoadGrayImageFromGrayBmpFile(GrayImage, "A212.bmp");
	CalcHistogram(GrayImage, hist);
	CalcIntegralOfHistogram(hist, intOfHist);
	CalculateLutByHistEqu(intOfHist, LUT);
	apply_LUT(GrayImage, LUT);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A232.bmp");

	LoadGrayImageFromGrayBmpFile(GrayImage, "A221.bmp");
	CalcHistogram(GrayImage, hist);
	CalcIntegralOfHistogram(hist, intOfHist);
	CalculateLutByHistEqu(intOfHist, LUT);
	apply_LUT(GrayImage, LUT);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A233.bmp");

	LoadGrayImageFromGrayBmpFile(GrayImage, "A222.bmp");
	CalcHistogram(GrayImage, hist);
	CalcIntegralOfHistogram(hist, intOfHist);
	CalculateLutByHistEqu(intOfHist, LUT);
	apply_LUT(GrayImage, LUT);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A234.bmp");
	
	WaitForUserPressKey();
}


