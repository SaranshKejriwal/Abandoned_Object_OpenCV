#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <conio.h>
#include <math.h>

int main() 
{
	using namespace cv;
	using namespace std;
  Mat frame;//to get primary frame
  Mat back;//to get background exclusively 
  Mat backz;//to get timed instances of background
  Mat fore;//to get foreground contours
  Mat oth;//aimed to obtain only foreground objects
  Mat sub;//difference between back and backz
//VideoCapture cap1(1);/*to capture from camera*/
VideoCapture cap1("crowd-plaza.avi");/*to capture from saved video*/
  //Background Subtraction Part_____________________________________________________________
 BackgroundSubtractorMOG2 bg;//works on GMM
  bg.set ("nmixtures", 10);
  vector < vector < Point > >contours;
 //cap1.set(CV_CAP_PROP_FPS, 0.1);
  namedWindow ("Background");
 namedWindow ("Frame");
 namedWindow( "Rectangle");
 cvMoveWindow("Rectangle",600,190);//to relocate the window to desired coordinates
 cvMoveWindow("Background",0,250);
 cvMoveWindow("Frame",0,0);//to relocate the "Frame" window to desired coordinates
   int i,j,k=0;
	int flag=0;
 double t1=getTickCount()/getTickFrequency();//for velocity
 double t = getTickCount()/getTickFrequency();//for abandoned object
  for (;;)
    {
		cap1 >> frame;
      bg.operator()(frame, fore);
      bg.getBackgroundImage (back);
      erode (fore, fore, cv::Mat ());//to remove image noise
     //erode (fore, fore, cv::Mat ());//to remove image noise more, but reduces contour size
     dilate (fore, fore, cv::Mat ());// perhaps to sharpen contour without noise,gives more square contour
     dilate (fore, fore, cv::Mat ());// perhaps to sharpen contour without noise,gives more square contour
     dilate (fore, fore, cv::Mat ());// perhaps to sharpen contour without noise
     findContours (fore, contours, CV_RETR_EXTERNAL,
     CV_CHAIN_APPROX_NONE);//CHAIN_APPROX_NONE ensures that straight lines of contour are also included
     for(i=0;i<contours.size();i++)
	 {
		 if(contourArea(contours[i])>500)//to eliminate noise contours
	 drawContours (frame, contours, -1, cv::Scalar (255, 255, 255), 1);
	 }
	 //to declare config of the contour B,G,R,thickness
	 Mat out=frame.clone(); //should we need more processing
	  
	 /*Rect r(10, 200, 600, 200);//x-origin,y-origin,column size, row size
    Mat small = out(r);//Print Same window with given ROI
	imshow("test",small);*/
	//Mat result;
	//absdiff(result,result,result);
	/*out.copyTo( result( Rect(0, 0, 400,400 ) ) );
    back.copyTo( result( Rect(620, 0, 400, 400) ) );
	imshow ("Test",result);*/
	
	 //imshow ("Background", back);
	 //imshow ("Frame", out);
	 
	 //Abandoned Object Part_____________________________________________________________     
	 int interval;
	 createTrackbar("Interval","Abandoned Objects",&interval,600);//max interval of 10 minutes
      
	 if(flag==0)
	 {
	 absdiff(back,back,backz);
	 interval=60; //to initialize trackbar position once at 60 seconds
	 flag=10 ;
	 }
	 
	 if(flag==10 && (getTickCount()/getTickFrequency()-t)>=2 )//this loop runs once
	 {
	 backz=back.clone();//extra loop run to ensure stable initial background
	 flag =20;
	 }
	 //cout<<"Difference = "<<getTickCount()/getTickFrequency()-t<<"\n" ;//to get count
	 //cout<<"\t___Difference = "<<getTickCount()/getTickFrequency()-t1<<"\n" ;//to get count
	 
	 if((getTickCount()/getTickFrequency()-t)>=interval )//interval can vary from 0 to 10 minutes; infinite loop
		{
			cout<<"New Interval \n";
		backz=back.clone();//not backz=back as they'll become pointers pointing to same address
	    t = getTickCount()/(getTickFrequency());
	    }
		if((getTickCount()/getTickFrequency()-t)>=interval-3 )//interval can vary from 0 to 10 minutes; infinite loop
		{
		cout<<"\a";//to produce a sound near new interval
		}
		absdiff(back,backz,sub);
	  threshold(sub,sub,35,255,THRESH_BINARY);
		//trying to establish area/size of abandoned object
	/*vector < vector < Point > >contours2;
 	 findContours (sub, contours2, CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point(0,0));
     Scalar color = Scalar( 100, 100, 100);
	   /* if(boundRect2[i].width>=40 || boundRect2[i].height>=40)
		color=Scalar(100,255,50);//green
		if(boundRect2[i].height/boundRect2[i].width<=1.5 && boundRect2[i].width>=60 && boundRect2[i].height>=60)//to color code contours
		  color=Scalar(0,0,255);//red
         drawContours( sub, contours2, -1, color, 1);*/
     //imshow("Abandoned Objects",sub);
	cvMoveWindow("Abandoned Objects",600,0);//to relocate the window to desired coordinates
 
	if (cv::waitKey(3)>=10)
     break;
	//getch();
	//Rectangles Part________________________________________________
	    Mat drawing = Mat::zeros( fore.size(), CV_8UC3 );//to initialize drawing to a blank matrix
	    drawing=out.clone();
		Scalar color = Scalar( 100, 100, 100);
	    int a=0;
	 	vector<Rect> boundRect( contours.size() );
		vector<RotatedRect> rotellipse( contours.size() );
          vector<Point> center( boundRect.size() );
        
 	  for( int i = 0; i < contours.size(); i++ )
	  {
		  
    boundRect[i] = boundingRect( contours[i] );//draws upright rectangles
	rotellipse[i] = fitEllipse(contours[i]);//draws free rotating ellipses
			
			//color coding and rect centres part____________________________________________________________________________________
  	   if(boundRect[i].width>=70 || boundRect[i].height>=70)//eliminates small boxes
	   {
		center[i] = Point((boundRect[i].x+boundRect[i].width/2),boundRect[i].y+boundRect[i].height/2);
		circle(drawing,center[i],4, Scalar(0,255,255),2,8);
		color=Scalar(200,200,200);
		//5 is radius, 2 is thickness, 8 means that point should be 8-connected
		if(boundRect[i].height/boundRect[i].width>1.3)
		color=Scalar(0,255,50);//green
		if(boundRect[i].height/boundRect[i].width<1.3)//to color code contours
		  color=Scalar(0,0,255);//red
		//if(rotellipse[i].size.height>=20 || rotellipse[i].size.width>=20)//can be used this way for speed and size
		//determination
		//ellipse height is major axis and width is minor axis
			//if (i%2==1)
			//color=Scalar(255,255,0);//to check random allocation of contour number
		
		//Tampering Part______________________________________________________________________
		a=a+(boundRect[i].height)*(boundRect[i].width);
	   }
	   if(a>=int(frame.rows)*int(frame.cols)/1.2)
	   {
	putText(drawing,"Tampering",Point(5,30),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,255),2);
	putText(out,"Tampering",Point(5,30),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,255),2);
	putText(sub,"Tampering",Point(5,30),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,255),2);
		   //1 is font-size and 2 is line thickness
	   }
//Note: Color code parameters vary with scale of a person on camera 
         drawContours( drawing, contours, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
         rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 1,0);//0 decides that the rectangle superimposes contour
	     ellipse( drawing, rotellipse[i], color,1,0);
		  }


//imshow ("Background", back);
imshow ("Frame", out);
	 
imshow("Abandoned Objects",sub);
imshow( "Rectangle", drawing );

}//end of infinite loop
return 1;
}//end of main
/*________________________________________
	 
	/*Limitations : Doesn't fully work when something  hides behind background, 
camouflaged foreground is not detected*/
