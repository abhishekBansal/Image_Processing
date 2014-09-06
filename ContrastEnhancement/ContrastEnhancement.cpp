#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <windows.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <math.h>
#include "linklist.h"
#include<GL\glut.h>

#define image "testImage.bmp"
#define xOfset 20
#define yOfset 20

GLuint main_window;
linklist redList,greenList,blueList;
int mouseX=0,mouseY=0;
bool red=false,green=false,blue=false;
IplImage *img=0;
IplImage *dstimg=0;
int width,height;
int glWinWidth =300;
int glWinHeight=300;

enum colors
{
    RED=0,
    GREEN,
    BLUE
};

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
void initOpenCV()
{
  int hist_size = 256;
  float range[] = {0,256};
  float *ranges[] = {range};

  IplImage *red_img;
  IplImage *green_img;
  IplImage *blue_img;

  CvHistogram* hist_red = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  CvHistogram* hist_blue = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  CvHistogram* hist_green = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);


  img=cvLoadImage("testImage.bmp");
  if(!img){
    printf("Could not load image file\n");
    //exit(0);
  }
  width=img->width;
  height=img->height;


   // create a window
  cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("mainWin", 100, 100);
  cvShowImage("mainWin", img);

  blue_img = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
  green_img = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
  red_img = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1 );
  
  cvCvtPixToPlane( img, blue_img, green_img, red_img, 0 );

  cvCalcHist( &red_img, hist_red, 0, NULL );
  cvCalcHist( &blue_img, hist_blue, 0, NULL );
  cvCalcHist( &green_img, hist_green, 0, NULL );

  CvScalar red=cvScalar(1.0,0.0,0.0);
  IplImage* imgHistRed = DrawHistogram(hist_red,1.0,1.0,red);
  cvNamedWindow("redHist", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("redHist", 260,260 );
  cvShowImage("redHist",imgHistRed);

  CvScalar green=cvScalar(0.0,1.0,0.0);
  IplImage* imgHistGreen = DrawHistogram(hist_green,1.0,1.0,green);
  cvNamedWindow("GreenHist", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("GreenHist", 260,360 );
  cvShowImage("GreenHist",imgHistGreen);

  CvScalar blue=cvScalar(0.0,0.0,1.0);
  IplImage* imgHistBlue = DrawHistogram(hist_blue,1.0,1.0,blue);
  cvNamedWindow("BlueHist", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("BlueHist", 260,460 );
  cvShowImage("BlueHist",imgHistBlue);
}
int interpolate(int x1,int y1,int x2,int y2,int pos)
{
    float m = (y2-y1)/(x2-x1);
    int y=0;
    int j=0;
    y = m*(pos-x1)+(float)y1;
    return y;
}
char mapColor(int color, int x) //color = color that is to be interpolated
{                               //value = value of that color 	
    if(color==BLUE)
    {
        int* values = blueList.retNeighbours(color); //returns values in format x1,val1,x2,val2
        return interpolate(values[0],values[1],values[2],values[3],x);

    }
    else if(color==GREEN)
    {
        int* values = greenList.retNeighbours(color); //returns values in format x1,val1,x2,val2
        return interpolate(values[0],values[1],values[2],values[3],x);

    }
    else if(color==RED)
    {
        int* values = redList.retNeighbours(color); //returns values in format x1,val1,x2,val2
        return interpolate(values[0],values[1],values[2],values[3],x);

    }
}

void showResults()
{
    dstimg=cvCreateImage(cvSize(width,height),8,3);
    for(int i=0;i<height;i++)
    {
        for (int j=0;j<width;j++)
        {
            uchar ptr[3]; //= cvPtr2D(img, i, j, NULL);
            ptr[0] = img->imageData[i*img->widthStep + j*3];
            ptr[1] = img->imageData[i*img->widthStep + j*3 + 1];
            ptr[2] = img->imageData[i*img->widthStep + j*3 + 2];

            //CvScalar bgr=cvScalar((int)ptr[0],(int)ptr[1],(int)ptr[2]);
            //cvSet2D(finalImage,mi,mj,bgr);
            dstimg->imageData[i*img->widthStep+j*3]=mapColor(BLUE,ptr[0]);
            dstimg->imageData[i*img->widthStep+j*3+1]=mapColor(GREEN,ptr[1]);
            dstimg->imageData[i*img->widthStep+j*3+2]=mapColor(RED,ptr[2]);
        }
    }

    cvNamedWindow("RESULTS", CV_WINDOW_AUTOSIZE); 
    //cvMoveWindow("RESULTS", 260,260 );
    cvShowImage("RESULTS",dstimg);


    int hist_size = 256;
  float range[] = {0,256};
  float *ranges[] = {range};

  IplImage *red_img, *red_histogram;
  IplImage *green_img, *green_histogram;
  IplImage *blue_img, *blue_histogram;

  CvHistogram* hist_red = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  CvHistogram* hist_blue = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
  CvHistogram* hist_green = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

   // Initialize three images that will show each histogram
  CvSize hsize = cvSize(256,256);
  red_histogram = cvCreateImage( hsize, IPL_DEPTH_8U, 3 );
  green_histogram = cvCreateImage(hsize, IPL_DEPTH_8U, 3 );
  blue_histogram = cvCreateImage( hsize, IPL_DEPTH_8U, 3 );

  width=dstimg->width;
  height=dstimg->height;


  blue_img = cvCreateImage( cvGetSize(dstimg), IPL_DEPTH_8U, 1 );
  green_img = cvCreateImage( cvGetSize(dstimg), IPL_DEPTH_8U, 1 );
  red_img = cvCreateImage( cvGetSize(dstimg), IPL_DEPTH_8U, 1 );
  
  cvCvtPixToPlane( dstimg, blue_img, green_img, red_img, 0 );

  cvCalcHist( &red_img, hist_red, 0, NULL );
  cvCalcHist( &blue_img, hist_blue, 0, NULL );
  cvCalcHist( &green_img, hist_green, 0, NULL );

  CvScalar red=cvScalar(1.0,0.0,0.0);
  IplImage* imgHistRed = DrawHistogram(hist_red,1.0,1.0,red);
  cvNamedWindow("Result redHist", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("Result redHist", 660,260 );
  cvShowImage("Result redHist",imgHistRed);

  CvScalar green=cvScalar(0.0,1.0,0.0);
  IplImage* imgHistGreen = DrawHistogram(hist_green,1.0,1.0,green);
  cvNamedWindow("Result GreenHist", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("Result GreenHist", 660,360 );
  cvShowImage("Result GreenHist",imgHistGreen);

  CvScalar blue=cvScalar(0.0,0.0,1.0);
  IplImage* imgHistBlue = DrawHistogram(hist_blue,1.0,1.0,blue);
  cvNamedWindow("Result BlueHist", CV_WINDOW_AUTOSIZE); 
  cvMoveWindow("Result BlueHist", 660,460 );
  cvShowImage("Result BlueHist",imgHistBlue);

}


void reshape(int w, int h)
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100.0,1.0/*((GLdouble)w/(GLdouble)h)*/,1,5.0);
    //glOrtho(0.0,4.0,0.0,4.0,0.0,4.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0,0.0,2.0,0.0,0.0,0.0,0.0,1.0,0.0);
}

void initialiseTF()
{
    redList.append(0,0);
    redList.append(255,255);

    greenList.append(0,0);
    greenList.append(255,255);

    blueList.append(0,0);
    blueList.append(255,255);
}

void drawTFLine(linklist *list)
{
    int countVal = list->count();
    int *values = new int[countVal*2];
    values= list->retValues();
    //cout <<"\n count: "<< countVal ;
    int i=0;
    //glBegin
    for (i=0 ; i< (countVal)*2-2 ; i=i+2)
    {
        float x1 =values[i];
        float y1 = values[i+1];
        float x2 = values[i+2];
        float y2 =values [i+3];
        
        glBegin(GL_LINES);
            glVertex2f(x1,y1);
            glVertex2f(x2,y2);
        glEnd();
    //	cout<<"\n coordinates: "<<x1<<","<<y1<<" and "<<x2<<","<<y2;
    }
    free(values);
}
void displayTFEditor()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
        gluOrtho2D(0.0,glWinWidth,0.0,glWinHeight);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
            mouseY=glWinHeight-mouseY;
            if(mouseX<=255 && mouseY<=255)
            {
                //cout<<"\n mouseX: "<<mouseX<<" mouseY: "<<mouseY<<"\n";
            }
    
            glColor3f(1.0,0.0,0.0);
            drawTFLine(&redList);

            glColor3f(0.0,1.0,0.0);
            drawTFLine(&greenList);

            glColor3f(0.0,0.0,1.0);
            drawTFLine(&blueList);
        glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    //glPopMatrix();
}
void init()
{
    glClearColor(0.0,0.0,0.0,0.0);
    glShadeModel(GL_SMOOTH);
    initialiseTF();
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0,0.0,0.0);
//	glutWireCube(.1);
    displayTFEditor();
    glFlush();
    glutSwapBuffers();
}
void mouse(int button, int state, int x, int y)
{
    if( button == GLUT_LEFT_BUTTON)
    {
        mouseX=x;
        mouseY=y;
        if(mouseX<=255 && (glWinHeight-mouseY)<=255 )
        {
            if(red && !blue && !green)
            {
                int index=redList.seekIndex(mouseX);
                if(index==-1)
                    redList.modify(mouseX,(glWinHeight-mouseY));
                else
                {
                    redList.addafter((glWinHeight-mouseY),mouseX,index);
                }
            }
            else if(!red && blue && !green)
            {
                int index=blueList.seekIndex(mouseX);
                if(index==-1)
                    blueList.modify(mouseX,(glWinHeight-mouseY));
                else
                {
                    blueList.addafter((glWinHeight-mouseY),mouseX,index);
                }
            }
            else if(!red && !blue && green)
            {
                int index=greenList.seekIndex(mouseX);
                if(index==-1)
                    greenList.modify(mouseX,(glWinHeight-mouseY));
                else
                {
                    greenList.addafter((glWinHeight-mouseY),mouseX,index);
                }
            }
        showResults();
            
        }
    }
}
void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 'R':
        red=true;
        green=false;
        blue=false;
        cout<<"Red line Activated !!!!\n";
        break;
    case 'r':
        red=true;
        green=false;
        blue=false;
        cout<<"Red line Activated !!!!\n";
        break;
    case 'G':
        red=false;
        green=true;
        blue=false;
        cout<<"Green line Activated !!!!\n";
        break;
    case 'g':
        red=false;
        green=true;
        blue=false;
        cout<<"Green line Activated !!!!\n";
        break;
    case 'B':
        red=false;
        green=false;
        blue=true;
        cout<<"Blue line Activated !!!!\n";
        break;
    case 'b':
        red=false;
        green=false;
        blue=true;
        cout<<"Blue line Activated !!!!\n";
        break;
    case 's':
    case 'S':
        redList.del();
        greenList.del();
        blueList.del();
        cout<<"\nTransfer Function Reset ...!!!\n";
        initialiseTF();
        break;
    default:
        break;
    }
}



int main(int argc,char **argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(glWinWidth,glWinHeight);
    glutInitWindowPosition(10,10);
    main_window=glutCreateWindow(argv[0]);

    init();
    initOpenCV();
    showResults();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutIdleFunc(display);
    glutMainLoop();

    // wait for a key
    cvWaitKey(0);
    return 0;
}

