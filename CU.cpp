//
//  main.cpp
//  Control Unit
//
//  Created by Aya Harb on 4/16/16.
//  /Users/ayass3r/Documents/Control Unit/Control Unit/main.cppCopyright © 2016 Aya Harb. All rights reserved.
//

#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <mutex>
#include "Color.h"
#include <ostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;


/**********************************************/
/*             Initializations               */
/*********************************************/

/**********************************************/
/*           Path & Conflicts Struct          */
/*********************************************/
struct Path{
    char Source;
    char Destination;
    char PathName;
    char Conflicts[6];
};

/**********************************************/
/*                 TPVPP  Struct             */
/*********************************************/
struct Timing{
    char PathName;
    char CarType;
    float Duration;
};

/**********************************************/
/*            System Dynamics  Struct        */
/*********************************************/
struct TimingDelay{
    time_t settime;
    char PathName;
    float WaitTime;
};

/**********************************************/
/*               Vehicle  Struct             */
/*********************************************/
struct Car{
    char PathName;
    bool Priority = false;
    char Conflicts[6];
    float Duration;
    float WaitTime;
    float CarType;
};

/**********************************************/
/*              Declarations                 */
/*********************************************/
char Source = '2';
char Destination = '1';
char CharType = 'C';


char OldSource = 'Y';
char OldDestination = 'Y';
char OldCharType = 'Y';


Car Vehicles[10];
Car HighPriority[3];

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

Path Paths[12];
Timing Timings[48];
TimingDelay TimingDelays[12];

int IncomingIndex = 0;
int HPIncomingIndex = 0;

int HPActiveIndex = 0;
int ActiveIndex = 0;

int HPSkippedIndex = 0;
int SkippedIndex = 0;

struct parameters
{
    char S,D,CT;
};


typedef struct parameters struct1;


//initial min and max HSV filter values.
//these will be changed using trackbars
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;
int XRef,X1,X2,X3,X4;
int YRef,Y1,Y2,Y3,Y4;
int X1c, X2c, X3c, X4c, X5c, X6c, X7c, X8c;
int Y1c, Y2c, Y3c, Y4c, Y5c, Y6c, Y7c, Y8c;
int counter = 1;
//int Lane = 0;
//char myColor;
//bool critical = false;
//default capture width and height
const int FRAME_WIDTH = 800;
const int FRAME_HEIGHT = 800;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS=50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20*20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH/1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";


/**********************************************/
/*           Time Management                  */
/*********************************************/
//This sets the wait times and updates the queues
void TimeManagement(Car NewCar, char CarType) {
    time_t nowtime;
    for (int i = 0; i<6; i++) {
        for (int j = 0; j<12; j++) {
            if (NewCar.Conflicts[i] == TimingDelays[j].PathName) {
                TimingDelays[j].WaitTime = TimingDelays[j].WaitTime - (time(&TimingDelays[j].settime) - time(&nowtime));
                TimingDelays[j].settime = time(&nowtime);
                for (int k = 0; k<48; k++) {
                    if (NewCar.PathName == Timings[k].PathName) {
                        if (CarType == Timings[k].CarType)
                            TimingDelays[j].WaitTime += Timings[k].Duration;
                    }
                }
            }
        }
    }
    if (NewCar.Priority != true){
        Vehicles[IncomingIndex] = NewCar;
        IncomingIndex ++;
    }
    else{
        HighPriority[HPIncomingIndex] = NewCar;
        HPIncomingIndex ++;
    }
}

/**********************************************/
/*             Incoming                      */
/*********************************************/
//Registers every new car into the appropriate queue
void *Incoming(void* parameters){ //char Source, char Destination, char CarType)

    char Source ,
    Destination,
    CarType;

    struct1 *params;
    params = (struct1*)parameters;


    Source = params->S;
    Destination = params->D;
    CarType = params->CT;

    Car NewCar;

    NewCar.CarType = CarType;

    for (int i = 0; i <12; i++) {
        if (Source == Paths[i].Source && Destination == Paths[i].Destination)
        { NewCar.PathName = Paths[i].PathName;
            NewCar.Conflicts[0] = Paths[i].Conflicts[0];
            NewCar.Conflicts[1] = Paths[i].Conflicts[1];
            NewCar.Conflicts[2] = Paths[i].Conflicts[2];
            NewCar.Conflicts[3] = Paths[i].Conflicts[3];
            NewCar.Conflicts[4] = Paths[i].Conflicts[4];
            NewCar.Conflicts[5] = Paths[i].Conflicts[5];
        }
    }
    for (int i = 0; i <48; i++) {
        if (NewCar.PathName == Timings[i].PathName && CarType == Timings[i].CarType)
            NewCar.Duration = Timings[i].Duration;
    }
    if (CarType == '1')
        NewCar.Priority = true;
    NewCar.WaitTime = 0;

    TimeManagement(NewCar, CarType);
    return 0;
}

