
#include <conio.h> // for _getch

// the following is needed for cin and cout instead of printf
#include <iostream>
#include <fstream>
using namespace std;  // explain someday

// The following is needed for math calculations
#include <math.h>
#define _USE_MATH_DEFINES 
#include <cmath> 

#include <time.h>  // for spead measurements

#include "ImProcInPlainC.h" // needed for those who wants to program in Plain C
#include "PrimeFFTn.h"      // for basic FFT operations

#define FILTER_SIZE_1 6
#define FILTER_SIZE_2 2

#define myMAXCOLORS 256

#define FILTER_HALF_WIDTH 2
#define FILTER_HALF_HEIGHT 2
#define FILTER_HALF_SIZE 12


unsigned char byteOriginal[VGA_HEIGHT][VGA_WIDTH];  //This is an array to store the original image data. SIZE_256 is a constant defined in the header

tFloat floatRe[VGA_HEIGHT][VGA_WIDTH];  //This stores the real part of an image in the frequency domain.
tFloat floatIm[VGA_HEIGHT][VGA_WIDTH];  // This stores the imaginary part of an image in the frequency domain.
tFloat floatFilter[VGA_HEIGHT][VGA_WIDTH];
unsigned char GrayImage1[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];


//These are 2 filter kernels used for convolution operations. The values inside these matrices represent the filter coefficients.
//these kernels are 5x5 HPF kernels - can be found in the web

double kernel_1[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] = {
	{-1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1},
	{-1, -1, 25, -1, -1},
	{-1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1}
};


double kernel_2[2 * FILTER_HALF_HEIGHT +1][2 * FILTER_HALF_HEIGHT + 1] = 
{
	{-1, -3, -4, -3, -1},
	{-3,  0,  6,  0, -3},
	{-4,  6, 40,  6, -4},
	{-3,  0,  6,  0, -3},
	{-1, -3, -4, -3, -1}
};


double kernel_3[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] = {
	{-1, -1, -1, -1, -1},
	{-1,  2,  2,  2, -1},
	{-1,  2,  8,  2, -1},
	{-1,  2,  2,  2, -1},
	{-1, -1, -1, -1, -1}
};


double kernel_4[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] =
{
	{-1, -1, -1, -1, -1},
	{-1,  1,  1,  1, -1},
	{-1,  1, 16,  1, -1},
	{-1,  1,  1,  1, -1},
	{-1, -1, -1, -1, -1}
};

double kernel_5[FILTER_HALF_HEIGHT+1][FILTER_HALF_HEIGHT+1] =
{
	{ 0,   -0.25,  0   },
	{-0.25, 2,   -0.25},
	{ 0,   -0.25,  0   }
};

unsigned char temp_dest[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];


//performs a convolution of the source image with a filter. It initializes the destination image to zero,
// then applies the filter to each pixel, and normalizes the result.
void DoFiltationByConvolution(unsigned char src[][NUMBER_OF_COLUMNS], unsigned char dest[][NUMBER_OF_COLUMNS],double filter[][2 * FILTER_HALF_WIDTH + 1])
{
	unsigned char* ptrToPixels = dest[0];
	for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; i++) { *ptrToPixels++ = 0; }
	for (int row = FILTER_HALF_HEIGHT;
		row < NUMBER_OF_ROWS - FILTER_HALF_HEIGHT;

		row++)

	{
		for (int column = FILTER_HALF_WIDTH;
			column < NUMBER_OF_COLUMNS - FILTER_HALF_WIDTH; column++)
		{
			double summa = 0;
			for (int y = -FILTER_HALF_HEIGHT; y <= FILTER_HALF_HEIGHT; y++)
			{
				for (int x = -FILTER_HALF_WIDTH; x <= FILTER_HALF_WIDTH; x++)
				{
					summa +=
						filter[FILTER_HALF_HEIGHT + y][FILTER_HALF_WIDTH + x] * src[row + y][column + x];

				} // of x
			} // of y
			summa /= 25.0;
			if (summa < 0) summa = 0;
			if (summa > 255) summa = 255;
			dest[row][column] = (unsigned char)summa;

		} // of column
	} // of row
}


