// CPP BMP Library

#include <conio.h> // for _getch

// the following is needed for cin and cout instead of printf
#include <iostream>
using namespace std;  // explain someday

// The following is needed for math calculations
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath> 
#include "c2dByteGrayImage.h"
#include <time.h>  // for speed measurements

#define BLACKVAL 0
#define WHITEVAL 255

#define NearestNeigbor 1 
#define Bilinear 2

struct AffineTransformParams          //taken from page 9 in lecture about affine transformations
{
    double a11;
    double a12;
    double a21;
    double a22;
    double b1;
    double b2;
};


void CalculateReverseAffineTransform(AffineTransformParams& forward, AffineTransformParams& reverse)  //taken from page 10 in lecture about affine transformations
{
    double det;
    det = forward.a11 * forward.a22 - forward.a21 * forward.a12;
    if (det == 0)
        throw "Cannot calculate reverse Affine Transform: det = 0";
    reverse.a11 = forward.a22 / det;
    reverse.a12 = -forward.a12 / det;
    reverse.b1 = (forward.a12 * forward.b2 - forward.a22 * forward.b1) / det;
    reverse.a21 = -forward.a21 / det;
    reverse.a22 = forward.a11 / det;
    reverse.b2 = -(forward.a11 * forward.b2 - forward.a21 * forward.b1) / det;
}

//this function was not given by samuel - students had to write it alone - to implement the billinear linear interpolation
unsigned char CalcBillinearInterpolatedPixelValue(c2dByteGrayImage& source, double dst_row, double dst_col)
{
    int rows = source.NumberOfRows();
    int cols = source.NumberOfColumns();
    int DestRow = int(dst_row);
    int DestCol = int(dst_col);

    if (source.IsInside(DestRow, DestCol) == false)
        return BLACKVAL;

    if ((DestRow >= (rows - 1)) || (DestCol >= (cols - 1)))
        return -1;

    double x, y;
    double a, b, c, d;
    double I, I00, I01, I10, I11;

    y = dst_row - DestRow;
    x = dst_col - DestCol;

    I00 = source.GetPixelValue(DestRow, DestCol);
    I01 = source.GetPixelValue(DestRow + 1, DestCol);
    I10 = source.GetPixelValue(DestRow, DestCol + 1);
    I11 = source.GetPixelValue(DestRow + 1, DestCol + 1);

    a = I00;
    b = I10 - I00;
    c = I01 - I00;
    d = I00 - I01 - I10 + I11;

    I = a + b * x + c * y + d * x * y;
    return (unsigned char)((int)(I + 0.5));
}


unsigned char CalcInterpolatedPixValue(c2dByteGrayImage& source, double oldRow, double oldColumn, int InterpolationType)    //taken from page 12 in lecture about affine transformations
{
    int OldRow;
    int OldColumn;
    if (InterpolationType == Bilinear)
        return CalcBillinearInterpolatedPixelValue(source, oldRow, oldColumn);
    else {
        OldRow = (int)(oldRow + 0.5);
        OldColumn = (int)(oldColumn + 0.5);
    }
    if (source.IsInside(OldRow, OldColumn) == true)
        return *source.fastAndDangerousPointerToPixel(OldRow, OldColumn);
    else
        return 0;
}


void DoAffineTransform(c2dByteGrayImage& src, c2dByteGrayImage& dest, AffineTransformParams& forwardAffineTransform, int InterpolationType)   //taken from page 11 in lecture about affine transformations
{
    AffineTransformParams reverseAffineTransform;
    CalculateReverseAffineTransform(forwardAffineTransform, reverseAffineTransform);
    double oldRow, oldColumn, interpolatedValue;
    unsigned char* ptrToDestPixel;
    ptrToDestPixel = dest.ScanLine(0);

    for (int row = 0; row < dest.NumberOfRows(); row++)
    {
        for (int column = 0; column < dest.NumberOfColumns(); column++)
        {
            oldColumn =
                reverseAffineTransform.a11 * column
                + reverseAffineTransform.a12 * row
                + reverseAffineTransform.b1;

            oldRow =
                reverseAffineTransform.a21 * column
                + reverseAffineTransform.a22 * row
                + reverseAffineTransform.b2;

            interpolatedValue =
                CalcInterpolatedPixValue(src, oldRow, oldColumn, InterpolationType);
            *ptrToDestPixel++ = (unsigned char)((int)(interpolatedValue + 0.5));
        }
    }
}