/**********************************************/
/*                DRIVING CARS               */
/*********************************************/
//CAR DRIVEN
void DriveNOW(char CarType, char PathName){
    //SWITCH ON DRIVING DIFFERENT CARS
    ofstream outfile;
    if(CharType  == 'D'){
        outfile.open("/home/hagar13/Desktop/commanddagu.txt", std::ofstream::out | std::ofstream::trunc);
        if(PathName=='A' || PathName == 'G' || PathName == 'C' || PathName == 'E'){
            outfile<<"3 0xD5 0 20";
            cout<<"WritingMoveDaguRight"<<endl;
        }
        else if(PathName=='H' || PathName == 'B' || PathName == 'D' || PathName == 'F'){
            outfile<<"3 0xD5 20 0";
            cout<<"WritingMoveDaguLeft"<<endl;
        }
        else if(PathName=='I' || PathName == 'J' || PathName == 'K' || PathName == 'L'){
            outfile<<"3 0xD5 20 20";
            cout<<"WritingMoveDaguStraight"<<endl;
        }
    }
    else if(CharType == 'S'){
        outfile.open("/home/hagar13/Desktop/commandsumo.txt", std::ofstream::out | std::ofstream::trunc);
        if(PathName=='A' || PathName == 'G' || PathName == 'C' || PathName == 'E'){
            outfile<<"10 10";
            cout<<"WritingMoveSumoRight"<<endl;
        }
        else if(PathName=='H' || PathName == 'B' || PathName == 'D' || PathName == 'F'){
            outfile<<"10 -10";
            cout<<"WritingMoveSumoLeft"<<endl;
        }
        else if(PathName=='I' || PathName == 'J' || PathName == 'K' || PathName == 'L'){
            outfile<<"20 0";
            cout<<"WritingMoveSumoStraight"<<endl;
        }
    }
    else if(CharType  == 'I'){
        outfile.open("/home/hagar13/Desktop/commandcreate.txt" , std::ofstream::out | std::ofstream::trunc);
        if(PathName=='A' || PathName == 'G' || PathName == 'C' || PathName == 'E'){
            outfile<<"4";
            cout<<"WritingMoveIcreateRight"<<endl;
        }
        else if(PathName=='H' || PathName == 'B' || PathName == 'D' || PathName == 'F'){
            outfile<<"5";
            cout<<"WritingMoveIcreateLeft"<<endl;
        }
        else if(PathName=='I' || PathName == 'J' || PathName == 'K' || PathName == 'L'){
            outfile<<"2";
            cout<<"WritingMoveIcreateStraight"<<endl;
        }
    }
    else if(CharType  == 'C'){
        outfile.open("/home/hagar13/Desktop/commandcreate2.txt", std::ofstream::out | std::ofstream::trunc);
        if(PathName=='A' || PathName == 'G' || PathName == 'C' || PathName == 'E'){
            outfile<<"4";
            cout<<"WritingMoveIcreate2Right"<<endl;
        }
        else if(PathName=='H' || PathName == 'B' || PathName == 'D' || PathName == 'F'){
            outfile<<"5";
            cout<<"WritingMoveIcreate2Left"<<endl;
        }
        else if(PathName=='I' || PathName == 'J' || PathName == 'K' || PathName == 'L'){
            outfile<<"2";
            cout<<"WritingMoveIcreate2Straight"<<endl;
        }
    }
    else {
        //cout<<"Wrong Vehicle!"<<endl;
    }

}

