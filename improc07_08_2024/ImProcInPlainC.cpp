#include "ImProcInPlainC.h"
#include <conio.h> // for getch
#include <stdio.h> // for old C-Style file operations 
#include <stdlib.h> // for exit

void WaitForUserPressKey()
{
	printf("Press any key to exit");
	_getch(); 
}

bool RowIsProperlyAligned( int rowSize )
{
	if (  ((((rowSize+3)/4)*4)-rowSize) == 0 )
	{
		return true;
	};
	return false;
}

// temporary storage
unsigned char localTempColorImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS];
unsigned char localTempGrayImage[NUMBER_OF_ROWS][NUMBER_OF_COLUMNS];
unsigned char localGrayPalette[ myMAXCOLORS*4]; // 4 not 3

// Beware: sizeof is not always what we need because of padding of compiler
tDWord sizeOfmyBITMAPINFOHEADER = 40; // sizeof( myBITMAPINFOHEADER ) = 40;
tDWord sizeOfmyBITMAPFILEHEADER = 14; // sizeof( myBITMAPFILEHEADER ) = 16 != 14;


void FillGrayPalette()
{
  for ( int i=0; i< myMAXCOLORS; i++) 
  { 
    localGrayPalette[i*4]     = (unsigned char) i;     // Blue 
    localGrayPalette[i*4+1]   = (unsigned char) i;     // Green 
    localGrayPalette[i*4+2]   = (unsigned char) i;     // Red 
    localGrayPalette[i*4+3]   =  0;                    // Reserved 
  } 
}


void ConvertGrayImageToColorImage( unsigned char source[][NUMBER_OF_COLUMNS],
								   unsigned char destination[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS])
{
	for ( int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
		{
           destination[row][column][B] = source[row][column];
		   destination[row][column][G] = source[row][column];
		   destination[row][column][R] = source[row][column];
		}
	}
}

