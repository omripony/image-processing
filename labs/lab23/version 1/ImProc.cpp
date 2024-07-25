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

#define myMAXCOLORS 256
#define PIXEL_VALUES 256


void fill_hist_and_calc_int(unsigned char* img, unsigned int hist[PIXEL_VALUES], unsigned int IntOfHist[PIXEL_VALUES], int rows, int cols) {
    // Initialize histogram to zero
    for (int i = 0; i < PIXEL_VALUES; i++) {
        hist[i] = 0;
    }

    // Calculate histogram
    for (int i = 0; i < rows * cols; i++) {
        hist[*(img + i)] += 1;
    }

    // Calculate integral histogram
    IntOfHist[0] = hist[0];
    for (unsigned i = 1; i < PIXEL_VALUES; i++) {
        IntOfHist[i] = IntOfHist[i - 1] + hist[i];
    }
}

void InitHISTLUT(unsigned char* LUT, unsigned int IntegralHist[]) {
    double F = (double)(myMAXCOLORS - 1) / (double)(NUMBER_OF_ROWS * NUMBER_OF_COLUMNS);
    double calc;
    for (int i = 0; i < myMAXCOLORS; i++) {
        calc = F * IntegralHist[i] + 0.5;
        calc = 255.0 * (int(calc) > 255) + calc * (int(calc) >= 0 && int(calc) <= 255);
        LUT[i] = (unsigned char)(int)(calc);
    }
}

void ApplyLUTOnGrayImage(unsigned char* img, unsigned char* LUT, int rows, int cols) {
	unsigned char* end = img + (rows * cols); // Pointer to the end of the image data
	while (img < end) {
		*img = LUT[*img];
		img++;
	}
}


unsigned char ProccesIMG[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char LUT[256];

void main() {
    unsigned int Hist[PIXEL_VALUES];
    unsigned int Int_Of_Hist[PIXEL_VALUES];

    // First image
    LoadGrayImageFromGrayBmpFile(ProccesIMG, "A20gray.bmp");
    fill_hist_and_calc_int((unsigned char*)ProccesIMG, Hist, Int_Of_Hist, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    InitHISTLUT(LUT, Int_Of_Hist);
    ApplyLUTOnGrayImage((unsigned char*)ProccesIMG, LUT, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    StoreGrayImageAsGrayBmpFile(ProccesIMG, "A230.bmp");

    // Second image
    LoadGrayImageFromGrayBmpFile(ProccesIMG, "A211.bmp");
    fill_hist_and_calc_int((unsigned char*)ProccesIMG, Hist, Int_Of_Hist, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    InitHISTLUT(LUT, Int_Of_Hist);
    ApplyLUTOnGrayImage((unsigned char*)ProccesIMG, LUT, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    StoreGrayImageAsGrayBmpFile(ProccesIMG, "A231.bmp");

    // Third image
    LoadGrayImageFromGrayBmpFile(ProccesIMG, "A212.bmp");
    fill_hist_and_calc_int((unsigned char*)ProccesIMG, Hist, Int_Of_Hist, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    InitHISTLUT(LUT, Int_Of_Hist);
    ApplyLUTOnGrayImage((unsigned char*)ProccesIMG, LUT, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    StoreGrayImageAsGrayBmpFile(ProccesIMG, "A232.bmp");

    // Fourth image
    LoadGrayImageFromGrayBmpFile(ProccesIMG, "A221.bmp");
    fill_hist_and_calc_int((unsigned char*)ProccesIMG, Hist, Int_Of_Hist, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    InitHISTLUT(LUT, Int_Of_Hist);
    ApplyLUTOnGrayImage((unsigned char*)ProccesIMG, LUT, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    StoreGrayImageAsGrayBmpFile(ProccesIMG, "A233.bmp");

    // Fifth image
    LoadGrayImageFromGrayBmpFile(ProccesIMG, "A222.bmp");
    fill_hist_and_calc_int((unsigned char*)ProccesIMG, Hist, Int_Of_Hist, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    InitHISTLUT(LUT, Int_Of_Hist);
    ApplyLUTOnGrayImage((unsigned char*)ProccesIMG, LUT, NUMBER_OF_ROWS, NUMBER_OF_COLUMNS);
    StoreGrayImageAsGrayBmpFile(ProccesIMG, "A234.bmp");

    WaitForUserPressKey();
}