//Small Rotation = a,g,c,e   // Right
//Big Rotation = h, b, d, f   // Left
//Straight = I, J, K, L
/**********************************************/
/*                Traffic Man                 */
/*********************************************/
//Decides which car is due to go
void Drive2(){
    cout<<"Drive2"<<'\n';
     pthread_mutex_lock(&mtx);
    Car NowCar;
    time_t nowtime;
    while (1){//!Vehicles.empty() || !HighPriority.empty())
        if (HighPriority[0].PathName == 'Z' && HighPriority[1].PathName == 'Z' && HighPriority[2].PathName == 'Z') {
            if (HPSkippedIndex > 0)
                HPActiveIndex = HPActiveIndex - 1;
            NowCar = HighPriority[(HPActiveIndex%10)];
            for (int j = 0; j<12; j++) {
                if (NowCar.PathName == TimingDelays[j].PathName) {
                    NowCar.WaitTime = TimingDelays[j].WaitTime - (time(&nowtime) - time(&TimingDelays[j].settime));
                }
            }
            if (NowCar.WaitTime <= 0){
                (NowCar.CarType, NowCar.PathName);
                pthread_mutex_unlock(&mtx);
            }else{
                HPActiveIndex ++;
                HPSkippedIndex ++;
                return Drive2();
            }
            HighPriority[(HPActiveIndex%10)].PathName = 'Z';
            HPActiveIndex ++;
        }
        else {
            if (SkippedIndex > 0)
                ActiveIndex = ActiveIndex - 1;
            NowCar = Vehicles[(ActiveIndex%10)];
            for (int j = 0; j<12; j++) {
                if (NowCar.PathName == TimingDelays[j].PathName) {
                    NowCar.WaitTime = TimingDelays[j].WaitTime - (time(&nowtime) - time(&TimingDelays[j].settime));
                }
            }
            if (NowCar.WaitTime <= 0){
                DriveNOW(NowCar.CarType, NowCar.PathName);
                pthread_mutex_unlock(&mtx);
            }
            else{
                ActiveIndex ++;
                SkippedIndex ++;
                return Drive2();
            }
            Vehicles[(ActiveIndex%10)].PathName = 'Z';
            ActiveIndex ++;
        }
    }
    pthread_mutex_unlock(&mtx);
}

void *Drive(void *arg){
    cout<<"Drive"<<'\n';
pthread_mutex_lock(&mtx);
Car NowCar;
time_t nowtime;
while (1){//!Vehicles.empty() || !HighPriority.empty())
    if (HighPriority[0].PathName == 'Z' && HighPriority[1].PathName == 'Z' && HighPriority[2].PathName == 'Z') {
        if (HPSkippedIndex > 0)
            HPActiveIndex = HPActiveIndex - 1;
        NowCar = HighPriority[(HPActiveIndex%10)];
        for (int j = 0; j<12; j++) {
            if (NowCar.PathName == TimingDelays[j].PathName) {
                NowCar.WaitTime = TimingDelays[j].WaitTime - (time(&nowtime) - time(&TimingDelays[j].settime));
            }
        }
        if (NowCar.WaitTime <= 0){
            DriveNOW(NowCar.CarType, NowCar.PathName);
            pthread_mutex_unlock(&mtx);
        }else{
            HPActiveIndex ++;
            HPSkippedIndex ++;
            Drive2();
        }
        HighPriority[(HPActiveIndex%10)].PathName = 'Z';
        HPActiveIndex ++;
    }
    else {
        if (SkippedIndex > 0)
            ActiveIndex = ActiveIndex - 1;
        NowCar = Vehicles[(ActiveIndex%10)];
        for (int j = 0; j<12; j++) {
            if (NowCar.PathName == TimingDelays[j].PathName) {
                NowCar.WaitTime = TimingDelays[j].WaitTime - (time(&nowtime) - time(&TimingDelays[j].settime));
            }
        }
        if (NowCar.WaitTime <= 0){
            DriveNOW(NowCar.CarType, NowCar.PathName);
            pthread_mutex_unlock(&mtx);
        }
        else{
            ActiveIndex ++;
            SkippedIndex ++;
            Drive2();
        }
        Vehicles[(ActiveIndex%10)].PathName = 'Z';
        ActiveIndex ++;
    }
}
pthread_mutex_unlock(&mtx);
}

\
/*void *Tracking(void *arg)//int argc, char* argv[])
{
    return 0;
} */