//convert is a given function by samuel - page 59 in FFT part 1 lecture
void Convert(tFloat floatImage[][NUMBER_OF_COLUMNS], unsigned char byteOriginal[][NUMBER_OF_COLUMNS], tFloat minVal, tFloat maxVal)
{
	tFloat* ptrToFloatImage = floatImage[0];
	tFloat c, b, temp;
	if (minVal == maxVal)
	{
		c = 1; b = 0;
	}
	else
	{
		c = 255.0 / (maxVal - minVal);
		b = -c * minVal;
	}

	unsigned char* ptrToByteImage = byteOriginal[0];
	ptrToFloatImage = floatImage[0];

	for (int pixel = 0; pixel < NUMBER_OF_COLUMNS * NUMBER_OF_ROWS; pixel++)
	{
		temp = c * (*ptrToFloatImage++) + b;
		if (temp < 0)    temp = 0;
		if (temp > 255)  temp = 255;
		*ptrToByteImage++ = (unsigned char)((int)temp);
	}
}


//OptimalConvert is a given function by samuel - page 60 in FFT part 1 lecture
void OptimalConvert(tFloat floatImage[][NUMBER_OF_COLUMNS],	unsigned char byteOriginal[][NUMBER_OF_COLUMNS])
{
	tFloat* ptrToFloatImage = floatImage[0];
	tFloat MinVal, MaxVal, temp;
	// Find MinMax
	MinVal = *ptrToFloatImage;
	MaxVal = MinVal;

	for (int pixel = 0; pixel < NUMBER_OF_COLUMNS * NUMBER_OF_ROWS; pixel++)
	{
		temp = *ptrToFloatImage++;
		if (MinVal > temp) MinVal = temp;
		if (MaxVal < temp) MaxVal = temp;
	}
	Convert(floatImage, byteOriginal, MinVal, MaxVal);
}


//DoFFT is a given function by samuel - page 61 in FFT part 1 lecture
void DoFFT(tFloat imageRe[][NUMBER_OF_COLUMNS],	tFloat imageIm[][NUMBER_OF_COLUMNS], int exponentSign,int normalizationScalingType)
{

	tFloat* ptrToRe = imageRe[0];
	tFloat* ptrToIm = imageIm[0];

	tInteger arrayOfDimensions[2];
	arrayOfDimensions[0] = NUMBER_OF_COLUMNS;
	arrayOfDimensions[1] = NUMBER_OF_ROWS;

	PrimeFFTn(2,
		arrayOfDimensions,
		ptrToRe,
		ptrToIm,
		exponentSign,
		normalizationScalingType);
}


//ShiftHalfSize is a given function by samuel - page 62 in FFT part 1 lecture
void ShiftHalfSize(tFloat floatImage[][NUMBER_OF_COLUMNS])
{
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int column = 0; column < NUMBER_OF_COLUMNS; column += 2)
		{
			floatImage[row][column] *= -1;
		}
	}

	for (int row = 0; row < NUMBER_OF_ROWS; row += 2)
	{
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
		{
			floatImage[row][column] *= -1;
		}
	}
}


//CreateRectangle is a given function by samuel - page 64 in FFT part 1 lecture
void CreateRectangle(tFloat floatImage[][NUMBER_OF_COLUMNS],int halfHeight,int halfWidth,tFloat centralValue,tFloat peripheryValue)
{
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
		{
			floatImage[row][column] = peripheryValue;
		}
	}

	for (int row = NUMBER_OF_ROWS / 2 - halfHeight; row < NUMBER_OF_ROWS / 2 + halfHeight; row++)
	{
		for (int column = NUMBER_OF_COLUMNS / 2 - halfWidth; column < NUMBER_OF_COLUMNS / 2 + halfWidth; column++)
		{
			floatImage[row][column] = centralValue;
		}
	}


}


//DoFiltrationInFD is a given function by samuel - page 63 in FFT part 1 lecture
void DoFiltrationInFD(tFloat floatRe[][NUMBER_OF_COLUMNS],tFloat floatIm[][NUMBER_OF_COLUMNS],tFloat floatFilter[][NUMBER_OF_COLUMNS])
{
	tFloat* ptrToRe = floatRe[0];
	tFloat* ptrToIm = floatIm[0];
	tFloat* ptrToFilter = floatFilter[0];

	for (int pixel = 0; pixel < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; pixel++)
	{
		*ptrToRe++ *= *ptrToFilter;
		*ptrToIm++ *= *ptrToFilter++;
	}

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


void CreateGreyGaussian(unsigned char image[][NUMBER_OF_COLUMNS], double SigmaX, double SigmaY) {
	int midy = NUMBER_OF_ROWS / 2;
	int midx = NUMBER_OF_COLUMNS / 2;

	for (int row = 0; row < NUMBER_OF_ROWS; row++)
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
			double xa = column - midx;
			xa /= SigmaX;
			double ya = row - midy;
			ya /= SigmaY;

			double answer = 255.0 * (exp(-xa * xa - ya * ya));

			image[row][column] = (unsigned char)(unsigned)(answer);
		}
}


