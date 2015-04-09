#include "Image.h"
#include "Color.h"

#include <stdio.h>
#include <stdlib.h>


Image::Image(int xres, int yres): xres(xres), yres(yres)
{
    image =new Color*[yres];
        for (int i=0;i<yres;i++)
                image[i] = new Color[xres];
}


Image::~Image()
{
    if(image)
        {
                for (int i=0;i<yres;i++)
                        delete[] image[i];
                delete[] image;
    }
}


void Image::Save(char* filename)
{
        FILE *file;
        int count=0;
        int r, g, b;

        file = fopen( filename, "w" );
        if (!file)
                {
                        fprintf( stderr, "Error: Unable to write to %s!\n", filename );
                        return;
                }

        fprintf( file, "P3\n# Image output from Image::save()!\n%d %d\n%d\n", xres, yres, 255 );

        for (int j=yres-1;j>=0;j--)
                for (int i=0;i<xres;i++)
                {
                        r = (int)(image[j][i].red()*255);
                        g = (int)(image[j][i].green()*255);
                        b = (int)(image[j][i].blue()*255);
                        r = ( r>255 ? 255 : (r<0 ? 0 : r) );
                        g = ( g>255 ? 255 : (g<0 ? 0 : g) );
                        b = ( b>255 ? 255 : (b<0 ? 0 : b) );
                        fprintf( file, "%d %d %d ", r, g, b );
                        if ((++count % 5) == 0)
                                fprintf( file, "\n");
                }

        fclose( file );
}
