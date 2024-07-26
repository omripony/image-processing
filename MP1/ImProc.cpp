#include <stdio.h> // for printf
#include <conio.h> // for getch

#include <fstream>  // For file IO
using namespace std; // Explain some day 

// This needed to do Math calculations
#define _USE_MATH_DEFINES
#include <math.h>

// BMP Library
#include "ImProcInPlainC.h"


void lightness(unsigned char image[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	unsigned char* pixel = &image[0][0][0];
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			int Red = *(pixel + R);
			int Green = *(pixel + G);
			int Blue = *(pixel + B);
			int lightness = (max(max(Red, Blue), Green) + min(min(Red, Blue), Green)) / 2;
			*(pixel + R) = *(pixel + G) = *(pixel + B) = lightness;
			pixel += NUMBER_OF_COLORS;
		}
	}
}


void average(unsigned char image[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	unsigned char* pixel = &image[0][0][0];
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			int Red = *(pixel + R);
			int Green = *(pixel + G);
			int Blue = *(pixel + B);
			int avg = (Red + Green + Blue) / 3;
			*(pixel + R) = *(pixel + G) = *(pixel + B) = avg;
			pixel += NUMBER_OF_COLORS;
		}
	}
}


void luminosity(unsigned char image[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	unsigned char* pixel = &image[0][0][0];
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			int Red = *(pixel + R);
			int Green = *(pixel + G);
			int Blue = *(pixel + B);
			int lum = int(0.21 * Red + 0.72 * Green + 0.07 * Blue);
			*(pixel + R) = *(pixel + G) = *(pixel + B) = lum;
			pixel += NUMBER_OF_COLORS;
		}
	}
}


void singleColorChannel(unsigned char image[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	unsigned char* pixel = &image[0][0][0];
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			int green = *(pixel + G);
			*(pixel + R) = *(pixel + G) = *(pixel + B) = green;
			pixel += NUMBER_OF_COLORS;
		}
	}
}


void main()
{
	static unsigned char Picture[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS];

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

}