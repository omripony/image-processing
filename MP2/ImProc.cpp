
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

double kernel_3x3_option1_Laplacian[FILTER_HALF_HEIGHT +1][FILTER_HALF_HEIGHT + 1] =
{
	{  0, -1,  0},
	{ -1,  4, -1},
	{  0, -1,  0}
};

double kernel_3x3_option2_High_Boost_Filter[FILTER_HALF_HEIGHT + 1][FILTER_HALF_HEIGHT + 1] =
{
   {-0.25, -0.25, -0.25},
   {-0.25, 2.5, -0.25},
   {-0.25, -0.25, -0.25}
};

double kernel_3x3_option3_Unsharp_Masking_Filter[FILTER_HALF_HEIGHT + 1][FILTER_HALF_HEIGHT + 1] =
{
   {-1, -1, -1},
   {-1, 9, -1},
   {-1, -1, -1}
};


double kernel_5x5_option1_Laplacian_of_Gaussian[2*FILTER_HALF_HEIGHT + 1][2*FILTER_HALF_HEIGHT + 1] =
{
   {0, 0, -1, 0, 0},
   {0, -1, -2, -1, 0},
   {-1, -2, 16, -2, -1},
   {0, -1, -2, -1, 0},
   {0, 0, -1, 0, 0}
};

double kernel_5x5_option2_High_Boost_Filter[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] =
{
   {-1, -1, -1, -1, -1},
   {-1, -1, -1, -1, -1},
   {-1, -1, 24, -1, -1},
   {-1, -1, -1, -1, -1},
   {-1, -1, -1, -1, -1}
};

double kernel_5x5_option3_Unsharp_Masking_Filter[2 * FILTER_HALF_HEIGHT + 1][2 * FILTER_HALF_HEIGHT + 1] =
{
   {1, 4, 6, 4, 1},
   {4, 16, 24, 16, 4},
   {6, 24, -476, 24, 6},
   {4, 16, 24, 16, 4},
   {1, 4, 6, 4, 1}
};

double kernel_7x7_option1_Laplacian_of_Gaussian[2 * FILTER_HALF_HEIGHT + 3][2 * FILTER_HALF_HEIGHT + 3] =
{
   {0, 0, -1, -1, -1, 0, 0},
   {0, -1, -3, -3, -3, -1, 0},
   {-1, -3, 0, 7, 0, -3, -1},
   {-1, -3, 7, 24, 7, -3, -1},
   {-1, -3, 0, 7, 0, -3, -1},
   {0, -1, -3, -3, -3, -1, 0},
   {0, 0, -1, -1, -1, 0, 0}
};

double kernel_7x7_option2_High_Boost_Filter[2 * FILTER_HALF_HEIGHT + 3][2 * FILTER_HALF_HEIGHT + 3] =
{
   {-1, -1, -1, -1, -1, -1, -1},
   {-1, -1, -1, -1, -1, -1, -1},
   {-1, -1, -1, -1, -1, -1, -1},
   {-1, -1, -1, 49, -1, -1, -1},
   {-1, -1, -1, -1, -1, -1, -1},
   {-1, -1, -1, -1, -1, -1, -1},
   {-1, -1, -1, -1, -1, -1, -1}
};

double kernel_7x7_option3_Unsharp_Masking_Filter[2 * FILTER_HALF_HEIGHT + 3][2 * FILTER_HALF_HEIGHT + 3] =
{
	{1, 1, 2, 2, 2, 1, 1},
	{1, 2, 4, 4, 4, 2, 1},
	{2, 4, 8, 8, 8, 4, 2},
	{2, 4, 8, -124, 8, 4, 2},
	{2, 4, 8, 8, 8, 4, 2},
	{1, 2, 4, 4, 4, 2, 1},
	{1, 1, 2, 2, 2, 1, 1}
};

unsigned char temp_dest[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];


