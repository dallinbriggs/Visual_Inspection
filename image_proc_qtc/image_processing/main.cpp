#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{

    Mat image;
    Mat current_frame;
    Mat previous_frame;
    Mat frame_thresh;
    string filename;
    string header;
    string tail;

    namedWindow("Team 10",1);
    header = "/home/dallin/robotic_vision_ws/Visual_Inspection/image_proc_qtc/images/";
    tail = ".bmp";
    filename = header + "nilla_good" + tail;


    image = imread(filename,CV_LOAD_IMAGE_GRAYSCALE);
    GaussianBlur(image, image, Size(7,7), 1.5, 1.5);
    threshold(image, image,80,255,1);

    // Setup SimpleBlobDetector parameters.
    SimpleBlobDetector::Params params_good;
    SimpleBlobDetector::Params params_acceptable;
    SimpleBlobDetector::Params params_bad;

    // Change thresholds
    params_good.minThreshold = 10;
    params_good.maxThreshold = 255;
    params_acceptable.minThreshold = 10;
    params_acceptable.maxThreshold = 255;
    params_bad.minThreshold = 10;
    params_bad.maxThreshold = 255;

    // Filter by Area.
    params_good.filterByArea = true;
    params_good.minArea = 9000;
    params_good.maxArea = 11500;

    // Filter by Circularity
    params_good.filterByCircularity = true;
    params_good.minCircularity = .82;

    // Filter by Convexity
    params_good.filterByConvexity = true;
    params_good.minConvexity = .95;

    // Filter by Inertia
    params_good.filterByInertia = true;
    params_good.minInertiaRatio = .7;

#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

    // Set up detector with params
    SimpleBlobDetector detector(params);

    // You can use the detector this way
    // detector.detect( im, keypoints);

#else

    // Set up detector with params
    Ptr<SimpleBlobDetector> blobby = SimpleBlobDetector::create(params_good);

    // SimpleBlobDetector::create creates a smart pointer.
    // So you need to use arrow ( ->) instead of dot ( . )
    // detector->detect( im, keypoints);

#endif

    // Detect blobs.
    std::vector<KeyPoint> keypoints;
    blobby->detect(image, keypoints);

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    Mat im_with_keypoints;
    drawKeypoints(image, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

    cout << keypoints.size() << endl;

    imshow("Good", im_with_keypoints);



    waitKey(0);

    return 0;
}


// Below is how I did the last HW assignment. We can use all of these as examples.

/*

#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"


using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    VideoCapture cap(0); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    namedWindow("Dallin Briggs",1);

    int exit_key = 0;
    VideoWriter VOut; // Create a video write object.
    // Initialize video write object (only done once). Change frame size to match your camera resolution.
    VOut.open("VideoOut.avi", CV_FOURCC('M', 'P', 'E', 'G') , 30, Size(640, 480), 0);

    while(1)
    {
        Mat edges;
        Mat current_frame;
        Mat previous_frame;
        Mat frame_gray;
        Mat frame_thresh;
        Mat cdst;
        Mat motion;



        switch(waitKey(1) & 0xFF){
        case '1':
            while(1){
                cap >> current_frame; // get a new current_frame from camera
                cvtColor(current_frame,frame_gray,COLOR_BGR2GRAY);
                imshow("Briggs", current_frame);
                VOut << frame_gray;
                if(waitKey(1) >= 0)
                    break;
            }
            break;
        case '2':
            while(1){
                cap >> current_frame; // get a new current_frame from camera
                cvtColor(current_frame, edges, COLOR_BGR2GRAY);
                GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
                Canny(edges, edges, 0, 30, 3);
                imshow("Briggs", edges);
                VOut << edges;
                if(waitKey(1) >= 0)
                    break;
            }
            break;
        case '3':
            while(1){
                cap >> current_frame;
                cvtColor(current_frame, frame_gray, CV_BGR2GRAY);
                threshold(frame_gray, frame_thresh,100,255,0);
                imshow("Briggs", frame_thresh);
                VOut << frame_thresh;
                if(waitKey(1) >= 0)
                    break;
            }
            break;
        case '4':
            while(1){
                std::vector< cv::Point2f > frame_corners;
                /// Detector parameters
                int blockSize = 3;
                int apertureSize = 3;
                double k = 0.04;
                int thresh = 170;
                Mat copy;

                cap >> current_frame;
                cvtColor(current_frame, frame_gray, CV_BGR2GRAY);
                copy = current_frame.clone();
                goodFeaturesToTrack(frame_gray,frame_corners, 120, .01, 10, Mat(), blockSize, false, .04);

                /// Drawing a circle around corners
                for( size_t i = 0; i < frame_corners.size(); i++ )
                {
                    cv::circle(frame_gray, frame_corners[i], 10, cv::Scalar( 255. ), -1 );
                }

                imshow("Briggs", frame_gray);
                VOut << frame_gray;
                if(waitKey(1) >= 0)
                    break;
            }
            break;
        case '5':
            while(1){
                cap >> current_frame; // get a new current_frame from camera
                cvtColor(current_frame, edges, COLOR_BGR2GRAY);
                GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
                Canny(edges, edges, 0, 30, 3);
                cvtColor(edges, cdst, CV_GRAY2BGR);

                vector<Vec4i> lines;
                HoughLinesP(edges, lines, 1, CV_PI/180, 50, 50, 10 );
                for( size_t i = 0; i < lines.size(); i++ )
                {
                    Vec4i l = lines[i];
                    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
                }

                cvtColor(cdst, cdst, CV_BGR2GRAY);
                imshow("source", current_frame);
                imshow("detected lines", cdst);
                VOut << cdst;
                if(waitKey(1) >= 0)
                    break;
            }
            break;

        case '6':
            while(1){
                frame_gray.copyTo(previous_frame);
                cap >> current_frame; // get a new current_frame from camera
                cvtColor(current_frame,frame_gray,COLOR_BGR2GRAY);
                if(previous_frame.empty())
                {
                    previous_frame = Mat::zeros(frame_gray.size(), frame_gray.type()); // prev frame as black
                    //signed 16bit mat to receive signed difference
                }

                absdiff(frame_gray, previous_frame, motion);

                imshow("Briggs", motion);
                VOut << motion;
                if(waitKey(1) >= 0)
                    break;

            }
            break;
        case '7':
            exit_key = 1;
            break;
        }

        if(exit_key == 1)
            break;

    }
    return 0;
}

*/