//version 1 of work() function - include a simple rectangle HPF/LPF already in FD:
//void Work(unsigned char ProccesIMG[][NUMBER_OF_COLUMNS],int filter_size)
//{
//	cout << "FFT Filtration in nearly Plain C" << endl;
//
//	//input image processing - FFT in preperation to the filteration process in DoFiltrationInFD
//	for (int i = 0; i < NUMBER_OF_ROWS; i++) // copies the image data from the ProccesIMG array to the floatRe array
//	{
//		for (int j = 0;j < NUMBER_OF_COLUMNS;j++)
//			floatRe[i][j] = ProccesIMG[i][j];
//	}
//	CreateRectangle(floatRe, NUMBER_OF_ROWS / 4, NUMBER_OF_COLUMNS / 4, 200, 50);
//	CreateRectangle(floatIm, NUMBER_OF_ROWS / 4, NUMBER_OF_COLUMNS / 4, 0, 0); // kind of clean
//	Convert(floatRe, byteOriginal, 0, 255);
//
//
//	ShiftHalfSize(floatRe);  // This function shifts the image data by multiplying every other row and column by -1, 
//	ShiftHalfSize(floatIm);	 // a common preprocessing step before performing FFT.
//
//	DoFFT(floatRe, floatIm, FORWARD_FFT, NORMALIZE_BY_SQRT);  //performs the Fast Fourier Transform on the image, converting it from the
//															  //spatial domain to the frequency domain
//	Convert(floatRe, byteOriginal, 0, 50);  //After the FFT, the real part of the frequency domain image (floatRe) is converted back
//										   //to a byte image for display purposes. The values are scaled between 0 and 50 in this case.
//
//
//	//AT THE END OF THE FIRST PART - THE INPUT IMAGE IS READY FOR FILTERATION PROCESS IN FD (already after FFT)!
//	
//	//THE NEXT PART OF THE Work FUNCTION IS DEDECATED TO THE FILTER CREATION AND THE FILTERATION IN THE FD AND THEN IFFT THE RESULT BACK TO TD!
//
//	//filter creation
//	CreateRectangle( floatFilter, filter_size, filter_size, 0, 1 );  // ideal HPF - using creatRectangle function
//	OptimalConvert(floatFilter, byteOriginal);
//	StoreGrayImageAsGrayBmpFile(byteOriginal, "filter.bmp");
//
//	//filteration 
//	DoFiltrationInFD(floatRe, floatIm, floatFilter);
//	Convert(floatRe, byteOriginal, 0, 50);
//
//	//IFFT
//	DoFFT(floatRe, floatIm, REVERSE_FFT, NORMALIZE_BY_SQRT);
//	ShiftHalfSize(floatRe);
//	ShiftHalfSize(floatIm);
//
//	OptimalConvert(floatRe, byteOriginal);
//
//	for (int i = 0; i < NUMBER_OF_ROWS; i++)
//	{
//		for (int j = 0;j < NUMBER_OF_COLUMNS;j++)
//			ProccesIMG[i][j] = byteOriginal[i][j];
//	}
//}


