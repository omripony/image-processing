
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

#define FILTER_SIZE_1 20
#define FILTER_SIZE_2 2
#define FILTER_SIZE_3 1

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

double kernel_1[2 * FILTER_HALF_HEIGHT +1][2 * FILTER_HALF_HEIGHT + 1] = 
{
	{-1, -3, -4, -3, -1},
	{-3,  0,  6,  0, -3},
	{-4,  6, 40,  6, -4},
	{-3,  0,  6,  0, -3},
	{-1, -3, -4, -3, -1}
};


double kernel_2[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] = {
	{-1, -1, -1, -1, -1},
	{-1,  2,  2,  2, -1},
	{-1,  2,  8,  2, -1},
	{-1,  2,  2,  2, -1},
	{-1, -1, -1, -1, -1}
};


double kernel_3[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] =
{
	{-1, -1, -1, -1, -1},
	{-1,  1,  1,  1, -1},
	{-1,  1, 16,  1, -1},
	{-1,  1,  1,  1, -1},
	{-1, -1, -1, -1, -1}
};


double kernel_4[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] = {
	{ 0, -1, -1, -1,  0},
	{-1,  2,  2,  2, -1},
	{-1,  2,  8,  2, -1},
	{-1,  2,  2,  2, -1},
	{ 0, -1, -1, -1,  0}
};


unsigned char temp_dest[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];


//DoFiltationByConvolution is a given function by samuel - page 5 in filtration by convolution lecture
void DoFiltationByConvolution(unsigned char src[][NUMBER_OF_COLUMNS], unsigned char dest[][NUMBER_OF_COLUMNS],
															double filter[][2 * FILTER_HALF_WIDTH + 1])
{
	unsigned char* ptrToPixels = dest[0];
	for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; i++) { *ptrToPixels++ = 0; }
	for (int row = FILTER_HALF_HEIGHT; row < NUMBER_OF_ROWS - FILTER_HALF_HEIGHT; row++)
	{
		for (int column = FILTER_HALF_WIDTH; column < NUMBER_OF_COLUMNS - FILTER_HALF_WIDTH; column++)
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


void CreateHFEF(tFloat HFEF[][NUMBER_OF_COLUMNS], double alpha)
{
	int centerX = NUMBER_OF_COLUMNS / 2;
	int centerY = NUMBER_OF_ROWS / 2;

	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			double distance = sqrt(pow(row - centerY, 2) + pow(col - centerX, 2));
			HFEF[row][col] = 1 + alpha * distance;
		}
	}
}


void DoGaussianFiltration(unsigned char* src, double* filter, int filter_size) //filteration using convolution - x direction and then y direction
{
	int half_size = (filter_size - 1) / 2;
	double summa;

	memset(temp_dest, 0, sizeof(temp_dest));  

// x direction
	for (int row = 0; row < NUMBER_OF_ROWS; row++) {
		for (int column = half_size; column < NUMBER_OF_COLUMNS - half_size; column++) {
			summa = 0;
			for (int x = -half_size; x <= half_size; x++) {
				summa += filter[half_size + x] * (double)*(src + row * NUMBER_OF_COLUMNS + (column + x));
			}
			if (summa < 0) summa = 0;
			if (summa > 255) summa = 255;
			*(temp_dest[0] + row * NUMBER_OF_COLUMNS + column) = (unsigned char)summa;
		}
	}

	// Clear Source !!!
	for (int i = 0; i < NUMBER_OF_ROWS * NUMBER_OF_COLUMNS; i++) {
		src[i] = 0;
	}

	// y direction
	for (int row = half_size; row < NUMBER_OF_ROWS - half_size; row++) {
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
			summa = 0;
			for (int y = -half_size; y <= half_size; y++) {
				summa += filter[half_size + y] * *(temp_dest[0] + (row + y) * NUMBER_OF_COLUMNS + column);}			
			if (summa < 0) summa = 0;
			if (summa > 255) summa = 255;
			*(src + row * NUMBER_OF_COLUMNS + column) = (unsigned char)summa;
		}
	}
}