//DoFiltationByConvolution is a given function by samuel - page 5 in filtration by convolution lecture
void DoFiltationByConvolution(unsigned char src[][NUMBER_OF_COLUMNS], unsigned char dest[][NUMBER_OF_COLUMNS],
							  double* filter, int filter_size)
{
	// Calculate half-width and half-height dynamically based on the filter size
	int filter_half_size = filter_size / 2;

	// Perform convolution
	for (int row = filter_half_size; row < NUMBER_OF_ROWS - filter_half_size; row++)
	{
		for (int column = filter_half_size; column < NUMBER_OF_COLUMNS - filter_half_size; column++)
		{
			double summa = 0;
			for (int y = -filter_half_size; y <= filter_half_size; y++)
			{
				for (int x = -filter_half_size; x <= filter_half_size; x++)
				{
					// Perform convolution with the unsharp masking kernel
					summa += filter[(y + filter_half_size) * filter_size + (x + filter_half_size)] * src[row + y][column + x];
				}
			}

			// Clip values to the 0-255 range
			summa = std::min(255.0, std::max(0.0, summa));

			// Write the result into the destination image
			dest[row][column] = static_cast<unsigned char>(summa);
		}
	}
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
	tFloat* floatImage_ptr= floatImage[0];
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
void CreateRectangle(tFloat* floatImage, int halfHeight, int halfWidth, tFloat centralValue, tFloat peripheryValue)
{
	// Fill the entire image with the peripheryValue
	for (int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
		{
			*(floatImage + row * NUMBER_OF_COLUMNS + column) = peripheryValue;
		}
	}

	// Set the central rectangle to centralValue
	for (int row = NUMBER_OF_ROWS / 2 - halfHeight; row < NUMBER_OF_ROWS / 2 + halfHeight; row++)
	{
		for (int column = NUMBER_OF_COLUMNS / 2 - halfWidth; column < NUMBER_OF_COLUMNS / 2 + halfWidth; column++)
		{
			*(floatImage + row * NUMBER_OF_COLUMNS + column) = centralValue;
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


void PrepareGaussianFilter(double* filter, int filter_size, double sigma)
{
	int half_size = (filter_size - 1) / 2;
	double summa = 0;

	for (int i = 0; i < filter_size; i++)
	{
		double temp = (double)(i - half_size) / sigma;
		temp = exp(-temp * temp);
		*(filter + i) = temp;  // Use pointer arithmetic instead of array indexing
		summa += temp;
	}

	for (int i = 0; i < filter_size; i++)
	{
		*(filter + i) /= summa;  // Normalize the filter using pointer arithmetic
	}
}


void CreateGreyGaussian(unsigned char(*image)[NUMBER_OF_COLUMNS], double SigmaX, double SigmaY) {
	int midy = NUMBER_OF_ROWS / 2;
	int midx = NUMBER_OF_COLUMNS / 2;

	for (int row = 0; row < NUMBER_OF_ROWS; row++) {
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
			double xa = column - midx;
			xa /= SigmaX;
			double ya = row - midy;
			ya /= SigmaY;

			double answer = 255.0 * (exp(-xa * xa - ya * ya));

			image[row][column] = (unsigned char)(unsigned)(answer);
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


void CreateHighPassFilter(tFloat(*floatFilter)[NUMBER_OF_COLUMNS], int cutoff, int order)
{
	int centerX = NUMBER_OF_COLUMNS / 2;
	int centerY = NUMBER_OF_ROWS / 2;

	// Loop to compute the high-pass filter
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


//work() is the a general function that includes all the steps that necessary for conducting FFT and filtering in FD
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
	// Modify this line to make filter_size impact the Gaussian filter
	CreateGreyGaussian(GrayImage1, filter_size / 10.0, filter_size / 10.0); // Make filter_size affect sigma
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


void PrintFilterValues(tFloat filter[][NUMBER_OF_COLUMNS], int filter_size, const char* filter_type) {
	printf("Filter Type: %s, Size: %d\n", filter_type, filter_size);
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++) {
			printf("%.2f ", filter[i][j]);
		}
		printf("\n");
	}
}


void CreateHighFrequencyEnhancingFilter(tFloat filter[][NUMBER_OF_COLUMNS], const char* filter_type, unsigned char gaussianImage[][NUMBER_OF_COLUMNS], int filter_size, float boost_factor) {
	int centerX = NUMBER_OF_COLUMNS / 2;
	int centerY = NUMBER_OF_ROWS / 2;

	// Step 1: Initialize filter to zero
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++) {
			filter[i][j] = 0.0f;
		}
	}

	printf("Creating High-Frequency Enhancing Filter of type: %s, size: %d, boost factor: %.2f\n", filter_type, filter_size, boost_factor);

	if (strcmp(filter_type, "gaussian") == 0) {
		// Step 2: Convert the Gaussian LPF from unsigned char (0-255) to normalized float (0-1)
		for (int i = 0; i < NUMBER_OF_ROWS; i++) {
			for (int j = 0; j < NUMBER_OF_COLUMNS; j++) {
				filter[i][j] = 1.0 - (gaussianImage[i][j] / 255.0);  // Convert to HPF by subtracting from 1
			}
		}
	}
	else if (strcmp(filter_type, "high_boost") == 0) {
		// Step 3: Create High-Boost filter kernel
		tFloat center_value = filter_size * filter_size + (boost_factor - 1) * (filter_size * filter_size - 1);
		for (int i = -filter_size / 2; i <= filter_size / 2; i++) {
			for (int j = -filter_size / 2; j <= filter_size / 2; j++) {
				int y = centerY + i;
				int x = centerX + j;
				if (y >= 0 && y < NUMBER_OF_ROWS && x >= 0 && x < NUMBER_OF_COLUMNS) {
					filter[y][x] = -1;
				}
			}
		}
		filter[centerY][centerX] = center_value;
	}
	else if (strcmp(filter_type, "unsharp") == 0) {
		// Step 4: Create Unsharp Masking filter kernel
		if (filter_size == 3) {
			filter[centerY][centerX] = 9;
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (!(i == 0 && j == 0)) {
						filter[centerY + i][centerX + j] = -1;
					}
				}
			}
		}
		else if (filter_size == 5) {
			filter[centerY][centerX] = -476;
			filter[centerY - 1][centerX] = filter[centerY + 1][centerX] = filter[centerY][centerX - 1] = filter[centerY][centerX + 1] = 24;
			filter[centerY - 2][centerX] = filter[centerY + 2][centerX] = filter[centerY][centerX - 2] = filter[centerY][centerX + 2] = 6;
		}
	}
	else {
		printf("Unknown filter type: %s\n", filter_type);
		return;  // Early return if filter type is not recognized
	}

	// Step 5: Check if filter has non-zero values for debugging
	bool hasNonZero = false;
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++) {
			if (filter[i][j] != 0.0) {
				hasNonZero = true;
				break;
			}
		}
		if (hasNonZero) break;
	}

	if (!hasNonZero) {
		printf("All filter values are zero after creation. Check filter logic.\n");
	}
	else {
		printf("Filter created successfully with non-zero values.\n");
	}

	// Step 6: Normalize the filter for better visualization
	tFloat maxVal = 0.0;
	for (int i = 0; i < NUMBER_OF_ROWS; i++) {
		for (int j = 0; j < NUMBER_OF_COLUMNS; j++) {
			if (filter[i][j] > maxVal) maxVal = filter[i][j];
		}
	}
	if (maxVal > 0) {
		for (int i = 0; i < NUMBER_OF_ROWS; i++) {
			for (int j = 0; j < NUMBER_OF_COLUMNS; j++) {
				filter[i][j] /= maxVal;
			}
		}
	}
}




