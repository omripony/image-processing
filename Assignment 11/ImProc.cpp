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


/*Adds an Rectangle with coordinates,gray color and transparency*/
void AddRectangle(unsigned char img[][NUMBER_OF_COLUMNS],
	int top, int left, int bottom, int right,
	unsigned char grayLevel, unsigned char transparency)
{
	// the min and max function take care of extream cases.	
	for (int row = max(bottom,0); row < min(top, NUMBER_OF_ROWS); row++) 
		{
			for (int column = max(left,0); column < min(right, NUMBER_OF_COLUMNS); column++)
			{
				img[row][column] = transparency * (img[row][column] / 255.0); // The transparency function
				img[row][column] += (255 - transparency) * grayLevel / 255.0;
				
			}
		}
}



// Declare Gray Image
unsigned char img[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

void main()
{
	// initiate gray img backround to Black
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
		{			
			img[row][column] = 0;
		}
	}
	// add Rect in the second quadrant
	AddRectangle(img, NUMBER_OF_ROWS + 40, -30, NUMBER_OF_ROWS / 2, NUMBER_OF_COLUMNS / 2, 190, 50);
	// add Rect in the forth quadrant
	AddRectangle(img, NUMBER_OF_ROWS/2-20, NUMBER_OF_COLUMNS / 2, 0, NUMBER_OF_COLUMNS, 240, 30);
	// add Rect in the first quadrant in the middle
	AddRectangle(img, NUMBER_OF_ROWS-20, NUMBER_OF_COLUMNS / 2+20, NUMBER_OF_ROWS/2 + 20, NUMBER_OF_COLUMNS-20, 100, 90);
	//save gray img with the rectangles 
	StoreGrayImageAsGrayBmpFile(img, "grayImg11.bmp");
	WaitForUserPressKey();
}