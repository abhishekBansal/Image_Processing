#include <iostream>
#include <windows.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

IplImage* DrawHistogram(CvHistogram *hist,float scaleX,float  scaleY,CvScalar color)
{
    // Find the maximum value of the histogram to scale
    // other values accordingly
    float histMax = 0;
    cvGetMinMaxHistValue(hist, 0, &histMax, 0, 0);

    // Create a new blank image based on scaleX and scaleY
    IplImage* imgHist = cvCreateImage(cvSize(256*scaleX, 64*scaleY), 8 ,1);
    cvZero(imgHist);

    // Go through each bin
    for(int i=0;i<255;i++)
    {
        float histValue = cvQueryHistValue_1D(hist, i);		// Get value for the current bin...
        float nextValue = cvQueryHistValue_1D(hist, i+1);	// ... and the next bin

        // Calculate the four points of the polygon that these two 
        // bins enclose
        CvPoint pt1 = cvPoint(i*scaleX, 64*scaleY);
        CvPoint pt2 = cvPoint(i*scaleX+scaleX, 64*scaleY);
        CvPoint pt3 = cvPoint(i*scaleX+scaleX, (64-nextValue*64/histMax)*scaleY);
        CvPoint pt4 = cvPoint(i*scaleX, (64-histValue*64/histMax)*scaleY);
        
        // A close convex polygon
        int numPts = 5;
        CvPoint pts[] = {pt1, pt2, pt3, pt4, pt1};

        // Draw it to the image
        cvFillConvexPoly(imgHist, pts, numPts, cvScalar(255));
    }

    // Return it... make sure you delete it once you're done!
    return imgHist;
}

int main(int argc, char *argv[])
{
    IplImage* img = 0; 
    int height,width,step,channels;
    uchar *data;
    // load an image  
    img=cvLoadImage("visitor.jpg");
    if(!img){
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
    cvNamedWindow("inputImage", CV_WINDOW_AUTOSIZE); 
    cvMoveWindow("inputImage", 100, 100);

    cvShowImage("inputImage", dst );

    //now start declaring histograms
    int hist_size = 256;
    float range[] = {0,256};
    float *ranges[] = {range};

    IplImage *grey_img, *grey_histogram;
    CvHistogram* hist_grey = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

    //now calculate imageHistogram
    cvCalcHist( &dst, hist_grey, 0, NULL );

    //now create a image and show histogram in it 
    CvScalar grey=cvScalar(1.0,1.0,1.0);
    IplImage* imgHistgrey = DrawHistogram(hist_grey,1.0,1.0,grey);
    cvNamedWindow("inputHistogram", CV_WINDOW_AUTOSIZE); 
    cvMoveWindow("inputHistogram", 260,260 );
    cvShowImage("inputHistogram",imgHistgrey);

    //calculating cdf
    int cdf[256];
    for (int i=0;i<256;i++)
    {
       cdf[i]=cvQueryHistValue_1D(hist_grey,i);	
    }
    //now accummulate
    int cdfMin=0;
    for(int i=1;i<256;i++)
    {
      cdf[i]=cdf[i]+cdf[i-1];
        std::cout<<cdf[i-1]<<"\t";
    }
    std::cout<<cdf[255]<<"\t";
    for(int i=0;i<256;i++)
      if(cdf[i]!=0)
      {
          cdfMin=cdf[i];
          std::cout<<"\n cdf min : "<<cdfMin;
          break;
      }


    IplImage *final = cvCreateImage( cvSize( width, height ), IPL_DEPTH_8U, 1 );

    for(int i=0; i<height;i++)
      for(int j=0;j<width;j++)
      {
          uchar val = dst->imageData [i * dst->widthStep + j] ;
          double numerator =cdf [ val ] - cdfMin;
          double den = (dst->width * dst->height) - cdfMin;
          double temp =numerator / den;
          int pixelValue =(int)((temp * 255) );

          final->imageData[i*dst->widthStep+j]=(char)pixelValue;
      }

    cvNamedWindow("ResultImage", CV_WINDOW_AUTOSIZE); 
    cvMoveWindow("ResultImage", 300, 300);
    cvShowImage("ResultImage", final );

    //creating hiostogram fr final
    IplImage *res_img, *res_histogram;
    CvHistogram* res_grey = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

    //now calculate imageHistogram
    cvCalcHist( &final, res_grey, 0, NULL );

    //now create a image and show histogram in it 
    IplImage* imgHistRes = DrawHistogram(res_grey,1.0,1.0,grey);
    cvNamedWindow("ResultHistogram", CV_WINDOW_AUTOSIZE); 
    cvMoveWindow("ResultHistogram", 260,260 );
    cvShowImage("ResultHistogram",imgHistRes);


    // wait for a key
    cvWaitKey(0);

    // release the image
    cvReleaseImage(&img );

    return 0;
}
