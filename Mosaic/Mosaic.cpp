#include <iostream>
#include <windows.h>
#include <winbase.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <math.h>

#define resizeWidth 50
#define resizeHeight 50
#define noOfImages 120
#define slavePath "C:\\Users\\Abhishek Bansal\\Documents\\sol compressed\\"
#define masterPath "C:\\Users\\Abhishek Bansal\\Documents\\sol\\masterImage.bmp"

using namespace std;

class imageDetail
{
public:
    char *imageName;
    int averageintensity;
};


int returnAverageIntensity(char *fileName)
{
    char path[200]=slavePath;
    int averageIntensity=0;
    const char* image=fileName;
    strncat(path,image,strnlen(image,100));
    //cout<<path<<"in func"<<endl;

    //now we start opencv stuff
    IplImage* img = 0; 
    img=cvLoadImage(path);
    if(!img)
        cout<<"cannot load image: "<<path<<endl;
    int w=resizeWidth;
    int h=resizeHeight;
    IplImage* resizedImage=cvCreateImage(cvSize(w,h),8,3);
    cvResize(img,resizedImage,CV_INTER_LINEAR);

    IplImage* greyImage = cvCreateImage(cvSize(resizedImage->width,resizedImage->height),IPL_DEPTH_8U,1);
    cvCvtColor(resizedImage, greyImage, CV_RGB2GRAY);
    if(!greyImage)
        cout<<"cannot convert to grey image is : "<<path<<endl;

    cvReleaseImage(&img);
    cvReleaseImage(&resizedImage);

    
    long int avg=0;

    for(int i=0;i<greyImage->height;i++)
    {
        for(int j=0;j<greyImage->width;j++)
        {
            unsigned char intensity = greyImage->imageData[i*greyImage->widthStep + j];
            avg=avg+intensity;
        }
    }

    int average= avg/(greyImage->height*greyImage->width);
    cvReleaseImage(&greyImage );

    return average;

}

char * wcharToChar(wchar_t *wchar)
{
    char*   nPtr = new char [lstrlen( wchar ) + 1];
    for( int i = 0; i < lstrlen( wchar ); i++ )
        nPtr[i] = char( wchar[i] );
    nPtr[lstrlen( wchar )] = '\0';
    return nPtr;
}
int lookUp(int intensity, imageDetail *images1)
{
    int index=0,diff=0,temp=300;
    int n=noOfImages;
    for(int i=0;i<n;i++)
    {
    //	cout<<endl<<images1[i].averageintensity;
        if (intensity==images1[i].averageintensity)
            return i;
        else
        {
            diff=abs(intensity-images1[i].averageintensity);
            if(diff<temp)
            {
                index=i;
                temp=diff;
            }
        }
    }

    return index;
}