/*
 *
 * My code from P3
#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"
#include <fstream>
#include <string>

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    Mat image;
    Mat current_frame;
    Mat previous_frame;
    Mat frame_thresh;
    Mat motion;
    Mat first_frame;
    string filename;
    string header;
    string tail;

    VideoWriter VOut; // Create a video write object.
    // Initialize video write object (only done once). Change frame size to match your camera resolution.
    VOut.open("VideoOut.avi", CV_FOURCC('M', 'P', 'E', 'G') , 30, Size(640, 480), 1);

    namedWindow("Dallin Briggs",1);
    //    SimpleBlobDetector blobby;

    // Setup SimpleBlobDetector parameters.
    SimpleBlobDetector::Params params;

    // Change thresholds
    params.minThreshold = 10;
    params.maxThreshold = 255;

    // Filter by Area.
    params.filterByArea = true;
    params.minArea = 100;

    // Filter by Circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    // Filter by Convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    // Filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = .3;

#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

    // Set up detector with params
    SimpleBlobDetector detector(params);

    // You can use the detector this way
    // detector.detect( im, keypoints);

#else

    // Set up detector with params
    Ptr<SimpleBlobDetector> blobby = SimpleBlobDetector::create(params);

    // SimpleBlobDetector::create creates a smart pointer.
    // So you need to use arrow ( ->) instead of dot ( . )
    // detector->detect( im, keypoints);

#endif


    for(int i = 5; i < 41; i++)
    {
        header = "/home/dallin/Dropbox/Robotic_Vision/HW1_P3/Baseball_images/1R";
        tail = ".jpg";
        filename = header + to_string(i) + tail;

        first_frame = imread(header + '5' + tail, CV_LOAD_IMAGE_GRAYSCALE);
        image = imread(filename,CV_LOAD_IMAGE_GRAYSCALE);

        current_frame.copyTo(previous_frame);

        current_frame = image;
        if(previous_frame.empty())
        {
            previous_frame = Mat::zeros(current_frame.size(), current_frame.type()); // prev frame as black
            //signed 16bit mat to receive signed difference
        }
        absdiff(current_frame, first_frame, motion);
        GaussianBlur(motion,motion, Size(7,7), 1.5, 1.5);
        threshold(motion, frame_thresh,10,255,1);


        // Detect blobs.
        std::vector<KeyPoint> keypoints;
        blobby->detect(frame_thresh, keypoints);

        // Draw detected blobs as red circles.
        // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
        Mat im_with_keypoints;
        drawKeypoints(current_frame, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

        cout << keypoints.size() << endl;
        // Show blobs
        imshow("keypoints", im_with_keypoints );
        VOut << im_with_keypoints;
        //        imshow("Briggs", frame_circles);

        waitKey(100);

    }

    return 0;
}

*/