void CreateHighPassFilter(tFloat floatFilter[][NUMBER_OF_COLUMNS], int cutoff, int order)
{
	int centerX = NUMBER_OF_COLUMNS / 2;
	int centerY = NUMBER_OF_ROWS / 2;

	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			double distance = sqrt(pow(row - centerY, 2) + pow(col - centerX, 2));
			floatFilter[row][col] = 1.0 / (1.0 + pow((double)cutoff / (distance + 1e-10), 2 * order));
		}
	}

	// Invert the filter values to correctly visualize the HPF
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int col = 0; col < NUMBER_OF_COLUMNS; col++)
		{
			floatFilter[row][col] = 1.0 - floatFilter[row][col];
		}
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
//#include <vector>
//
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
//															 // from the spatial domain to the frequency domain. This transformation separates
//															// the image into its frequency components, stored in floatRe (real part) and 
//														   // floatIm (imaginary part).
//
//
//	Convert(floatRe, byteOriginal, 0, 50); // ***for debugging***
//	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_FFT_Before_Filtering.bmp"); // ***for debugging***
//
//	//****** THIS PART IS THE CHANGE FROM THE GIVEN CODE BY SAMUEL - FOR GAUSSIAN FILTER (RATHER THAN RECTANGLE LPF) - ******* HAS TO BE CHECKED!
//	// Step 2: Prepare the Gaussian filter
//	
//
//	CreateGreyGaussian(GrayImage1, 20, 30);
//	StoreGrayImageAsGrayBmpFile(GrayImage1, "GreyGaussianfilter.bmp");
//
//
//	//// Perform FFT on the Gaussian filter to move it to the frequency domain --------> assume that the gaussian filter is already in FD 
//	//tFloat floatImFilter[SIZE_256][SIZE_256] = { 0 };
//	//ShiftHalfSize(floatFilter); // Shift before FFT
//	//DoFFT(floatFilter, floatImFilter, FORWARD_FFT, NORMALIZE_BY_SQRT);
//	//ShiftHalfSize(floatFilter); // Shift back after FFT
//
//	//******* END OF THE NEW PART - GAUSSIAN FILTER CREATION AND FFT **************************
//
//	// Step 3: Filtration in the frequency domain
//	DoFiltrationInFD(floatRe, floatIm, floatFilter);
//
//	// Convert the filtered image back to a byte image
//	Convert(floatRe, byteOriginal, 0, 50); // ***for debugging***
//	StoreGrayImageAsGrayBmpFile(byteOriginal, "filtered_image_in_FD_before_inverse_FFT.bmp");  // ***for debugging***
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


//version 4 of work() function - i understood that the problem in the first part of the work() function - the FFT of the given image.
//so a change to see if the FFT of the given image is as neccecery 

void Work(unsigned char ProccesIMG[][NUMBER_OF_COLUMNS], int filter_size)
{
	cout << "FFT Filtration in nearly Plain C" << endl;

	// Step 1: Input image processing - FFT in preparation for the filtration process in DoFiltrationInFD
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
		{
			floatRe[i][j] = ProccesIMG[i][j];
			floatIm[i][j] = 0;  // Ensure imaginary part is initialized to 0
		}
	}

	Convert(floatRe, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_Initial.bmp");

	// Step 2: Apply FFT preprocessing (centering)
	ShiftHalfSize(floatRe);
	ShiftHalfSize(floatIm);

	Convert(floatRe, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_After_Shift.bmp");

	// Step 3: Perform FFT to convert the image to the frequency domain
	DoFFT(floatRe, floatIm, FORWARD_FFT, NORMALIZE_BY_SQRT);

	Convert(floatRe, byteOriginal, 0, 50);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_FFT_Before_Filtering.bmp");

	// Step 4: Prepare the Gaussian filter
	CreateGreyGaussian(GrayImage1, 10, 10);
	StoreGrayImageAsGrayBmpFile(GrayImage1, "GreyGaussianFilter.bmp");

	// Convert the GrayImage1 to floatFilter (as it's likely in 8-bit unsigned char format) -- very important!!
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
		{
			floatFilter[i][j] = (tFloat)GrayImage1[i][j] / 255.0;  // Normalize to 0-1 range
		}
	}

	// Apply the Gaussian filter in the frequency domain
	DoFiltrationInFD(floatRe, floatIm, floatFilter);

	// Debug: Save the filtered frequency domain image before inverse FFT
	Convert(floatRe, byteOriginal, 0, 50);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Filtered_Image_in_FD_before_IFFT.bmp");

	// Step 5: Perform inverse FFT to return to the spatial domain
	DoFFT(floatRe, floatIm, REVERSE_FFT, NORMALIZE_BY_SQRT);
	ShiftHalfSize(floatRe);
	ShiftHalfSize(floatIm);

	// Debug: Save the final result after inverse FFT and shifting
	OptimalConvert(floatRe, byteOriginal);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_Final_Result.bmp");

	// Step 6: Copy the result back to ProccesIMG
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
		{
			ProccesIMG[i][j] = byteOriginal[i][j];
		}
	}
}


