#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>

using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>

// BMP Library
#include "ImProcInPlainC.h"
#define PIXEL_VALUES (255)
#define PIXEL_MINVAL (0)
#define PIXEL_MAXVAL (255)

//Declare Gray Image
unsigned char GrayImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char LUT[PIXEL_VALUES];

void Create_LUT(unsigned char LUT[], double c, double b)
{
	for (unsigned i = 0; i <= PIXEL_MAXVAL; i++)
	{
		double val = c * i + b;
		if (val > PIXEL_MAXVAL)
			LUT[i] = PIXEL_MAXVAL;
		else if (val < PIXEL_MINVAL)
			LUT[i] = PIXEL_MINVAL;
		else
			LUT[i] = val;
	}
}

void ApplyLUTonGrayImage(unsigned char img[][NUMBER_OF_COLUMNS], unsigned char LUT[])
{
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			unsigned char* pixel = &img[row][col];
			*pixel = LUT[*pixel];
		}
	}
}


void Spoil_good_looking_Image(unsigned char img[][NUMBER_OF_COLUMNS])
{
	// from short ID => ABCD=1950 (BARAK'S ID) and EFGH=2210 (OMRI'S ID)

	double c = 0.442;  //  c=0.22+0.023*B+0.003*C => ABCD=1950 ==> B=9,C=5 ==> c=0.442
	double b = 30; //23+3*F+G => EFGH=2210 ==> F=2,G=1 ==> b=30

	Create_LUT(LUT, c, b);
	ApplyLUTonGrayImage(GrayImage, LUT);
}

void CreateMinMaxLUT(unsigned char LUT[], unsigned char min, unsigned max)
{
	double c = 255./ (max - min);
	cout << "c" << c << endl;
	double b = -c * min; 
	Create_LUT(LUT, c, b);
}

void Min_Max_Algorithm(unsigned char img[][NUMBER_OF_COLUMNS])
{
	unsigned char min, max;
	min = PIXEL_MAXVAL;
	max = PIXEL_MINVAL;
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			unsigned char* pixel = &img[row][col];
			if (*pixel < min)
				min = *pixel;
			if (*pixel > max)
				max = *pixel;
		}
	}
	CreateMinMaxLUT(LUT, min, max);
	cout << "max " << (int)max << endl;
	cout << "min " << int(min) << endl;
	cout << "pixel_maxval " << int(PIXEL_MAXVAL) << endl;
	cout << "pixel minval " << int(PIXEL_MINVAL) << endl;
	ApplyLUTonGrayImage(GrayImage, LUT);
}




void main()
{
	LoadGrayImageFromTrueColorBmpFile(GrayImage, "A20.bmp");
	StoreGrayImageAsGrayBmpFile(GrayImage, "A20gray.bmp");
	Spoil_good_looking_Image(GrayImage);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A211.bmp");
	Min_Max_Algorithm(GrayImage);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A212.bmp");

	LoadGrayImageFromTrueColorBmpFile(GrayImage, "A201.bmp");
	StoreGrayImageAsGrayBmpFile(GrayImage, "A201gray.bmp");
	Spoil_good_looking_Image(GrayImage);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A2111.bmp");
	Min_Max_Algorithm(GrayImage);
	StoreGrayImageAsGrayBmpFile(GrayImage, "A2121.bmp");

	WaitForUserPressKey();
}