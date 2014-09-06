#include <iostream>
#include <windows.h>
#include <winbase.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <math.h>

using namespace std;

#define path "penguins.jpeg"

int dithering[4][4]={ {7,8,9,10},{6,1,2,11},{5,4,3,12},{0,15,14,13}}; //clustered
//int dithering[4][4]={ {1,9,3,11},{13,5,15,7},{4,12,2,10},{16,8,14,6}}; //ordered or dispersed

void main()
{
    IplImage *image=cvLoadImage(path);
    if(!image)
    {
        cout<<"masterImage cannot be loaded!!! ";
        exit(0);
    }
    cout<<endl<<"Master Image loaded successfully....!!";
    
    IplImage *masterGreyImage= cvCreateImage(cvSize(image->width,image->height),IPL_DEPTH_8U,1);
    cvCvtColor( image, masterGreyImage, CV_RGB2GRAY );

    cvSaveImage("greyImage.bmp",masterGreyImage);

    IplImage *resultImage= cvCreateImage(cvSize(masterGreyImage->width,masterGreyImage->height),IPL_DEPTH_8U,1);

    for(int p=0;p<masterGreyImage->height;p++)
    {
        for(int q=0; q<masterGreyImage->width; q++)
        {
            uchar pixelVal = masterGreyImage->imageData[p*masterGreyImage->widthStep + q];
            if(pixelVal> dithering[p%4][q%4]*16)
            {
                CvScalar value = cvScalar(255);
                cvSet2D(resultImage,p,q,value);
            }
            else{
                CvScalar value = cvScalar(0);
                cvSet2D(resultImage,p,q,value);
            }
        }
    }
    cvSaveImage("result.bmp",resultImage);

}