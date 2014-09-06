///
/// \author Abhishek Bansal
/// \email discover.ab@gmail.com
/// \brief simple program to convert an image to ascii art image
///

#include <iostream>
#include <fstream>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;

unsigned char mapping[10] ={ 
'@', 
'#', 
'8',
'&', 
'o', 
':', 
'*', 
'.',
' '
};

int ascii(int col)
{
    if(col>230) return 0;
    else if(col>200) return 1;
    else if(col>180) return 2;
    else if(col>160) return 3;
    else if(col>130) return 4;
    else if(col>100) return 5;
    else if(col>70) return 6;
    else if(col>50) return 7;
    else return 8;
}

int main(int argc, char *argv[])
{
    IplImage* img = 0; 
    int height,width,step,channels;
    uchar *data;
    int i,j;
    fstream outfile("output.txt",ios::out);
    
    // load an image  
    img=cvLoadImage("bike.jpg");
    if(!img)
    {
        printf("Could not load image file: %s\n",argv[1]);
        exit(0);
    }
  
    //RgbImage image(img);
    // get the image data
    height    = img->height;
    width     = img->width;
    step      = img->widthStep;
    channels  = img->nChannels;
    data      = (uchar *)img->imageData;

    /* create new image for the grayscale version */

    IplImage *dst = cvCreateImage( cvSize( width, height ), IPL_DEPTH_8U, 1 );
    /* CV_RGB2GRAY: convert RGB image to grayscale */
    cvCvtColor( img, dst, CV_RGB2GRAY );

    // create a window
    cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE); 
    cvMoveWindow("mainWin", 100, 100);
  
  
    for(i=0;i<height;i+=2)
    {
        for(j=0;j<width;j++) 
        {
         
            unsigned char red =255- dst->imageData[i*dst->widthStep  + j];
            int index = ascii(red);
            outfile<<mapping[index];
        }
        outfile<<"\n";
    }

    // show the image
    cvShowImage("mainWin", dst );

    // wait for a key
    cvWaitKey(0);

    // release the image
    cvReleaseImage(&img );
    outfile.close();
    return 0;
}