//version 2 of work() function - include a gauusian filter creation and FFT it to FD before filteration:
//void Work(unsigned char ProccesIMG[][NUMBER_OF_COLUMNS], int filter_size)
//{
//	cout << "FFT Filtration in nearly Plain C" << endl;
//
//	// Step 1: Input image processing - FFT in preparation for the filtration process in DoFiltrationInFD
//	for (int i = 0; i < NUMBER_OF_ROWS; i++) // Copy image data from the ProccesIMG array to the floatRe array
//	{
//		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
//			floatRe[i][j] = ProccesIMG[i][j];
//	}
//	CreateRectangle(floatRe, NUMBER_OF_ROWS / 4, NUMBER_OF_COLUMNS / 4, 200, 50);
//	CreateRectangle(floatIm, NUMBER_OF_ROWS / 4, NUMBER_OF_COLUMNS / 4, 0, 0); // Kind of clean
//	Convert(floatRe, byteOriginal, 0, 255);
//
//	ShiftHalfSize(floatRe);  // preprocessing step before performing an FFT to center the low frequencies in the FFT output.
//	ShiftHalfSize(floatIm);
//
//	DoFFT(floatRe, floatIm, FORWARD_FFT, NORMALIZE_BY_SQRT);  // performs the Fast Fourier Transform on the image, converting it
//															 //  from the spatial domain to the frequency domain. This transformation separates
//															//   the image into its frequency components, stored in floatRe (real part) and 
//														   //    floatIm (imaginary part).
//														   
//	//****** THIS PART IS THE CHANGE FROM THE GIVEN CODE BE SAMUEL - FOR GAUSSIAN FILTER (RATHER THAN RECTANGLE LPF) - ******* HAS TO BE CHECKED!
//	// Step 2: Prepare the Gaussian filter
//	double gaussianFilter[filter_size];
//	PrepareGaussianFilter(gaussianFilter, filter_size, 1.0);  //A 1D Gaussian filter is created using the PrepareGaussianFilter function
//
//	//The 1D Gaussian filter is extended to a 2D filter by multiplying the 1D Gaussian values in both the x and y dimensions,
//   // storing the result in floatFilter.
//	for (int i = 0; i < NUMBER_OF_ROWS; i++)
//	{
//		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
//		{
//			floatFilter[i][j] = gaussianFilter[i % filter_size] * gaussianFilter[j % filter_size];
//		}
//	}
//
//	// Perform FFT on the Gaussian filter to move it to the frequency domain
//	tFloat floatImFilter[SIZE_256][SIZE_256] = { 0 };
//	ShiftHalfSize(floatFilter); // Shift before FFT
//	DoFFT(floatFilter, floatImFilter, FORWARD_FFT, NORMALIZE_BY_SQRT);
//	ShiftHalfSize(floatFilter); // Shift back after FFT
//
//	//******* END OF THE NEW PART - GAUSSIAN FILTER CREATION AND FFT ************
//
//	// Step 3: Filtration in the frequency domain
//	DoFiltrationInFD(floatRe, floatIm, floatFilter);
//
//	// Convert the filtered image back to a byte image
//	Convert(floatRe, byteOriginal, 0, 50);
//
//	// Step 4: Perform inverse FFT to return to the spatial domain
//	DoFFT(floatRe, floatIm, REVERSE_FFT, NORMALIZE_BY_SQRT);
//	ShiftHalfSize(floatRe);
//	ShiftHalfSize(floatIm);
//
//	OptimalConvert(floatRe, byteOriginal);
//
//	// Step 5: Copy the result back to ProccesIMG
//	for (int i = 0; i < NUMBER_OF_ROWS; i++)
//	{
//		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
//			ProccesIMG[i][j] = byteOriginal[i][j];
//	}
//}

//version 3 of work() function - include a gauusian filter creation and FFT it to FD before filteration + (addition) dynamic alocation for the
//size of the gaussian filter buffer according to the different sizes we want to try.
#include <vector>