//WorkHPEF() is the a general function that 
void WorkHPEF(unsigned char ProccesIMG[][NUMBER_OF_COLUMNS], int filter_size)
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
	Convert(floatIm, byteOriginal, 0, 50);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "Tim2_FFT_Before_Filtering_im.bmp");

	// Step 4: Create the High Frequency Enhancing Filter (HFEF)
	//tFloat floatFilter_hpf[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
	const char* filter_type = "high_boost"; // Example filter type, can be changed to "laplacian" or "unsharp"
	float boost_factor = 1.8f; // Only relevant for "high_boost" filter
	void CreateHighFrequencyEnhancingFilter(
		tFloat filter[][NUMBER_OF_COLUMNS],            // Output: The high-frequency enhancing filter
		const char* filter_type,                       // Input: Type of filter ("gaussian", "high_boost", "unsharp", etc.)
		unsigned char gaussianImage[][NUMBER_OF_COLUMNS], // Input: The Gaussian low-pass filter in the form of a grayscale image
		int filter_size,                               // Input: The size of the filter to be created
		float boost_factor                             // Input: The boost factor for high-boost filters
	);
	PrintFilterValues(floatFilter, filter_size, filter_type);
	Convert(floatFilter, byteOriginal, 0, 255);
	StoreGrayImageAsGrayBmpFile(byteOriginal, "HPFE.bmp");
	

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


