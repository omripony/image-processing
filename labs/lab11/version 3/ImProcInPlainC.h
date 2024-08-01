#pragma once
using namespace std;
#include <conio.h> // for getch
#include <stdio.h> // for old C-Style file operations 
#include <stdlib.h> // for exit
#include <iostream>
#include <math.h> 
#define _USE_MATH_DEFINES
// The following declarations are needed for Plain C programming
#define CGA_WIDTH  (320)  // Please do not change
#define CGA_HEIGHT (240)  // Please do not change

#define VGA_WIDTH  (640)  // Please do not change
#define VGA_HEIGHT (480)  // Please do not change

#define SIZE_256 256  // for simple FFT tests
#define SIZE_512 512  // for CT and MRI files

#define NUMBER_OF_ROWS    (CGA_HEIGHT)
#define NUMBER_OF_COLUMNS (CGA_WIDTH)

#define NUMBER_OF_COLORS (3)
#define B (0)
#define G (1)
#define R (2)
#define M_PI 3.14
/*
  Image size is restricted to properly aligned images like 320x240, 512x512
  WARNING: Big image sizes may create Real-Time problems in STACKS
		   640x480 may be big size
  In that case declare images "globally" - bad practice, but...

  Example of declaration of the Gray Image:
	   unsigned char Image[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
  Example of declaration of the BGR Image:
	   unsigned char Image[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS];
*/

// I do not want to include real Windows headers:
//             because of Express versions - PSDK then must be installed - explain
//             + do not mess with NET staff

// The following may be changed in the future versions of the Windows
typedef unsigned short int tWord;
typedef unsigned int       tDWord;
typedef int                tLong;


#define myBMP_TAG 0x4d42        // "BM"  

#define myMAXCOLORS  (256)        // Needed for GrayPalette

// BMP file starts with BITMAPINFOHEADER
// see MSHelp for last version of BITMAPINFOHEADER
struct myBITMAPFILEHEADER
{
	tWord bfType; // Specifies the file type, must be BM.
	tDWord bfSize; // Specifies the size, in bytes, of the bitmap file. 
	tWord bfReserved1; //Reserved; must be zero. 
	tWord bfReserved2; //Reserved; must be zero. 
	tDWord bfOffBits;   //  Specifies the offset, in bytes, 
	//  from the beginning of the BITMAPFILEHEADER structure to the bitmap bits.
};

// BITMAPINFO structure immediately follows the BITMAPFILEHEADER structure in the BMP file 
struct myBITMAPINFOHEADER
{
	tDWord biSize;   // Specifies the number of bytes required by the structure.
	tLong biWidth;  // Specifies the width of the bitmap, in pixels. 
	tLong biHeight; // Specifies the height of the bitmap, in pixels. 
	//      If biHeight is positive, the bitmap is a bottom-up DIB
	//      and its origin is the lower-left corner. 
	//
	//      If biHeight is negative, the bitmap is a top-down DIB
	//      and its origin is the upper-left corner.
	tWord biPlanes; // Specifies the number of planes for the target device. 
	//      This value must be set to 1. 
	tWord biBitCount;  // Specifies the number of bits-per-pixel
	// Must be one of the following: 
	//  0 - for JPEG or PNG format - does not rely on extention only
	//  1 - Monochrome, for example Black and White image
	//  4 - 16 colors - remember ???
	//  8 - 256 colors - still used with Palette
	//         detailed explanation will follow later
	//  24 - this is what we need now 24=8*3
	//  32 - 32 = 8*4 = BGR and ALPHA - transparency
	//       nearly not used today inthe Windows XP
	//       but increase speed (32 bit = size of integer )
	//       will be intencively used in GUI of Vista
	tDWord biCompression;   // Must be one of the following (constanmts defined deep in SDK
	//  BI_RGB = 0 - An uncompressed format. - what we need now
	//  BI_RLE8, BI_RLE4  A run-length encoded (RLE) format
	//           explanation later
	//  BI_BITFIELDS - no compression. Used with 16 and 32 bpp images
	//  BI_JPEG - Indicates that the image is a JPEG image.
	//  BI_PNG - Indicates that the image is a PNG image.
	tDWord biSizeImage;    //  Specifies the size, in bytes, of the image. 
	//  This may be set to zero for BI_RGB bitmaps - our case here.
	tLong biXPelsPerMeter; // Specifies the horizontal resolution, in pixels-per-meter, 
	//   of the target device for the bitmap. 
	//   Mostly not used and thus not set
	tLong biYPelsPerMeter; //Specifies the vertical resolution, n pixels-per-meter, 
	//   of the target device for the bitmap. 
	//   Mostly not used and thus not set 
	tDWord biClrUsed;       // 0 in our case
	tDWord biClrImportant;  // 0 in our case
};

// service function for primitive user interface
void WaitForUserPressKey();

void StoreBgrImageAsTrueColorBmpFile(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
	char bmpFileName[]);
void StoreBgrImageAsGrayBmpFile(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
	char bmpFileName[]);
void StoreGrayImageAsTrueColorBmpFile(unsigned char image[][NUMBER_OF_COLUMNS],
	char bmpFileName[]);
void StoreGrayImageAsGrayBmpFile(unsigned char image[][NUMBER_OF_COLUMNS],
	char bmpFileName[]);

void LoadBgrImageFromTrueColorBmpFile(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
	char bmpFileName[]);
void LoadBgrImageFromGrayBmpFile(unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
	char bmpFileName[]);
void LoadGrayImageFromTrueColorBmpFile(unsigned char image[][NUMBER_OF_COLUMNS],
	char bmpFileName[]);
void LoadGrayImageFromGrayBmpFile(unsigned char image[][NUMBER_OF_COLUMNS],
	char bmpFileName[]);

void ConvertGrayImageToColorImage(unsigned char source[][NUMBER_OF_COLUMNS],
	unsigned char destination[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS]);

void ConvertColorImageToGrayImage(unsigned char source[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
	unsigned char destination[][NUMBER_OF_COLUMNS]);

struct s2dPoint
{
	int X, Y;

	// Constructor with default values and validation
	s2dPoint(int x = 0, int y = 0) : X(x), Y(y)
	{
		validate();
	}

	// Validation function
	void validate()
	{

		if (X < 0 || X > NUMBER_OF_COLUMNS)
		{
		

			if (X < 0)
				X = 0;
			if (X > NUMBER_OF_COLUMNS)
				X = NUMBER_OF_COLUMNS;
			
		}
		if (Y < 0 || Y > NUMBER_OF_ROWS)
		{

			
			if (Y < 0)
				Y = 0;
			if (Y > NUMBER_OF_ROWS)
				Y = NUMBER_OF_ROWS;
			
		}

	}
};


void AddGrayRectangle(unsigned char image[][NUMBER_OF_COLUMNS], s2dPoint A,
	s2dPoint B1, unsigned char transparency, unsigned char grayLevel);