void DoSmartAffineTransform(c2dByteGrayImage& source, c2dByteGrayImage& dest,
                            AffineTransformParams& forwardAffineTransform, int InterpolationType)               //taken from page 17-19 in lecture about affine transformations
{
    double MinC;
    double MaxC;
    double MinR;
    double MaxR;
    double tempC;
    double tempR;

    tempC = forwardAffineTransform.b1;
    tempR = forwardAffineTransform.b2;

    MinC = tempC;
    MaxC = MinC;
    MinR = tempR;
    MaxR = MinR;

    tempC = forwardAffineTransform.a11 * (source.NumberOfColumns() - 1) + forwardAffineTransform.b1;
    tempR = forwardAffineTransform.a21 * (source.NumberOfColumns() - 1) + forwardAffineTransform.b2;

    if (MinC > tempC) MinC = tempC;
    if (MaxC < tempC) MaxC = tempC;
    if (MinR > tempR) MinR = tempR;
    if (MaxR < tempR) MaxR = tempR;

    tempC = forwardAffineTransform.a12 * (source.NumberOfRows() - 1) + forwardAffineTransform.b1;
    tempR = forwardAffineTransform.a22 * (source.NumberOfRows() - 1) + forwardAffineTransform.b2;

    if (MinC > tempC) MinC = tempC;
    if (MaxC < tempC) MaxC = tempC;
    if (MinR > tempR) MinR = tempR;
    if (MaxR < tempR) MaxR = tempR;

    tempC = forwardAffineTransform.a11 * (source.NumberOfColumns() - 1) + forwardAffineTransform.a12 * (source.NumberOfRows() - 1) +
                                                                                                        forwardAffineTransform.b1;
    tempR = forwardAffineTransform.a21 * (source.NumberOfColumns() - 1) + forwardAffineTransform.a22 * (source.NumberOfRows() - 1) +
                                                                                                        forwardAffineTransform.b2;

    if (MinC > tempC) MinC = tempC;
    if (MaxC < tempC) MaxC = tempC;
    if (MinR > tempR) MinR = tempR;
    if (MaxR < tempR) MaxR = tempR;

    int newNumberOfRows = (int)(MaxR - MinR + 0.5);
    int newNumberOfColumns = (int)(MaxC - MinC + 0.5);
    forwardAffineTransform.b1 -= MinC;
    forwardAffineTransform.b2 -= MinR;
    dest.Init(newNumberOfRows, newNumberOfColumns);
    DoAffineTransform(source, dest, forwardAffineTransform, InterpolationType);
}


void InsertMatParams(AffineTransformParams& our_struct)
{
    // Given parameters
    double Sx = 1.13;
    double Sy = 2.13;
    double Tx = -40; // Moving left by 40 pixels (negative direction)
    double angleDegrees = 17;
    double angleRadians = angleDegrees * M_PI / 180.0; // Convert to radians
    double cx = 20; // Rotation center x
    double cy = 10; // Rotation center y

    // Rotation matrix components around (cx, cy)
    double cosAngle = cos(angleRadians);
    double sinAngle = sin(angleRadians);

    // Affine transformation parameters
    our_struct.a11 = Sx * cosAngle;
    our_struct.a12 = -Sy * sinAngle;
    our_struct.a21 = Sx * sinAngle;
    our_struct.a22 = Sy * cosAngle;

    // Translation after rotation around the point (cx, cy)
    our_struct.b1 = cx * (1 - Sx * cosAngle) + cy * Sy * sinAngle + Tx;
    our_struct.b2 = cy * (1 - Sy * cosAngle) - cx * Sx * sinAngle;
}


void Work()
{
    c2dByteGrayImage src; // Create clean image
    src.LoadFromBmpFile("grayImage13.bmp"); // Load TV test image
    c2dByteGrayImage dest(src.NumberOfRows(), src.NumberOfColumns());

    AffineTransformParams forwardAffineTransform;
    InsertMatParams(forwardAffineTransform);

    DoSmartAffineTransform(src, dest, forwardAffineTransform, NearestNeigbor);
    dest.SaveToGrayBmpFile("A31.bmp");
    DoSmartAffineTransform(src, dest, forwardAffineTransform, Bilinear);
    dest.SaveToGrayBmpFile("A32.bmp");
}


void main()
{
    Work();
    cout << "Press any key to exit" << endl;
    _getch();
}