// Declare Gray Image
unsigned char ProccesIMG11[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];  //ProccesIMG1 are the results of the blurring FFT of Tim1.bmp
unsigned char ProccesIMG12[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];  
unsigned char ProccesIMG13[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];  

unsigned char ProccesIMG21[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];  //ProccesIMG2 are the results of the blurring FFT of Tim2.bmp
unsigned char ProccesIMG22[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];  
unsigned char ProccesIMG23[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];  

unsigned char src1[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];       // src1-result of the blurring convolution between 5x5 filter & Tim1
unsigned char src2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];      // src2-result of the blurring convolution between 5x5 filter & Tim2
unsigned char src3[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];	   // src3-result of the blurring convolution between 7x7 filter & Tim1
unsigned char src4[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // src4-result of the blurring convolution between 7x7 filter & Tim2

unsigned char dst13_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];     // dest13-result of the restore convolution between 3x3 kernel & src1
unsigned char dst15_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest15-result of the restore convolution between 5x5 kernel & src1
unsigned char dst17_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest17-result of the restore convolution between 7x7 kernel & src1

unsigned char dst33_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];     // dest33-result of the restore convolution between 3x3 kernel & src3
unsigned char dst35_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest35-result of the restore convolution between 5x5 kernel & src3
unsigned char dst37_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest37-result of the restore convolution between 7x7 kernel & src3

unsigned char dst23_option3[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];     // dest23-result of the restore convolution between 3x3 kernel & src2
unsigned char dst25_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest15-result of the restore convolution between 5x5 kernel & src2
unsigned char dst27_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest27-result of the restore convolution between 5x5 kernel & src2

unsigned char dst43_option3[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];     // dest43-result of the restore convolution between 3x3 kernel & src4
unsigned char dst45_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest45-result of the restore convolution between 5x5 kernel & src4
unsigned char dst47_option2[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];    // dest47-result of the restore convolution between 7x7 kernel & src4