void WorkHPEF1(unsigned char ProccesIMG[][NUMBER_OF_COLUMNS], int filter_size)
{
	cout << "FFT Filtration in nearly Plain C" << endl;

	// Step 1: Input image processing - FFT in preparation for the filtration process
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
		{
			floatRe[i][j] = ProccesIMG[i][j];
			floatIm[i][j] = 0;  // Ensure imaginary part is initialized to 0
		}
	}

	Convert(floatRe, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim2_Initial.bmp");

	// Step 2: Apply FFT preprocessing (centering)
	ShiftHalfSize(floatRe);
	ShiftHalfSize(floatIm);

	Convert(floatRe, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim2_After_Shift.bmp");

	// Step 3: Perform FFT to convert the image to the frequency domain
	DoFFT(floatRe, floatIm, FORWARD_FFT, NORMALIZE_BY_SQRT);

	Convert(floatRe, byteOriginal, 0, 50);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim2_FFT_Before_Filtering.bmp");

	// Step 4: Create the High Frequency Enhancing Filter (HFEF)
	int cutoff = 80; // Use the best cutoff frequency from the last row
	int order = 8;   // Use the best order from the last row
	CreateHighPassFilter(floatFilter, cutoff, order);
	OptimalConvert(floatFilter, byteOriginal);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Filter.bmp");

	// Step 5: Apply the HFEF in the frequency domain
	DoFiltrationInFD(floatRe, floatIm, floatFilter);

	// Debug: Save the filtered frequency domain image before inverse FFT
	Convert(floatRe, byteOriginal, 0, 50);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Filtered_Image_in_FD_before_IFFT.bmp");

	// Step 6: Perform inverse FFT to return to the spatial domain
	DoFFT(floatRe, floatIm, REVERSE_FFT, NORMALIZE_BY_SQRT);
	ShiftHalfSize(floatRe);
	ShiftHalfSize(floatIm);

	// Debug: Save the final result after inverse FFT and shifting
	OptimalConvert(floatRe, byteOriginal);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim2_Final_Result_With_HFEF.bmp");

	// Step 7: Copy the result back to ProccesIMG
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
		{
			ProccesIMG[i][j] = byteOriginal[i][j];
		}
	}
}


void WorkHPEF2(unsigned char ProccesIMG[][NUMBER_OF_COLUMNS], int filter_size)
{
	cout << "FFT Filtration in nearly Plain C" << endl;

	// Step 1: Input image processing - FFT in preparation for the filtration process
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
		{
			floatRe[i][j] = ProccesIMG[i][j];
			floatIm[i][j] = 0;  // Ensure imaginary part is initialized to 0
		}
	}

	Convert(floatRe, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_Initial.bmp");

	// Step 2: Apply FFT preprocessing (centering)
	ShiftHalfSize(floatRe);
	ShiftHalfSize(floatIm);

	Convert(floatRe, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_After_Shift.bmp");

	// Step 3: Perform FFT to convert the image to the frequency domain
	DoFFT(floatRe, floatIm, FORWARD_FFT, NORMALIZE_BY_SQRT);

	Convert(floatRe, byteOriginal, 0, 50);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_FFT_Before_Filtering.bmp");

	// Step 4: Create the High Frequency Enhancing Filter (HFEF)
	CreateHFEF(floatFilter, 1);  // Adjust the alpha value to control the strength of enhancement

	// Debug: Save the HFEF for inspection
	Convert(floatFilter, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "HFEF.bmp");

	// Step 5: Apply the HFEF in the frequency domain
	DoFiltrationInFD(floatRe, floatIm, floatFilter);

	// Debug: Save the filtered frequency domain image before inverse FFT
	Convert(floatRe, byteOriginal, 0, 50);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Filtered_Image_in_FD_before_IFFT.bmp");

	// Step 6: Perform inverse FFT to return to the spatial domain
	DoFFT(floatRe, floatIm, REVERSE_FFT, NORMALIZE_BY_SQRT);
	ShiftHalfSize(floatRe);
	ShiftHalfSize(floatIm);

	// Debug: Save the final result after inverse FFT and shifting
	OptimalConvert(floatRe, byteOriginal);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim1_Final_Result_With_HFEF.bmp");

	// Step 7: Copy the result back to ProccesIMG
	for (int i = 0; i < NUMBER_OF_ROWS; i++)
	{
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++)
		{
			ProccesIMG[i][j] = byteOriginal[i][j];
		}
	}
}