/**********************************************/
/*             Tracking                      */
/*********************************************/
void on_trackbar( int, void* )
{//This function gets called whenever a
    // trackbar position is changed





}
string intToString(int number){


    std::stringstream ss;
    ss << number;
    return ss.str();
}
void createTrackbars(){
    //create window for trackbars


    namedWindow(trackbarWindowName,0);
    //create memory to store trackbar name on window
    char TrackbarName[50];
    sprintf( TrackbarName, "H_MIN", H_MIN);
    sprintf( TrackbarName, "H_MAX", H_MAX);
    sprintf( TrackbarName, "S_MIN", S_MIN);
    sprintf( TrackbarName, "S_MAX", S_MAX);
    sprintf( TrackbarName, "V_MIN", V_MIN);
    sprintf( TrackbarName, "V_MAX", V_MAX);
    //create trackbars and insert them into window
    //3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
    //the max value the trackbar can move (eg. H_HIGH),
    //and the function that is called whenever the trackbar is moved(eg. on_trackbar)
    //                                  ---->    ---->     ---->
    createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );

}
void drawObject(vector<Color> theObjects,Mat &frame){

    for(int i =0; i<theObjects.size(); i++){
        cv::circle(frame,cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()),10,cv::Scalar(0,0,255));
        cv::putText(frame,intToString(theObjects.at(i).getXPos())+ " , " + intToString(theObjects.at(i).getYPos()),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()+20),1,1,Scalar(0,255,0));
        cv::putText(frame,theObjects.at(i).getType(),cv::Point(theObjects.at(i).getXPos(),theObjects.at(i).getYPos()-30),1,2,theObjects.at(i).getColour());
    }}