void main()
{
	//part 2 of the task - "By using reworked 2D 5x5 and 7x7 filters BLUR Image Tim1.bmp by using convolution with Fast Gaussian 1D Filters"
	cout << "Gaussian filtration for blurring the input Tim1.bmp and Tim2.bmp images" << endl;

	// Apply a 5x5 Gaussian filter
	const int FILTER_HALF_SIZE_5 = 2;
	double offset = 0.23;
	const int FILTER_SIZE_5 = 2 * FILTER_HALF_SIZE_5 + 1;
	double filter_5[FILTER_SIZE_5] = { 7 / 273 + offset, 26 / 273 + offset, 41 / 273 + offset, 26 / 273 + offset, 7 / 273 + offset };  // Use the provided filter

	LoadGrayImageFromTrueColorBmpFile(src1, "Tim1.bmp");  // Load the image
	DoGaussianFiltration(&src1[0][0], filter_5, FILTER_SIZE_5);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src1, "Tim1LPF5c.bmp");  // Save the blurred image with 5x5 filter

	// Apply a 7x7 Gaussian filter
	const int FILTER_HALF_SIZE_7 = 3;
	const int FILTER_SIZE_7 = 2 * FILTER_HALF_SIZE_7 + 1;
	double offset2 = 0.16;
	double filter_7[FILTER_SIZE_7] = { 2 / 1003 + offset2, 22 / 1003 + offset2, 97 / 1003 + offset2, 159 / 1003 + offset2, 97 / 1003 + offset2, 22 / 1003 + offset2, 2 / 1003 + offset2 };  // Use the provided filter

	LoadGrayImageFromTrueColorBmpFile(src3, "Tim1.bmp");  // Reload the original image
	DoGaussianFiltration(&src3[0][0], filter_7, FILTER_SIZE_7);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src3, "Tim1LPF7c.bmp");  // Save the blurred image with 7x7 filter

	
	//********* part 4 of the task - try to enhance/restore the blurred images using convolution in TD *************

	DoFiltationByConvolution(src1, dst13_option2, &kernel_3x3_option2_High_Boost_Filter[0][0], 3);
	StoreGrayImageAsGrayBmpFile(dst13_option2, "dest13_option2.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src1, dst15_option2, &kernel_5x5_option2_High_Boost_Filter[0][0], 5);
	StoreGrayImageAsGrayBmpFile(dst15_option2, "dest15_option2.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src1, dst17_option2, &kernel_7x7_option2_High_Boost_Filter[0][0], 7);
	StoreGrayImageAsGrayBmpFile(dst17_option2, "dest17_option2.bmp");


	DoFiltationByConvolution(src3, dst33_option2, &kernel_3x3_option2_High_Boost_Filter[0][0], 3);
	StoreGrayImageAsGrayBmpFile(dst33_option2, "dest33_option2.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src3, dst35_option2, &kernel_5x5_option2_High_Boost_Filter[0][0], 5);
	StoreGrayImageAsGrayBmpFile(dst35_option2, "dest35_option2.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src3, dst37_option2, &kernel_7x7_option2_High_Boost_Filter[0][0], 7);
	StoreGrayImageAsGrayBmpFile(dst37_option2, "dest37_option2.bmp");

	///// now we will examine the same for Tim2.bmp : 
	LoadGrayImageFromGrayBmpFile(src4, "Tim2.bmp");  // Load the image
	DoGaussianFiltration(&src4[0][0], filter_7, FILTER_SIZE_7);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src4, "Tim2LPF7c.bmp");  // Save the blurred image with 7x7 filter

	LoadGrayImageFromGrayBmpFile(src2, "Tim2.bmp");  // Load the image
	DoGaussianFiltration(&src2[0][0], filter_5, FILTER_SIZE_5);  // Perform the filtration
	StoreGrayImageAsGrayBmpFile(src2, "Tim2LPF5c.bmp");  // Save the blurred image with 5x5 filter

	//-------------------------------------------------------------------------//

	DoFiltationByConvolution(src2, dst23_option3, &kernel_3x3_option3_Unsharp_Masking_Filter[0][0], 3);
	StoreGrayImageAsGrayBmpFile(dst23_option3, "dest23_option3.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src2, dst25_option2, &kernel_5x5_option2_High_Boost_Filter[0][0], 5);
	StoreGrayImageAsGrayBmpFile(dst25_option2, "dest25_option2.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src2, dst27_option2, &kernel_7x7_option2_High_Boost_Filter[0][0], 7);
	StoreGrayImageAsGrayBmpFile(dst27_option2, "dest27_option2.bmp");

	//-------------------------------------------------------------------------//
	
	DoFiltationByConvolution(src4, dst43_option3, &kernel_3x3_option3_Unsharp_Masking_Filter[0][0], 3);
	StoreGrayImageAsGrayBmpFile(dst43_option3, "dest43_option3.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src4, dst45_option2, &kernel_5x5_option2_High_Boost_Filter[0][0], 5);
	StoreGrayImageAsGrayBmpFile(dst45_option2, "dest45_option2.bmp");

	//-------------------------------------------------------------------------//
	DoFiltationByConvolution(src4, dst47_option2, &kernel_7x7_option2_High_Boost_Filter[0][0], 7);
	StoreGrayImageAsGrayBmpFile(dst47_option2, "dest47_option2.bmp");


	//********* part 3 of the task - "Blur test Images by using FFT using Gaussian Filter in FD. " *************

	// Load and process the Tim1.bmp image
	LoadGrayImageFromTrueColorBmpFile(ProccesIMG11, "Tim1.bmp");
	StoreGrayImageAsGrayBmpFile(ProccesIMG11, "Tim1_gray.bmp");  //**for debugging

	/// FFT filtration to blur using gaussian filter  
	Work(ProccesIMG11, 600);
	StoreGrayImageAsGrayBmpFile(ProccesIMG11, "Tim1LPFF.bmp");
	
	/// FFT filtration using HPEF to restore (enhance) the blurred image - section 5 
	WorkHPEF(ProccesIMG11, 3);
	StoreGrayImageAsGrayBmpFile(ProccesIMG11, "Tim1HPEF.bmp");

	////**************************************************************
	////***** repeat the steps before on Tim2.bmp given image **********

	// Load and process the Tim2.bmp image
	LoadGrayImageFromGrayBmpFile(ProccesIMG21, "Tim2.bmp");

	/// FFT filtration using gaussian filter
	Work(ProccesIMG21, 400);
	StoreGrayImageAsGrayBmpFile(ProccesIMG21,"Tim2LPFF.bmp");

	//// FFT filtration using HPF to restore (enhance) the blurred image - section 5 
	//WorkHPEF1(ProccesIMG2, 1);
	//StoreGrayImageAsGrayBmpFile(ProccesIMG2, "Tim2HPEF.bmp");

	cout << "Press any key to exit" << endl;
	
}