// Declare Gray Image
unsigned char ProccesIMG1[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char ProccesIMG2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char dst[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char src1[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char src2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char src3[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char src4[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
void main()
{

	//part 1 of the task - "By using reworked 2D 5x5 and 7x7 filters BLUR Image Tim1.bmp by using convolution with Fast Gaussian 1D Filters"
	cout << "Gaussian Filter" << endl;

	//***** option 1 for part 1 - using the PrepareGaussianFilter function ******
	// Apply a 5x5 Gaussian filter
	const int FILTER_HALF_SIZE_5 = 2;
	double offset = 0.24;
	const int FILTER_SIZE_5 = 2 * FILTER_HALF_SIZE_5 + 1;
	double filter_5[FILTER_SIZE_5] = { 7/273+ offset, 26/273 + offset, 41/273 + offset, 26/273 + offset, 7/273 + offset };  // Use the provided filter

	LoadGrayImageFromTrueColorBmpFile(src1, "Tim1.bmp");  // Load the image
	DoGaussianFiltration(&src1[0][0], filter_5, FILTER_SIZE_5);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src1, "Tim1LPF5c.bmp");  // Save the blurred image with 5x5 filter

	LoadGrayImageFromGrayBmpFile(src2, "Tim2.bmp");  // Load the image
	DoGaussianFiltration(&src2[0][0], filter_5, FILTER_SIZE_5);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src2, "Tim2LPF5c.bmp");  // Save the blurred image with 5x5 filter

	// Apply a 7x7 Gaussian filter
	const int FILTER_HALF_SIZE_7 = 3;
	const int FILTER_SIZE_7 = 2 * FILTER_HALF_SIZE_7 + 1;
	double offset2 = 0.16;
	double filter_7[FILTER_SIZE_7] = { 2/1003 + offset2, 22/1003 + offset2, 97/1003 + offset2, 159/1003 + offset2, 97/1003 + offset2, 22/1003 + offset2, 2/1003 + offset2 };  // Use the provided filter

	LoadGrayImageFromTrueColorBmpFile(src3, "Tim1.bmp");  // Reload the original image
	DoGaussianFiltration(&src3[0][0], filter_7, FILTER_SIZE_7);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src3, "Tim1LPF7c.bmp");  // Save the blurred image with 7x7 filter

	LoadGrayImageFromGrayBmpFile(src4, "Tim2.bmp");  // Load the image
	DoGaussianFiltration(&src4[0][0], filter_7, FILTER_SIZE_7);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src4, "Tim2LPF7c.bmp");  // Save the blurred image with 7x7 filter


	
	//********* part 2 of the task - "Blur test Image by using FFT and “restore it” *************

	// Load and process the Tim1.bmp image
	LoadGrayImageFromTrueColorBmpFile(ProccesIMG1, "Tim1.bmp");
	StoreGrayImageAsGrayBmpFile(ProccesIMG1, "Tim1_gray.bmp");  //**for debugging

	/// FFT filtration to blur using gaussian filter - section 3 
	Work(ProccesIMG1, FILTER_SIZE_1);
	StoreGrayImageAsGrayBmpFile(ProccesIMG1, "Tim1LPFF.bmp");

	// convolution filtration using HPF with different kernels - here Applying kernel_1 on src1- section 4 
	DoFiltationByConvolution(src1, dst, kernel_1);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF5cHFEF1c.bmp"); //Tim1 blurred by gaussian in size 5 restored by kernel_1

	// convolution filtration using HPF with different kernels - here Applying kernel_1 on src3 - section 4 
	DoFiltationByConvolution(src3, dst, kernel_1);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF7cHFEF1c.bmp"); //Tim1 blurred by gaussian in size 7 restored by kernel_1
	//------------------------------------------------------------//
	// convolution filtration using HPF with different kernels - here Applying kernel_2 on src1 - section 4 
	DoFiltationByConvolution(src1, dst, kernel_2);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF5cHFEF2c.bmp"); //Tim1 blurred by gaussian in size 5 restored by kernel_2

	// convolution filtration using HPF with different kernels - here Applying kernel_2 on src3 - section 4 
	DoFiltationByConvolution(src3, dst, kernel_2);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF7cHFEF2c.bmp");  //Tim1 blurred by gaussian in size 7 restored by kernel_2
	//------------------------------------------------------------//
	// convolution filtration using HPF with different kernels - here Applying kernel_3 on src1 - section 4 
	DoFiltationByConvolution(src1, dst, kernel_3);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF5cHFEF3c.bmp"); //Tim1 blurred by gaussian in size 5 restored by kernel_3

	// convolution filtration using HPF with different kernels - here Applying kernel_3 on src3 - section 4 
	DoFiltationByConvolution(src3, dst, kernel_3);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF7cHFEF3c.bmp"); //Tim1 blurred by gaussian in size 7 restored by kernel_3
	//------------------------------------------------------------//
	//// convolution filtration using HPF with different kernels - here Applying kernel_4 on src1 - section 4 
	DoFiltationByConvolution(src1, dst, kernel_4);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF5cHFEF4c.bmp");  //Tim1 blurred by gaussian in size 5 restored by kernel_4

	//// convolution filtration using HPF with different kernels - here Applying kernel_4 on src3 - section 4 
	DoFiltationByConvolution(src3, dst, kernel_4);
	StoreGrayImageAsGrayBmpFile(dst, "Tim1LPF7cHFEF4c.bmp"); //Tim1 blurred by gaussian in size 7 restored by kernel_4
	//------------------------------------------------------------//
	/// FFT filtration using HPF to restore (enhance) the blurred image - section 5 
	WorkHPEF2(ProccesIMG1, FILTER_SIZE_3);
	StoreGrayImageAsGrayBmpFile(ProccesIMG1, "Tim1HPEF.bmp");
	

	//**************************************************************
	//***** repeat the steps before on Tim2.bmp given image **********

	// Load and process the Tim2.bmp image
	LoadGrayImageFromGrayBmpFile(ProccesIMG2, "Tim2.bmp");

	/// FFT filtration using gaussian filter - section 3 
	Work(ProccesIMG2, FILTER_SIZE_1);
	StoreGrayImageAsGrayBmpFile(ProccesIMG2, "Tim2LPFF.bmp");

	// convolution filtration using HPF with different kernels - here Applying kernel_1 on src2- section 4 
	DoFiltationByConvolution(src2, dst, kernel_1);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF5cHFEF1c.bmp"); //Tim2 blurred by gaussian in size 5 restored by kernel_1

	// convolution filtration using HPF with different kernels - here Applying kernel_1 on src4 - section 4 
	DoFiltationByConvolution(src4, dst, kernel_1);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF7cHFEF1c.bmp"); //Tim2 blurred by gaussian in size 7 restored by kernel_1
	//------------------------------------------------------------//
	// convolution filtration using HPF with different kernels - here Applying kernel_2 on src2 - section 4 
	DoFiltationByConvolution(src2, dst, kernel_2);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF5cHFEF2c.bmp"); //Tim2 blurred by gaussian in size 5 restored by kernel_2

	// convolution filtration using HPF with different kernels - here Applying kernel_2 on src4 - section 4 
	DoFiltationByConvolution(src4, dst, kernel_2);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF7cHFEF2c.bmp"); //Tim2 blurred by gaussian in size 7 restored by kernel_2
	//------------------------------------------------------------//
	// convolution filtration using HPF with different kernels - here Applying kernel_3 on src2 - section 4 
	DoFiltationByConvolution(src2, dst, kernel_3);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF5cHFEF3c.bmp"); //Tim2 blurred by gaussian in size 5 restored by kernel_3

	// convolution filtration using HPF with different kernels - here Applying kernel_3 on src4 - section 4 
	DoFiltationByConvolution(src4, dst, kernel_3);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF7cHFEF3c.bmp"); //Tim2 blurred by gaussian in size 7 restored by kernel_3
	//------------------------------------------------------------//
	//// convolution filtration using HPF with different kernels - here Applying kernel_4 on src2 - section 4 
	DoFiltationByConvolution(src2, dst, kernel_4);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF5cHFEF4c.bmp"); //Tim2 blurred by gaussian in size 5 restored by kernel_4

	//// convolution filtration using HPF with different kernels - here Applying kernel_4 on src4 - section 4 
	DoFiltationByConvolution(src4, dst, kernel_4);
	StoreGrayImageAsGrayBmpFile(dst, "Tim2LPF7cHFEF4c.bmp"); //Tim2 blurred by gaussian in size 7 restored by kernel_4
	//------------------------------------------------------------//
	/// FFT filtration using HPF to restore (enhance) the blurred image - section 5 
	WorkHPEF1(ProccesIMG2, FILTER_SIZE_3);
	StoreGrayImageAsGrayBmpFile(ProccesIMG2, "Tim2HPEF.bmp");

	cout << "Press any key to exit" << endl;
	_getch();
}