void morphOps(Mat &thresh){

    //create structuring element that will be used to "dilate" and "erode" image.
    //the element chosen here is a 3px by 3px rectangle

    Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
    Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

    erode(thresh,thresh,erodeElement);
    erode(thresh,thresh,erodeElement);


    dilate(thresh,thresh,dilateElement);
    dilate(thresh,thresh,dilateElement);



}
void trackFilteredObject(Mat threshold,Mat HSV, Mat &cameraFeed){
    vector <Color> Objects;
    Mat temp;
    threshold.copyTo(temp);
    //these two vectors needed for output of findContours
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    //use moments method to find our filtered object
    double refArea = 0;
    bool objectFound = false;
    if (hierarchy.size() > 0) {
        int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
            for (int index = 0; index >= 0; index = hierarchy[index][0]) {

                Moments moment = moments((cv::Mat)contours[index]);
                double area = moment.m00;

                //if the area is less than 20 px by 20px then it is probably just noise
                //if the area is the same as the 3/2 of the image size, probably just a bad filter
                //we only want the object with the largest area so we safe a reference area each
                //iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA){

                    Color Purple;

                    Purple.setXPos(moment.m10/area);
                    Purple.setYPos(moment.m01/area);
                    Objects.push_back(Purple);
                    objectFound = true;
                }else objectFound = false;


            }
            //let user know you found an object
            if(objectFound ==true){
                //draw object location on screen
                drawObject(Objects,cameraFeed);


            }


        }else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
    }
}
Color trackFilteredObject(Color theColor,Mat threshold,Mat HSV, Mat &cameraFeed){

    vector <Color> Objects;
    Color color;
    Mat temp;
    threshold.copyTo(temp);
    //these two vectors needed for output of findContours
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //find contours of filtered image using openCV findContours function
    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    //use moments method to find our filtered object
    double refArea = 0;
    bool objectFound = false;
    if (hierarchy.size() > 0) {
        int numObjects = hierarchy.size();
        //if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS){
            for (int index = 0; index >= 0; index = hierarchy[index][0]) {

                Moments moment = moments((cv::Mat)contours[index]);
                double area = moment.m00;

                //if the area is less than 20 px by 20px then it is probably just noise
                //if the area is the same as the 3/2 of the image size, probably just a bad filter
                //we only want the object with the largest area so we safe a reference area each
                //iteration and compare it to the area in the next iteration.
                if(area>MIN_OBJECT_AREA){



                    color.setXPos(moment.m10/area);
                    color.setYPos(moment.m01/area);
                    color.setType(theColor.getType());
                    color.setColour(theColor.getColour());
                    Objects.push_back(color);
                    objectFound = true;

                    //Initialize the Lanes Boundries
                    if ((counter == 1) && (color.getType()=="Orange"))
                    {
                        XRef = color.getXPos();
                        YRef = color.getYPos();
                        X1 = XRef+100;
                        Y1 = YRef-100;

                        X2 = XRef-100;
                        Y2 = YRef-100;

                        X3 = XRef-100;
                        Y3 = YRef+100;

                        X4 = XRef+100;
                        Y4 = YRef+100;

                        X1c = XRef+200;
                        Y1c = YRef-100;

                        X2c = XRef+100;
                        Y2c = YRef-200;

                        X3c = XRef-100;
                        Y3c = YRef-200;

                        X4c = XRef-200;
                        Y4c = YRef-100;

                        X5c = XRef-200;
                        Y5c = YRef+100;

                        X6c = XRef-100;
                        Y6c = YRef+200;

                        X7c = XRef+100;
                        Y7c = YRef+200;

                        X8c = XRef+200;
                        Y8c = YRef+100;

                        counter ++;
                    }


                    // Checking the Lane.
                    if ((color.getXPos()>X1) && (Y1<color.getYPos()<Y3) )
                        color.setLane(1);
                    else if ((X2 <color.getXPos()<X1) && (color.getYPos()<= Y1) )
                        color.setLane(2);
                    else if ((color.getXPos()<=X3) && (Y1<color.getYPos()< Y3) )
                        color.setLane(3);
                    else if ((X3<color.getXPos()<X4) && (color.getYPos()>=Y3) )
                        color.setLane(4);
                    else
                        color.setLane(5);

                    //Checking the critical sections.

                    if ((color.getXPos()<=X1c) && (Y1c<=color.getYPos()<=Y8c) && (color.getLane() == 1))
                    {
                        color.setCritical(true);
                        if (color.getType() == "Green")
                            CharType  = 'C';
                        else if (color.getType() == "Purple")
                            CharType  = 'D';
                        else if (color.getType() == "Blue")
                            CharType  = 'I';
                        else if (color.getType() == "Pink")
                            CharType  = 'S';
                    }
                    else if ((X3c <=color.getXPos()<=X2c) && (color.getYPos()>= Y2c) && (color.getLane() == 2))
                    {
                        color.setCritical(true);
                        if (color.getType() == "Green")
                            CharType  = 'C';
                        else if (color.getType() == "Purple")
                            CharType  = 'D';
                        else if (color.getType() == "Blue")
                            CharType = 'I';
                        else if (color.getType() == "Pink")
                            CharType  = 'S';
                    }
                    else if ((color.getXPos()>=X4c) && (Y4c<color.getYPos()< Y5c) && (color.getLane() == 3))
                    {
                        color.setCritical(true);
                        if (color.getType() == "Green")
                            CharType  = 'C';
                        else if (color.getType() == "Purple")
                            CharType  = 'D';
                        else if (color.getType() == "Blue")
                            CharType  = 'I';
                        else if (color.getType() == "Pink")
                            CharType  = 'S';
                    }
                    else if ((X6c<=color.getXPos()<=X7c) && (color.getYPos()<=Y6c) && (color.getLane() == 4))
                    {
                        color.setCritical(true);
                        if (color.getType() == "Green")
                            CharType = 'C';
                        else if (color.getType() == "Purple")
                            CharType  = 'D';
                        else if (color.getType() == "Blue")
                            CharType  = 'I';
                        else if (color.getType() == "Pink")
                            CharType  = 'S';
                    }
                    else
                    {
                        color.setCritical(true);
                        if (color.getType() == "Green")
                            CharType  = 'C';
                        else if (color.getType() == "Purple")
                            CharType  = 'D';
                        else if (color.getType() == "Blue")
                            CharType  = 'I';
                        else if (color.getType() == "Pink")
                            CharType = 'S';
                    }

                    //Setting the Source and Destination
                   if (color.getType()=="Blue")
                   {
                       color.setSource(4);
                       color.setDestination(2);
                   }
                   else if (color.getType()=="Pink")
                   {
                       color.setSource(2);
                       color.setDestination(4);
                   }
                   else if (color.getType()=="Green")
                   {
                       color.setSource(3);
                       color.setDestination(1);
                   }
                   else if (color.getType()=="Green")
                   {
                       color.setSource(1);
                       color.setDestination(3);
                   }



                }else objectFound = false;
            }
            //let user know you found an object
            if(objectFound ==true){
                //draw object location on screen
                drawObject(Objects,cameraFeed);}

        }else putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
    }
    return color;
}