int main()
{
    imageDetail images[noOfImages];
    int j=0;
    WIN32_FIND_DATA data;
    char fullPath[200];
    strncpy(fullPath,slavePath,strnlen(slavePath,200));
    fullPath[strnlen(slavePath,200)]='\0';
    strncat(fullPath,"*.*",strnlen("*.*",100));
    
    wchar_t* fp = new wchar_t [strnlen(fullPath,200)+1];
    for(int k =0; k<strnlen(fullPath,200); k++)
        fp[k] = fullPath[k];
    fp[strnlen(fullPath,200)]='\0';

    HANDLE h = FindFirstFile(fp,&data);
    
    if( h!=INVALID_HANDLE_VALUE ) 
    {
        do
        {
            char*   nPtr = wcharToChar(data.cFileName);
            
            //lstrcpy(
            if(strncmp(".",nPtr,sizeof(nPtr))!=0 && strncmp("..",nPtr,sizeof(nPtr)!=0))
            {
                images[j].averageintensity=returnAverageIntensity(nPtr);
                images[j].imageName=wcharToChar(data.cFileName);
            //	cout<<images[j].imageName;
                j++;
                //char*   Ptr = (images[j-1].imageName);
                //cout << ">>"<<Ptr ;
            }
        } while(FindNextFile(h,&data));
    } 
    else 
        cout << "Error: No such folder." << endl;
    FindClose(h);
    cout<<endl<<"average intensities calculated...";
    //now sort the given array
    for (int i=0; i<noOfImages; i++)
    {
        for (int j=i+1; j<noOfImages;j++)
        {
            if(images[i].averageintensity > images[j].averageintensity)
            {
                imageDetail temp;
                temp=images[j];
                images[j]=images[i];
                images[i]=temp;
            }			
        }
        char *temp = (images[i].imageName);
//		cout<<endl<<"after sorting: \n"<<i<<" "<<temp;

    }
    cout<<endl<<"Images Sorted according to intensity...";
    //now we take the master image

    IplImage * masterImage=cvLoadImage(masterPath);
    if(!masterImage)
    {
        cout<<"masterImage cannot be loaded!!! ";
        exit(0);
    }
    cout<<endl<<"Master Image loaded successfully....!!";
    
    IplImage *masterGreyImage= cvCreateImage(cvSize(masterImage->width,masterImage->height),IPL_DEPTH_8U,1);
    cvCvtColor( masterImage, masterGreyImage, CV_RGB2GRAY );
    //cvReleaseImage(&masterImage);

    //create image to display (final image)
    int w1=resizeWidth;
    int h1=resizeHeight;
    IplImage* finalImage=cvCreateImage(cvSize(w1*masterImage->width,h1*masterImage->height),8,3);
    if(!finalImage)
    {
        cout<<"\ncannot create final Image!!!!";
        exit(1);
    }
    cout<<endl<<"Final Image created Successfully in memory......!!";

//	cvReleaseImage(&masterImage);     //colored version of master image no longer needed

    int mi=0,mj=0;						// variables for final image

    for(int i=0;i<masterGreyImage->height;i++)
    {		
        for(int j=0;j<masterGreyImage->width;j++)
        {
            char path[200]=slavePath;
            unsigned char intensity = masterGreyImage->imageData[i*masterGreyImage->widthStep + j];
        //	cout<<"masterImage pixel" <<j<<" "<<i<<" "<<intensity<<endl;
            
            int slaveImage=lookUp(intensity,images) ;
            char* temp = images[slaveImage].imageName;
            strncat(path,temp,strnlen(temp,100));
            IplImage* slImage =cvLoadImage(path);

            int w=resizeWidth;
            int h=resizeHeight;
            IplImage* resizedImage=cvCreateImage(cvSize(w,h),8,3);
            cvResize(slImage,resizedImage,CV_INTER_LINEAR);
        //	cvReleaseImage(slImage);

            mj=resizedImage->width*j;
            mi=resizedImage->height*i;
            for(int p=0;p<resizedImage->height;p++)
            {
                for(int q=0; q<resizedImage->width; q++)
                {
                    //finalImage->imageData[mi*finalImage->widthStep + mj] = resizedImage->imageData[ p*resizedImage->widthStep + q ];
                    uchar *ptr = cvPtr2D(resizedImage, p, q, NULL);
                /*	CvScalar bgr=cvScalar((int)ptr[0],(int)ptr[1],(int)ptr[2]);
                    cvSet2D(finalImage,mi,mj,bgr);*/
                    finalImage->imageData[mi*finalImage->widthStep + mj*3] = ptr[0];
                    finalImage->imageData[mi*finalImage->widthStep + mj*3+1] = ptr[1];
                    finalImage->imageData[mi*finalImage->widthStep + mj*3+2] = ptr[2];
                    mj++;
                }
                mi++;
                mj=resizedImage->width*j;
            }	
        //	cvSaveImage("final.jpeg",finalImage);
        }
        cout<<endl<<"row "<<mi<<" of final image created in memory...";
            //cvSaveImage("final.jpeg",finalImage);
    }
    cout<<endl<<"Finally attempting to save Image to disk...!!!";
    if(!cvSaveImage("final.jpeg",finalImage))
    {
        cout<<endl<<"oops !!! something went wrong !! cannot save image....exiting now!!!!";
        exit(2);
    }
    cout<<endl<<"Everything gone fine final image saved !!!";
    return 0;
}