void Work(unsigned char ProccesIMG[][NUMBER_OF_COLUMNS], int filter_size)
{
	cout << "FFT Filtration in nearly Plain C" << endl;

	// Step 1: Input image processing - FFT in preparation for the filtration process in DoFiltrationInFD
	for (int i = 0; i < NUMBER_OF_ROWS; i++) // Copy image data from the ProccesIMG array to the floatRe array
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
			floatRe[i][j] = ProccesIMG[i][j];
	}
	CreateRectangle(floatRe, NUMBER_OF_ROWS / 4, NUMBER_OF_COLUMNS / 4, 200, 50);
	CreateRectangle(floatIm, NUMBER_OF_ROWS / 4, NUMBER_OF_COLUMNS / 4, 0, 0); // Kind of clean
	Convert(floatRe, byteOriginal, 0, 255);

	ShiftHalfSize(floatRe);  // preprocessing step before performing an FFT to center the low frequencies in the FFT output.
	ShiftHalfSize(floatIm);

	DoFFT(floatRe, floatIm, FORWARD_FFT, NORMALIZE_BY_SQRT);  // performs the Fast Fourier Transform on the image, converting it
															 // from the spatial domain to the frequency domain. This transformation separates
															// the image into its frequency components, stored in floatRe (real part) and 
														   // floatIm (imaginary part).

	//****** THIS PART IS THE CHANGE FROM THE GIVEN CODE BY SAMUEL - FOR GAUSSIAN FILTER (RATHER THAN RECTANGLE LPF) - ******* HAS TO BE CHECKED!
	// Step 2: Prepare the Gaussian filter
	

	CreateGreyGaussian(GrayImage1, 20, 30);
	StoreGrayImageAsGrayBmpFile(GrayImage1, "GreyGaussianfilter.bmp");


	//// Perform FFT on the Gaussian filter to move it to the frequency domain --------> assume that the gaussian filter is already in FD 
	//tFloat floatImFilter[SIZE_256][SIZE_256] = { 0 };
	//ShiftHalfSize(floatFilter); // Shift before FFT
	//DoFFT(floatFilter, floatImFilter, FORWARD_FFT, NORMALIZE_BY_SQRT);
	//ShiftHalfSize(floatFilter); // Shift back after FFT

	//******* END OF THE NEW PART - GAUSSIAN FILTER CREATION AND FFT **************************

	// Step 3: Filtration in the frequency domain
	DoFiltrationInFD(floatRe, floatIm, floatFilter);

	// Convert the filtered image back to a byte image
	Convert(floatRe, byteOriginal, 0, 50);

	// Step 4: Perform inverse FFT to return to the spatial domain
	DoFFT(floatRe, floatIm, REVERSE_FFT, NORMALIZE_BY_SQRT);
	ShiftHalfSize(floatRe);
	ShiftHalfSize(floatIm);

	OptimalConvert(floatRe, byteOriginal);

	// Step 5: Copy the result back to ProccesIMG
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
			ProccesIMG[i][j] = byteOriginal[i][j];
	}
}




// Declare Gray Image
unsigned char ProccesIMG[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char dst[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];

void main()
{
	/// FFT 
	// Load and process the Tim1.bmp image
	LoadGrayImageFromTrueColorBmpFile(ProccesIMG, "Tim1.bmp");
	Work(ProccesIMG, FILTER_SIZE_1);
	StoreGrayImageAsGrayBmpFile(ProccesIMG, "Tim1LPFF.bmp");

	// Apply kernel_1
	DoFiltationByConvolution(ProccesIMG, dst, kernel_1);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1_HighPass_kernel_1.bmp");

	// Apply kernel_2
	DoFiltationByConvolution(ProccesIMG, dst, kernel_2);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1_HighPass_kernel_2.bmp");

	// Apply kernel_3
	DoFiltationByConvolution(ProccesIMG, dst, kernel_3);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1_HighPass_kernel_3.bmp");

	// Apply kernel_4
	DoFiltationByConvolution(ProccesIMG, dst, kernel_4);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1_HighPass_kernel_4.bmp");

	//// Apply kernel_5
	//DoFiltationByConvolution(ProccesIMG, dst, kernel_5);
	//StoreGrayImageAsGrayBmpFile(dst, "Tim1_HighPass_kernel_5.bmp");

	//***** repeat the steps before on Tim2.bmp given image **********

	// Load and process the Tim2.bmp image
	LoadGrayImageFromGrayBmpFile(ProccesIMG, "Tim2.bmp");
	Work(ProccesIMG, FILTER_SIZE_1);
	StoreGrayImageAsGrayBmpFile(ProccesIMG, "Tim2LPFF.bmp");

	// Apply kernel_1
	DoFiltationByConvolution(ProccesIMG, dst, kernel_1);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2_HighPass_kernel_1.bmp");

	// Apply kernel_2
	DoFiltationByConvolution(ProccesIMG, dst, kernel_2);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2_HighPass_kernel_2.bmp");

	// Apply kernel_3
	DoFiltationByConvolution(ProccesIMG, dst, kernel_3);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2_HighPass_kernel_3.bmp");

	// Apply kernel_4
	DoFiltationByConvolution(ProccesIMG, dst, kernel_4);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2_HighPass_kernel_4.bmp");

	//// Apply kernel_5
	//DoFiltationByConvolution(ProccesIMG, dst, kernel_5);
	//StoreGrayImageAsGrayBmpFile(dst, "Tim2_HighPass_kernel_5.bmp");

	cout << "Press any key to exit" << endl;
	_getch();
}