void *Tracking(void *arg)//int argc, char* argv[])
{	//if we would like to calibrate our filter values, set to true.
    bool calibrationMode = false;

    //Matrix to store each frame of the webcam feed
    Mat cameraFeed;
    Mat threshold;
    Mat HSV;
    //int counter = 1;
    if(calibrationMode){
        //create slider bars for HSV filtering
        createTrackbars();
    }
    //video capture object to acquire webcam feed
    VideoCapture capture;
    //open capture object at location zero (default location for webcam)
    capture.open(1);
    //set height and width of capture frame
    capture.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT);
    //start an infinite loop where webcam feed is copied to cameraFeed matrix
    //all of our operations will be performed within this loop
    while(1){
        //store image to matrix
        capture.read(cameraFeed);
        bool ok = capture.read(cameraFeed);
        if ( ! ok ) continue;   // 1st frame might be invalid

        /*************************************For checking if the camera is actually reading *////////////////////////////
        //namedWindow( "Display window", WINDOW_AUTOSIZE );
        //if(ok) imshow("Display window", capture.read(cameraFeed)); else cout <<"SHIT";


        //convert frame from BGR to HSV colorspace
        cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);

        if(calibrationMode==true){
            //if in calibration mode, we track objects based on the HSV slider values.
            cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
            inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
            morphOps(threshold);
            imshow(windowName2,threshold);
            trackFilteredObject(threshold,HSV,cameraFeed);
        }
        else{
            //create some temp Color objects so that
            //we can use their member functions/information
            Color Pink("Pink"), Blue("Blue"), Green("Green"), Purple("Purple"), Orange("Orange");

            //The reference point (Orange)
            //then Oranges
            cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
            inRange(HSV,Orange.getHSVmin(),Orange.getHSVmax(),threshold);
            morphOps(threshold);
            trackFilteredObject(Orange,threshold,HSV,cameraFeed);



            //first find purples
            cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
            inRange(HSV,Purple.getHSVmin(),Purple.getHSVmax(),threshold);
            morphOps(threshold);
            Purple = trackFilteredObject(Purple,threshold,HSV,cameraFeed);
            /*if (Purple.getCritical()==false)
                cout << "color: "<<Purple.getType() << " Lane: " <<Purple.getLane()  << " X position is: " << Purple.getXPos() << " Y position is: " << Purple.getYPos() << " and um ok!"<<endl;
            else
                cout << "color: "<<Purple.getType() << " Lane: " <<Purple.getLane()  << " X position is: " << Purple.getXPos() << " Y position is: " << Purple.getYPos() << " and um in critical sec!"<<endl;
*/
            //then yellows
            cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
            inRange(HSV,Pink.getHSVmin(),Pink.getHSVmax(),threshold);
            morphOps(threshold);
            Pink = trackFilteredObject(Pink,threshold,HSV,cameraFeed);
            /*
            if (Pink.getCritical()==false)
                cout << "color: "<<Pink.getType() << " Lane: " <<Pink.getLane()  << " X position is: " << Pink.getXPos() << " Y position is: " << Pink.getYPos()<< " and um ok!"<<endl;
            else
                cout << "color: "<<Pink.getType() << " Lane: " <<Pink.getLane()  << " X position is: " << Pink.getXPos() << " Y position is: " << Pink.getYPos()<< " and um in critical sec!"<<endl;
            */
            //then blues
            cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
            inRange(HSV,Blue.getHSVmin(),Blue.getHSVmax(),threshold);
            morphOps(threshold);
            Blue = trackFilteredObject(Blue,threshold,HSV,cameraFeed);
            /*if (Blue.getCritical()==false)
                cout << "color: "<<Blue.getType() << " Lane: " <<Blue.getLane() << " X position is: " << Blue.getXPos() << " Y position is: " <<  Blue.getYPos()<< " and um ok!"<<endl;
            else
                cout << "color: "<<Blue.getType() << " Lane: " <<Blue.getLane() << " X position is: " << Blue.getXPos() << " Y position is: " << Blue.getYPos()<< " and um in critical sec!"<<endl;
            */
            //then greens
            cvtColor(cameraFeed,HSV,COLOR_BGR2HSV);
            inRange(HSV,Green.getHSVmin(),Green.getHSVmax(),threshold);
            morphOps(threshold);
            Green = trackFilteredObject(Green,threshold,HSV,cameraFeed);
            /*if (Green.getCritical()==false)
                cout << "color: "<<Green.getType() << " Lane: " <<Green.getLane()  << " X position is: " << Green.getXPos() << " Y position is: " << Green.getYPos()<<" and um ok!"<<endl;
            else
                cout << "color: "<<Green.getType() << " Lane: " <<Green.getLane()  << " X position is: " << Green.getXPos() << " Y position is: " << Green.getYPos()<<" and um in critical sec!"<<endl;
            */

        }

        //AND MAKE THE OUTPUTS CarType, Source, and Destination

        //show frames
        //imshow(windowName2,threshold);

        imshow(windowName,cameraFeed);
        //imshow(windowName1,HSV);


        //delay 30ms so that screen can refresh.
        //image will not appear without this waitKey() command
        waitKey(30);
    }

    return 0;
}

