#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"


int main(int argc, char *argv [])
{
    //Make sure the user gave the right amount of arguments
    if(argc!=4)
    {
        printf("Usage: ./resize f infile outfile\n");
        return 1;
    }


    //Get the names of files
    char *infile = argv[2];
    char *outfile = argv[3];

    //Open the files
    FILE *inptr = fopen(infile, "r");
    FILE *outptr = fopen(outfile, "w");

    //Check if files were opened to avoid seg fault
    if(!inptr)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        fclose(inptr);
        return 2;
    }
    if(!outptr)
    {
        fclose(outptr);
        fprintf(stderr, "Could not open %s.\n", outfile);
        return 3;
    }


    //Get the BMP's header
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);


    //Ensure the file is of the right type
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }


    //Get the scale ratio. Another way is: float ratio = atoi(argv[1]);
    float ratio;
    sscanf(argv[1], "%f", &ratio);

    //Get the input padding
    int inPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    printf("Width before: %d, padding before: %i \n", bi.biWidth, inPadding);

    //Update the heaers, and save previous size
    int startingWidth = bi.biWidth;
    bi.biWidth = ceil(bi.biWidth * ratio);
    int startingHeight = bi.biHeight;

    //Height parameter may be negative, rounding must be specified
    if(bi.biHeight < 0)
    {
        bi.biHeight = floor(bi.biHeight * ratio);
    }else
    {
        bi.biHeight = ceil(bi.biHeight * ratio);
    }

    //Choose the right output padding
    int outPadding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    printf("Width after: %d, padding after: %i \n", bi.biWidth, outPadding);

    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + outPadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Write header's first part - file header
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    printf("%f\n", ratio);

    //Write header's second part - info header
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    //Set staring pixel that will control the scaling
    int verictalPosition = 1;
    int *verPosPtr = &verictalPosition;

    //Scan over height
    for(int i = 0, biHeight = abs(bi.biHeight); i<biHeight; i++)
    {
        //Set number of pixel being copied to starting position
        int horizontalPosition = 1;
        int *horPosPtr = &horizontalPosition;
        RGBTRIPLE buffer;
        fread(&buffer, sizeof(RGBTRIPLE), 1, inptr);

        //Scan over width
        for(int j = 0; j<bi.biWidth; j++)
        {
            //Read a new pixel/skip, according to scaling
            while(j >= (*horPosPtr)*ratio)
            {
                fread(&buffer, sizeof(RGBTRIPLE), 1, inptr);
                (*horPosPtr)++;
            }

            //Write the the pixel to the scaled image
            fwrite(&buffer, sizeof(RGBTRIPLE), 1, outptr);
        }

        //Skip input's padding2
        fseek(inptr, inPadding, SEEK_CUR);

        //Add the new padding
        for(int k = 0; k < outPadding; k++)
        {
            fputc(0x00, outptr);
        }


        //Check if previous row should be copied or next one skipped
        if((i+1)/(ratio*(*verPosPtr)) < 1)
        {
            fseek(inptr, -((startingWidth*sizeof(RGBTRIPLE))+inPadding), SEEK_CUR);
        }else if((i+1)/(ratio*(*verPosPtr)) > 1)
        {
            fseek(inptr, ((startingWidth*sizeof(RGBTRIPLE))+inPadding), SEEK_CUR);
        }else
            (*verPosPtr)++;

    }

    //close the files
    fclose(inptr);
    fclose(outptr);

    return 0;
}