void ConvertColorImageToGrayImage( unsigned char source[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
								   unsigned char destination[][NUMBER_OF_COLUMNS])
{
	int temp;
	for ( int row = 0; row < NUMBER_OF_ROWS; row++)
	{
		for (int column = 0; column < NUMBER_OF_COLUMNS; column++)
		{
            temp =    (int)source[row][column][B]
                    + (int)source[row][column][G]
			        + (int)source[row][column][R];

	    	destination[row][column] = (unsigned char) (temp /3);
		}
	}
}

void StoreBgrImageAsTrueColorBmpFile( unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
									  char bmpFileName[] )
{
    if ( RowIsProperlyAligned( NUMBER_OF_COLORS*NUMBER_OF_COLUMNS ) == false )
	{
		printf("Fatal error: Row size %d\n is not properly aligned\n",
			                 NUMBER_OF_COLUMNS*NUMBER_OF_COLORS );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
	}

	myBITMAPFILEHEADER bf;

	bf.bfType = 0;
	bf.bfOffBits = 0;
	bf.bfSize = 0;

	// Beware: sizeof is not always what we need because of padding of compiler
    tDWord sizeOfmyBITMAPINFOHEADER = 40; // sizeof( myBITMAPINFOHEADER ) = 40;
	tDWord sizeOfmyBITMAPFILEHEADER = 14; // sizeof( myBITMAPFILEHEADER ) = 16 != 14;
	
    bf.bfType = myBMP_TAG;

	bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
 
    bf.bfOffBits =     sizeOfmyBITMAPINFOHEADER 
                     + sizeOfmyBITMAPFILEHEADER 
					 + 0;  // no palette here
		
    bf.bfSize =   (tDWord) NUMBER_OF_ROWS 
                * (tDWord) ( NUMBER_OF_COLUMNS * NUMBER_OF_COLORS ) // no need for alignment here
				+ (tDWord) bf.bfOffBits ; 

    // int A[62];
	
	myBITMAPINFOHEADER bi;
    
	bi.biSize        = sizeOfmyBITMAPINFOHEADER; // sizeof( myBITMAPINFOHEADER ); 
    bi.biWidth       = NUMBER_OF_COLUMNS;
    bi.biHeight      = NUMBER_OF_ROWS;
    bi.biPlanes      = 1;
    bi.biBitCount    = 24;
    bi.biCompression = 0;
    
    bi.biSizeImage = 0; // for RGB images	                  
 
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed       = 0;
    bi.biClrImportant  = 0;

    /////////// 
    FILE * bmpFile; 
	errno_t err;

	// Old C syntax was:
    // bmpFile = fopen( bmpFileName, "wb" );  mus be replaced to fopen_s
    //  Do not ask about new syntax: old unclear, unreliable but fast and compact C
	err = fopen_s( &bmpFile, bmpFileName, "wb" );  
    if ( err != 0 ) // old way of error handling
    { 
		printf("Fatal error: Can not open file: \n%s\nfor writing\n", bmpFileName );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
    }

	size_t res; 
	
/* 
    // This does not work here because of size 14 (not multiple of 4) : special compiler switch is needed 
    res = fwrite( &bf, 1, sizeOfmyBITMAPFILEHEADER, bmpFile ); 
    if ( res != sizeOfmyBITMAPFILEHEADER ) 
    { 
       report error ...
    }
*/
	// wrap-out: hard manual work
    res = fwrite( &bf.bfType, 1, 2, bmpFile ); 

	if ( res != 2 )
	{
		fclose( bmpFile );
		printf("Fatal error: Can not write myBITMAPFILEHEADER to file: \n%s\n", bmpFileName );	 	    
        WaitForUserPressKey();
	    exit(-1); // exit with some error
	}

	// Hope that write will be OK here, no error checks ...
	res = fwrite( &bf.bfSize, 1, 4, bmpFile ); 
    res = fwrite( &bf.bfReserved1, 1, 2, bmpFile );
	res = fwrite( &bf.bfReserved2, 1, 2, bmpFile );
	res = fwrite( &bf.bfOffBits, 1, 4, bmpFile );

    // This work because of size 40
    res = fwrite( &bi, 1, sizeOfmyBITMAPINFOHEADER, bmpFile ); 
    if ( res != sizeOfmyBITMAPINFOHEADER ) 
    { 
     fclose( bmpFile );
	 printf("Fatal error: Can not write myBITMAPINFOHEADER to file: \n%s\n", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    } 

	// No need for row alignment here: fast work
	int rowSize = NUMBER_OF_COLUMNS*NUMBER_OF_COLORS;
	int dataSizeInBytes = NUMBER_OF_ROWS * NUMBER_OF_COLUMNS * NUMBER_OF_COLORS;
	res = fwrite( image, 1, dataSizeInBytes , bmpFile ); 
    if ( res != (unsigned int) dataSizeInBytes ) 
    { 
       fclose( bmpFile ); 
	   printf("Fatal error: Can not write pixels to file: \n%s\n", bmpFileName );         
       WaitForUserPressKey();
	   exit(-1); // exit with some error
    }
	
	fclose( bmpFile ); 
    printf("True Color BMP File\n   %s\n      was created\n", bmpFileName );
}

void StoreBgrImageAsGrayBmpFile( unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
								 char bmpFileName[] )
{	
	// Bad memory efficiency, but fast code development
    ConvertColorImageToGrayImage( image,
								  localTempGrayImage);
    StoreGrayImageAsGrayBmpFile( localTempGrayImage, bmpFileName );
}

void StoreGrayImageAsTrueColorBmpFile( unsigned char image[][NUMBER_OF_COLUMNS],
									   char bmpFileName[] )
{
	// Bad memory efficiency, but fast code development
    ConvertGrayImageToColorImage( image,
								  localTempColorImage);
    StoreBgrImageAsTrueColorBmpFile( localTempColorImage, bmpFileName );
}

void StoreGrayImageAsGrayBmpFile( unsigned char image[][NUMBER_OF_COLUMNS],
								  char bmpFileName[] )
{
    if ( RowIsProperlyAligned( NUMBER_OF_COLUMNS ) == false )
	{
		printf("Fatal error: Row size %d\n is not properly aligned\n",
			                 NUMBER_OF_COLUMNS );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
	} 
	FillGrayPalette();

	myBITMAPFILEHEADER bf;

	bf.bfType = 0;
	bf.bfOffBits = 0;
	bf.bfSize = 0;

    bf.bfType = myBMP_TAG;

	bf.bfReserved1 = 0;
    bf.bfReserved2 = 0;
 
    bf.bfOffBits =     sizeOfmyBITMAPINFOHEADER 
                     + sizeOfmyBITMAPFILEHEADER 
					 + myMAXCOLORS*4;  // Gray Palette present
		
    bf.bfSize =   (tDWord) NUMBER_OF_ROWS 
                * (tDWord) ( NUMBER_OF_COLUMNS ) // no need for alignment here
				+ (tDWord) bf.bfOffBits ; 
 
	
	myBITMAPINFOHEADER bi;
    
	bi.biSize        = sizeOfmyBITMAPINFOHEADER; // sizeof( myBITMAPINFOHEADER ); 
    bi.biWidth       = NUMBER_OF_COLUMNS;
    bi.biHeight      = NUMBER_OF_ROWS;
    bi.biPlanes      = 1;
    bi.biBitCount    = 8;
    bi.biCompression = 0;
    
    bi.biSizeImage = NUMBER_OF_ROWS*NUMBER_OF_COLUMNS; // needed here	                  
 
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed       = 0;
    bi.biClrImportant  = 0;

    /////////// 
    FILE * bmpFile; 
	errno_t err;

	// Old C syntax was:
    // bmpFile = fopen( bmpFileName, "wb" );  mus be replaced to fopen_s
    //  Do not ask about new syntax: old unclear, unreliable but fast and compact C
	err = fopen_s( &bmpFile, bmpFileName, "wb" );  
    if ( err != 0 ) // old way of error handling
    { 
		printf("Fatal error: Can not open file: \n%s\nfor writing\n", bmpFileName );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
    }

	size_t res; 
	
/* 
    // This does not work here because of size 14 (not multiple of 4) : special compiler switch is needed 
    res = fwrite( &bf, 1, sizeOfmyBITMAPFILEHEADER, bmpFile ); 
    if ( res != sizeOfmyBITMAPFILEHEADER ) 
    { 
       report error ...
    }
*/
	// wrap-out: hard manual work
    res = fwrite( &bf.bfType, 1, 2, bmpFile ); 

	if ( res != 2 )
	{
		fclose( bmpFile );
		printf("Fatal error: Can not write myBITMAPFILEHEADER to file: \n%s\n", bmpFileName );	 	    
        WaitForUserPressKey();
	    exit(-1); // exit with some error
	}

	// Hope that write will be OK here, no error checks ...
	res = fwrite( &bf.bfSize, 1, 4, bmpFile ); 
    res = fwrite( &bf.bfReserved1, 1, 2, bmpFile );
	res = fwrite( &bf.bfReserved2, 1, 2, bmpFile );
	res = fwrite( &bf.bfOffBits, 1, 4, bmpFile );

    // This work because of size 40
    res = fwrite( &bi, 1, sizeOfmyBITMAPINFOHEADER, bmpFile ); 
    if ( res != sizeOfmyBITMAPINFOHEADER ) 
    { 
     fclose( bmpFile );
	 printf("Fatal error: Can not write myBITMAPINFOHEADER to file: \n%s\n", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    } 

    // write palette
	res = fwrite( localGrayPalette, 1, myMAXCOLORS*4, bmpFile ); 
    if ( res != myMAXCOLORS*4 ) 
    { 
      fclose( bmpFile ); 
      printf("Fatal error: Can not write myGrayPalette to file: \n%s\n", bmpFileName );    		     
	  WaitForUserPressKey();
	  exit(-1); // exit with some error
  } 


	// No need for row alignment here: fast work
	int rowSize = NUMBER_OF_COLUMNS;
	int dataSizeInBytes = NUMBER_OF_ROWS * NUMBER_OF_COLUMNS;
	res = fwrite( image, 1, dataSizeInBytes , bmpFile ); 
    if ( res != (unsigned int) dataSizeInBytes ) 
    { 
       fclose( bmpFile ); 
	   printf("Fatal error: Can not write pixels to file: \n%s\n", bmpFileName );         
       WaitForUserPressKey();
	   exit(-1); // exit with some error
    }
	
	fclose( bmpFile ); 
    printf("Gray BMP File\n   %s\n      was created\n", bmpFileName );
}

void LoadBgrImageFromTrueColorBmpFile( unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
									   char bmpFileName[] )
{
	FILE * bmpFile; 
	errno_t err;

   	// Old C syntax was:
    // bmpFile = fopen( bmpFileName, "rb" );  mus be replaced to fopen_s
    //  Do not ask about new syntax: old unclear, unreliable but fast and compact C
	err = fopen_s( &bmpFile, bmpFileName, "rb" );  
    if ( err != 0 ) // old way of error handling
    { 
		printf("Fatal error: Can not open file: \n%s\nfor reading\n", bmpFileName );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
    }
      
    myBITMAPFILEHEADER bf;

    size_t res;

	//res = fread( &bf, 1, sizeOfmyBITMAPFILEHEADER, bmpFile ); - this does not work because of 14
	res = fread( &bf.bfType, 1, 2, bmpFile );
    
	if ( res != 2 ) 
    { 
     fclose( bmpFile );
	 printf("Fatal error: Can not read BITMAPFILEHEADER from file: \n%s\n", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    } 
	res = fread( &bf.bfSize, 1, 4, bmpFile );
	res = fread( &bf.bfReserved1, 1, 2, bmpFile );
	res = fread( &bf.bfReserved2, 1, 2, bmpFile );
	res = fread( &bf.bfOffBits, 1, 4, bmpFile );

    // Provide some checks:
    if ( bf.bfType != myBMP_TAG )
    {
	 fclose( bmpFile );
	 printf("Fatal error: File: \n%s\n  is not BMP file", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    }

    myBITMAPINFOHEADER bi;

    res = fread( &bi, 1, sizeOfmyBITMAPINFOHEADER, bmpFile ); 
    if ( res != sizeOfmyBITMAPINFOHEADER ) 
    {
	 fclose( bmpFile );
	 printf("Fatal error: Can not read BITMAPINFOHEADER from file: \n%s\n", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    }

	if ( bi.biBitCount!=24 )
    {
	 fclose( bmpFile );
	 printf("Fatal error: File: \n%s\n  is not True Color BMP file", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    }

    if ( RowIsProperlyAligned( bi.biWidth*NUMBER_OF_COLORS ) == false )
	{
		printf("Fatal error: Row size %d\n is not properly aligned\n",
			                 bi.biWidth*NUMBER_OF_COLORS );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
	}

	if ( bi.biHeight != NUMBER_OF_ROWS )
	{
      fclose( bmpFile );
	  printf("Fatal error: Height of bmp file: \n%s\n is not %d\n", bmpFileName, NUMBER_OF_ROWS );     
      WaitForUserPressKey();
	  exit(-1); // exit with some error
	}
	if ( bi.biWidth != NUMBER_OF_COLUMNS )
	{
      fclose( bmpFile );
	  printf("Fatal error: Width of bmp file: \n%s\n is not %d\n", bmpFileName, NUMBER_OF_COLUMNS );     
      WaitForUserPressKey();
	  exit(-1); // exit with some error
	}

	// Other values of the header must be checked.
	// The problem is that many third party software 
	// dose not fill all the fields properly

	
	int dataSizeInBytes = bi.biWidth*bi.biHeight*NUMBER_OF_COLORS;
	res = fread( image, 1, dataSizeInBytes , bmpFile ); 
    if ( res != (unsigned int) dataSizeInBytes ) 
    { 
       fclose( bmpFile ); 
	   printf("Fatal error: Can not read pixels from file: \n%s\n", bmpFileName );         
       WaitForUserPressKey();
	   exit(-1); // exit with some error
    }
	
	fclose( bmpFile ); 
}

void LoadBgrImageFromGrayBmpFile( unsigned char image[][NUMBER_OF_COLUMNS][NUMBER_OF_COLORS],
								  char bmpFileName[] )
{
    LoadGrayImageFromGrayBmpFile( localTempGrayImage, bmpFileName );
	ConvertGrayImageToColorImage( localTempGrayImage, image );
}

void LoadGrayImageFromTrueColorBmpFile( unsigned char image[][NUMBER_OF_COLUMNS],
									    char bmpFileName[] )
{
	LoadBgrImageFromTrueColorBmpFile( localTempColorImage, bmpFileName );
	ConvertColorImageToGrayImage( localTempColorImage, image);
}

void LoadGrayImageFromGrayBmpFile( unsigned char image[][NUMBER_OF_COLUMNS],
								   char bmpFileName[] )
{
    FILE * bmpFile; 
	errno_t err;

   	// Old C syntax was:
    // bmpFile = fopen( bmpFileName, "rb" );  mus be replaced to fopen_s
    //  Do not ask about new syntax: old unclear, unreliable but fast and compact C
	err = fopen_s( &bmpFile, bmpFileName, "rb" );  
    if ( err != 0 ) // old way of error handling
    { 
		printf("Fatal error: Can not open file: \n%s\nfor reading\n", bmpFileName );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
    }
      
    myBITMAPFILEHEADER bf;

    size_t res;

	res = fread( &bf.bfType, 1, 2, bmpFile );

    //res = fread( &bf, 1, sizeOfmyBITMAPFILEHEADER, bmpFile ); 
    
	if ( res != 2 ) 
    { 
     fclose( bmpFile );
	 printf("Fatal error: Can not read BITMAPFILEHEADER from file: \n%s\n", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    } 
	res = fread( &bf.bfSize, 1, 4, bmpFile );
	res = fread( &bf.bfReserved1, 1, 2, bmpFile );
	res = fread( &bf.bfReserved2, 1, 2, bmpFile );
	res = fread( &bf.bfOffBits, 1, 4, bmpFile );

    // Provide some checks:
    if ( bf.bfType != myBMP_TAG )
    {
	 fclose( bmpFile );
	 printf("Fatal error: File: \n%s\n  is not BMP file", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    }

    myBITMAPINFOHEADER bi;

    res = fread( &bi, 1, sizeOfmyBITMAPINFOHEADER, bmpFile ); 
    if ( res != sizeOfmyBITMAPINFOHEADER ) 
    {
	 fclose( bmpFile );
	 printf("Fatal error: Can not read BITMAPINFOHEADER from file: \n%s\n", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    }

	if ( bi.biBitCount!=8 )
    {
	 fclose( bmpFile );
	 printf("Fatal error: File: \n%s\n  is not Gray BMP file", bmpFileName );     
     WaitForUserPressKey();
	 exit(-1); // exit with some error
    }

    if ( RowIsProperlyAligned( bi.biWidth ) == false )
	{
		printf("Fatal error: Row size %d\n is not properly aligned\n",
			                 bi.biWidth );		
        WaitForUserPressKey();
		exit(-1); // exit with some error
	}

	if ( bi.biHeight != NUMBER_OF_ROWS )
	{
      fclose( bmpFile );
	  printf("Fatal error: Height of bmp file: \n%s\n is not %d\n", bmpFileName, NUMBER_OF_ROWS );     
      WaitForUserPressKey();
	  exit(-1); // exit with some error
	}
	if ( bi.biWidth != NUMBER_OF_COLUMNS )
	{
      fclose( bmpFile );
	  printf("Fatal error: Width of bmp file: \n%s\n is not %d\n", bmpFileName, NUMBER_OF_COLUMNS );     
      WaitForUserPressKey();
	  exit(-1); // exit with some error
	}

	// Other values of the header must be checked.
	// The problem is that many third party software 
	// dose not fill all the fields properly

    res = fread( localGrayPalette, 1, myMAXCOLORS*4, bmpFile ); 
    if ( res != myMAXCOLORS*4 ) 
    { 
      fclose( bmpFile );
	  printf("Fatal error: Can not read PALETTE from file: \n%s\n", bmpFileName );     
      WaitForUserPressKey();
	  exit(-1); // exit with some error
    }
    // Palette must be checked

	int dataSizeInBytes = bi.biWidth*bi.biHeight;
	res = fread( image, 1, dataSizeInBytes , bmpFile ); 
    if ( res != (unsigned int) dataSizeInBytes ) 
    { 
       fclose( bmpFile ); 
	   printf("Fatal error: Can not read pixels from file: \n%s\n", bmpFileName );         
       WaitForUserPressKey();
	   exit(-1); // exit with some error
    }
	
	fclose( bmpFile ); 
}


