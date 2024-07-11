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




void  lightness(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	int Red, Blue, Green;
	for (int row = 0; row < CGA_HEIGHT; row++)
	{
		for (int col = 0; col < CGA_WIDTH; col++)
		{
			Red = image[row][col][R];
			Green = image[row][col][G];
			Blue = image[row][col][B];
			image[row][col][R] = int((max(max(Red, Blue), Green) + (min(min(Red, Blue), Green))) / 2.0);
			image[row][col][G] = int((max(max(Red, Blue), Green) + (min(min(Red, Blue), Green))) / 2.0);
			image[row][col][B] = int((max(max(Red, Blue), Green) + (min(min(Red, Blue), Green))) / 2.0);
		}																		  
	}
}
void  average(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	int Red, Blue, Green;
	for (int row = 0; row < CGA_HEIGHT; row++)
	{
		for (int col = 0; col < CGA_WIDTH; col++)
		{
			Red = image[row][col][R];
			Green = image[row][col][G];
			Blue = image[row][col][B];
			image[row][col][R] = int((Red + Blue + Green) / 3.0);
			image[row][col][G] = int((Red + Blue + Green) / 3.0);
			image[row][col][B] = int((Red + Blue + Green) / 3.0);
		}
	}
}
void luminosity(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	int Red, Blue, Green;
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			Red = image[row][col][R];
			Green = image[row][col][G];
			Blue = image[row][col][B];
			image[row][col][R] = int((int(0.21 * Red) + int(0.72 * Green) + int(0.07 * Blue)));
			image[row][col][G] = int((int(0.21 * Red) + int(0.72 * Green) + int(0.07 * Blue)));
			image[row][col][B] = int((int(0.21 * Red) + int(0.72 * Green) + int(0.07 * Blue)));
		}
	}
}

void singleColorChannel(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			int green = image[row][col][G];
			image[row][col][R] = green;
			image[row][col][G] = green;
			image[row][col][B] = green;
		}
	}
}

void main()
{
	unsigned char Picture[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS];

	//First picture
	LoadBgrImageFromTrueColorBmpFile(Picture, "1.bmp");
	average(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "1_averge.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "1.bmp");
	lightness(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "1_lightness.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "1.bmp");
	luminosity(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "1_luminosity.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "1.bmp");
	singleColorChannel(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "1_singleColorChannel.bmp");

	//Second picture
	LoadBgrImageFromTrueColorBmpFile(Picture, "2.bmp");
	average(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "2_averge.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "2.bmp");
	lightness(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "2_lightness.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "2.bmp");
	luminosity(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "2_luminosity.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "2.bmp");
	singleColorChannel(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "2_singleColorChannel.bmp");

	//Third picture
	LoadBgrImageFromTrueColorBmpFile(Picture,"3.bmp");
	average(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "3_averge.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "3.bmp");
	lightness(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "3_lightness.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "3.bmp");
	luminosity(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "3_luminosity.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "3.bmp");
	singleColorChannel(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "3_singleColorChannel.bmp");

	//fourth picture

	LoadBgrImageFromTrueColorBmpFile(Picture, "4.bmp");
	average(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "4_averge.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "4.bmp");
	lightness(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "4_lightness.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "4.bmp");
	luminosity(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "4_luminosity.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "4.bmp");
	singleColorChannel(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "4_singleColorChannel.bmp");

	//fifth picture

	LoadBgrImageFromTrueColorBmpFile(Picture, "5.bmp");
	average(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "5_averge.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "5.bmp");
	lightness(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "5_lightness.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "5.bmp");
	luminosity(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "5_luminosity.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "5.bmp");
	singleColorChannel(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "5_singleColorChannel.bmp");

	////sixth picture

	//LoadBgrImageFromTrueColorBmpFile(Picture, "6.bmp");
	//average(Picture);
	//StoreBgrImageAsGrayBmpFile(Picture, "6_averge.bmp");

	//LoadBgrImageFromTrueColorBmpFile(Picture, "6.bmp");
	//lightness(Picture);
	//StoreBgrImageAsGrayBmpFile(Picture, "6_lightness.bmp");

	//LoadBgrImageFromTrueColorBmpFile(Picture, "6.bmp");
	//luminosity(Picture);
	//StoreBgrImageAsGrayBmpFile(Picture, "6_luminosity.bmp");

	//LoadBgrImageFromTrueColorBmpFile(Picture, "6.bmp");
	//singleColorChannel(Picture);
	//StoreBgrImageAsGrayBmpFile(Picture, "6_singleColorChannel.bmp");
	
	//seventh picture

	LoadBgrImageFromTrueColorBmpFile(Picture, "7.bmp");
	average(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "7_averge.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "7.bmp");
	lightness(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "7_lightness.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "7.bmp");
	luminosity(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "7_luminosity.bmp");

	LoadBgrImageFromTrueColorBmpFile(Picture, "7.bmp");
	singleColorChannel(Picture);
	StoreBgrImageAsGrayBmpFile(Picture, "7_singleColorChannel.bmp");

	WaitForUserPressKey();
}