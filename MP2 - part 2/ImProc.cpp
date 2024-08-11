#include <stdio.h>      // for printf
#include <conio.h>      // for getch
#include <iostream>     // for cin, cout
#include <fstream>      // For file IO
#include <cmath>        // For math calculations

// BMP Library
#include "ImProcInPlainC.h"
#include "PrimeFFTn.h"  // for basic FFT operations

using namespace std;

void ShiftHalfSize(tFloat floatImage[][NUMBER_OF_COLUMNS]);
void BlurImage(c2dByteGrayImage& LayerName, c2dFloatGrayImage& filterInFD, const char* Load_file_name, const char* file_name, const char* file_name1);
void DoFFT(tFloat imageRe[][NUMBER_OF_COLUMNS], tFloat imageIm[][NUMBER_OF_COLUMNS], int exponentSign, int normalizationScalingType);
void Work();

// This function converts the image from TD to FD and does the convolution in FD with the filter 
void BlurImage(c2dByteGrayImage& LayerName, c2dFloatGrayImage& filterInFD, const char* Load_file_name, const char* file_name, const char* file_name1) {
    LayerName.LoadFromBmpFile(Load_file_name);
    c2dFloatGrayImage RE_OriginalImage(LayerName);
    c2dFloatGrayImage IM_OriginalImage(RE_OriginalImage.NumberOfRows(), RE_OriginalImage.NumberOfColumns());

    ShiftHalfSize(RE_OriginalImage);
    ShiftHalfSize(IM_OriginalImage);
    DoFFT(RE_OriginalImage, IM_OriginalImage, FORWARD_FFT, NORMALIZE_BY_SQRT);
    DoFiltrationInFD(RE_OriginalImage, IM_OriginalImage, filterInFD); // Convolve filter
    ReverseShiftSave(RE_OriginalImage, IM_OriginalImage, file_name, file_name1);
}

// This function converts the image from FD to TD and saves the result image
void ReverseShiftSave(c2dFloatGrayImage& realPart, c2dFloatGrayImage& imagPart, const char* file_name, const char* file_name1) {
    DoFFT(realPart, imagPart, REVERSE_FFT, NORMALIZE_BY_NUMBER);
    ShiftHalfSize(realPart);
    ShiftHalfSize(imagPart);

    realPart.SaveAsOptimalGrayBmpFile(file_name);
    realPart.SaveToGrayBmpFile(file_name1, 0, 1);
}

void DoFFT(tFloat imageRe[][NUMBER_OF_COLUMNS], tFloat imageIm[][NUMBER_OF_COLUMNS], int exponentSign, int normalizationScalingType) {
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

void ShiftHalfSize(tFloat floatImage[][NUMBER_OF_COLUMNS]) {
    for (int row = 0; row < NUMBER_OF_ROWS; row++) {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column += 2) {
            floatImage[row][column] *= -1;
        }
    }

    for (int row = 0; row < NUMBER_OF_ROWS; row += 2) {
        for (int column = 0; column < NUMBER_OF_COLUMNS; column++) {
            floatImage[row][column] *= -1;
        }
    }
}

void Work() {
    cout << "FFT : 1D Sin Filter With Shift Half Size" << endl;

    c2dFloatGrayImage Filter(480, 640);
    CreateFilterImage(Filter, 0, 1, 4, 4); // Create a 640x480 filter with an inner white rectangle of size 4x4

    c2dFloatGrayImage FilterRe(Filter);
    c2dFloatGrayImage FilterIm(FilterRe.NumberOfRows(), FilterRe.NumberOfColumns());
    c2dFloatGrayImage Res(Filter);

    ShiftHalfSize(FilterRe);
    FilterRe.SaveAsOptimalGrayBmpFile("RealPartFilterLPF.bmp");
    ShiftHalfSize(FilterIm);

    DoFFT(FilterRe, FilterIm, FORWARD_FFT, NORMALIZE_BY_SQRT);
    FilterRe.SaveAsOptimalGrayBmpFile("RealPartFFT.bmp");

    Magnitude(Res, FilterRe, FilterIm); // Take only the magnitude
    Res.SaveAsOptimalGrayBmpFile("RESFFT.bmp");

    c2dByteGrayImage OriginalImage(480, 640);
    BlurImage(OriginalImage, Res, "2603-7596-8509_Grayscale.bmp", "Tim2LPFF.bmp", "Tim2LPFF_0_1.bmp");
}