/**********************************************/
/*              UpdateCheck                  */
/*********************************************/
//Starts the Updating thread
void *UpdateCheck(void *arg){
    pthread_t UpdateVehicle;

    struct1 *params;
    params=(struct1 *)malloc(sizeof(struct1));


  //  printf("Updated CharType: %p\n",&CharType);

    while(1){
      if ((CharType != OldCharType)){//removed s and d
        params->S = Source;
        params->D = Destination;
        params->CT = CharType;
        pthread_create (&UpdateVehicle, NULL, Incoming, (void*)params );
        OldSource = Source;
        OldDestination = Destination;
        OldCharType = CharType;
      }
    }
  }


/**********************************************/
/*                     MAIN                  */
/*********************************************/
//ALL THE THREADS

int main(){

    cout<<"Started"<<'\n';

    time_t nowtime;

    Paths[0].Source = '1';
    Paths[0].Destination = '3';
    Paths[0].PathName = 'L';
    Paths[0].Conflicts[0] = 'H';
    Paths[0].Conflicts[1] = 'K';
    Paths[0].Conflicts[2] = 'J';
    Paths[0].Conflicts[3] = 'F';
    Paths[0].Conflicts[4] = 'H';
    Paths[0].Conflicts[5] = 'I';

    Paths[1].Source = '2';
    Paths[1].Destination = '1';
    Paths[1].PathName = 'A';
    Paths[1].Conflicts[0] = 'H';
    Paths[1].Conflicts[1] = 'K';
    Paths[1].Conflicts[2] = 'Z';
    Paths[1].Conflicts[3] = 'Z';
    Paths[1].Conflicts[4] = 'Z';
    Paths[1].Conflicts[5] = 'Z';

    Paths[2].Source = '1';
    Paths[2].Destination = '2';
    Paths[2].PathName = 'B';
    Paths[2].Conflicts[0] = 'C';
    Paths[2].Conflicts[1] = 'I';
    Paths[2].Conflicts[2] = 'J';
    Paths[2].Conflicts[3] = 'H';
    Paths[2].Conflicts[4] = 'K';
    Paths[2].Conflicts[5] = 'D';

    Paths[3].Source = '3';
    Paths[3].Destination = '2';
    Paths[3].PathName = 'C';
    Paths[3].Conflicts[0] = 'I';
    Paths[3].Conflicts[1] = 'B';
    Paths[3].Conflicts[2] = 'Z';
    Paths[3].Conflicts[3] = 'Z';
    Paths[3].Conflicts[4] = 'Z';
    Paths[3].Conflicts[5] = 'Z';

    Paths[4].Source = '2';
    Paths[4].Destination = '3';
    Paths[4].PathName = 'D';
    Paths[4].Conflicts[0] = 'E';
    Paths[4].Conflicts[1] = 'L';
    Paths[4].Conflicts[2] = 'K';
    Paths[4].Conflicts[3] = 'B';
    Paths[4].Conflicts[4] = 'F';
    Paths[4].Conflicts[5] = 'I';

    Paths[5].Source = '4';
    Paths[5].Destination = '3';
    Paths[5].PathName = 'E';
    Paths[5].Conflicts[0] = 'I';
    Paths[5].Conflicts[1] = 'D';
    Paths[5].Conflicts[2] = 'Z';
    Paths[5].Conflicts[3] = 'Z';
    Paths[5].Conflicts[4] = 'Z';
    Paths[5].Conflicts[5] = 'Z';

    Paths[6].Source = '3';
    Paths[6].Destination = '4';
    Paths[6].PathName = 'F';
    Paths[6].Conflicts[0] = 'J';
    Paths[6].Conflicts[1] = 'G';
    Paths[6].Conflicts[2] = 'L';
    Paths[6].Conflicts[3] = 'K';
    Paths[6].Conflicts[4] = 'D';
    Paths[6].Conflicts[5] = 'B';

    Paths[7].Source = '1';
    Paths[7].Destination = '4';
    Paths[7].PathName = 'G';
    Paths[7].Conflicts[0] = 'F';
    Paths[7].Conflicts[1] = 'J';
    Paths[7].Conflicts[2] = 'Z';
    Paths[7].Conflicts[3] = 'Z';
    Paths[7].Conflicts[4] = 'Z';
    Paths[7].Conflicts[5] = 'Z';

    Paths[8].Source = '4';
    Paths[8].Destination = '1';
    Paths[8].PathName = 'H';
    Paths[8].Conflicts[0] = 'K';
    Paths[8].Conflicts[1] = 'A';
    Paths[8].Conflicts[2] = 'F';
    Paths[8].Conflicts[3] = 'L';
    Paths[8].Conflicts[4] = 'B';
    Paths[8].Conflicts[5] = 'J';

    Paths[9].Source = '4';
    Paths[9].Destination = '2';
    Paths[9].PathName = 'I';
    Paths[9].Conflicts[0] = 'C';
    Paths[9].Conflicts[1] = 'B';
    Paths[9].Conflicts[2] = 'L';
    Paths[9].Conflicts[3] = 'D';
    Paths[9].Conflicts[4] = 'F';
    Paths[9].Conflicts[5] = 'K';

    Paths[10].Source = '2';
    Paths[10].Destination = '4';
    Paths[10].PathName = 'J';
    Paths[10].Conflicts[0] = 'F';
    Paths[10].Conflicts[1] = 'G';
    Paths[10].Conflicts[2] = 'K';
    Paths[10].Conflicts[3] = 'H';
    Paths[10].Conflicts[4] = 'B';
    Paths[10].Conflicts[5] = 'L';

    Paths[11].Source = '3';
    Paths[11].Destination = '1';
    Paths[11].PathName = 'K';
    Paths[11].Conflicts[0] = 'H';
    Paths[11].Conflicts[1] = 'A';
    Paths[11].Conflicts[2] = 'B';
    Paths[11].Conflicts[3] = 'D';
    Paths[11].Conflicts[4] = 'J';
    Paths[11].Conflicts[5] = 'I';

    //Initialize to Current Time
    TimingDelays[0].settime = time(&nowtime);
    TimingDelays[0].PathName = 'A';
    TimingDelays[0].WaitTime = 0;

    TimingDelays[1].settime = time(&nowtime);
    TimingDelays[1].PathName = 'B';
    TimingDelays[1].WaitTime = 0;

    TimingDelays[2].settime = time(&nowtime);
    TimingDelays[2].PathName = 'C';
    TimingDelays[2].WaitTime = 0;

    TimingDelays[3].settime = time(&nowtime);
    TimingDelays[3].PathName = 'D';
    TimingDelays[3].WaitTime = 0;

    TimingDelays[4].settime = time(&nowtime);
    TimingDelays[4].PathName = 'E';
    TimingDelays[4].WaitTime = 0;

    TimingDelays[5].settime = time(&nowtime);
    TimingDelays[5].PathName = 'F';
    TimingDelays[5].WaitTime = 0;

    TimingDelays[6].settime = time(&nowtime);
    TimingDelays[6].PathName = 'G';
    TimingDelays[6].WaitTime = 0;

    TimingDelays[7].settime = time(&nowtime);
    TimingDelays[7].PathName = 'H';
    TimingDelays[7].WaitTime = 0;

    TimingDelays[8].settime = time(&nowtime);
    TimingDelays[8].PathName = 'I';
    TimingDelays[8].WaitTime = 0;

    TimingDelays[9].settime = time(&nowtime);
    TimingDelays[9].PathName = 'J';
    TimingDelays[9].WaitTime = 0;

    TimingDelays[10].settime = time(&nowtime);
    TimingDelays[10].PathName = 'K';
    TimingDelays[10].WaitTime = 0;

    TimingDelays[11].settime = time(&nowtime);
    TimingDelays[11].PathName = 'L';
    TimingDelays[11].WaitTime = 0;



    struct1 *params;
    params=(struct1 *)malloc(sizeof(struct1));
    params->S = Source;
    params->D = Destination;
    params->CT = CharType;

    pthread_t Driving, Updating,Track;
    pthread_create (&Track, NULL, &Tracking, NULL);
    pthread_create (&Driving, NULL, &Drive, NULL);
    pthread_create (&Updating, NULL, &UpdateCheck, NULL);

    pthread_join(Driving, NULL);
    pthread_join(Updating, NULL);
    pthread_join(Track, NULL);

    pthread_exit(NULL);

    return 0;

}
