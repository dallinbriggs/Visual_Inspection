#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{

    Mat image;
    Mat image_original;
    Mat current_frame;
    Mat previous_frame;
    Mat frame_thresh;
    string filename;
    string header;
    string tail;

    //    namedWindow("Team 10",1);
    header = "/home/dallin/robotic_vision_ws/Visual_Inspection/image_proc_qtc/images/";
    tail = ".bmp";
    filename = header + "nilla_edge" + tail;

    image_original = imread(filename,CV_LOAD_IMAGE_COLOR);
    image = imread(filename,CV_LOAD_IMAGE_GRAYSCALE);
    GaussianBlur(image, image, Size(7,7), 1.5, 1.5);
    threshold(image, image,80,255,0);

    cv::Rect bottomROI(cv::Point(150,480), cv::Point(520, 400));
    cv::Rect topROI(cv::Point(150,0), cv::Point(520,80));
    int bottomArea = cv::countNonZero(image(bottomROI));
    int topArea = cv::countNonZero(image(topROI));
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
    params_good.minArea = 10500;
    params_good.maxArea = 12000;
    params_acceptable.filterByArea = true;
    params_acceptable.minArea = 8000;
    params_acceptable.maxArea = 10500;
    params_bad.filterByArea = true;
    params_bad.minArea = 4000;
    params_bad.maxArea = 8000;

    // Filter by Circularity
    params_good.filterByCircularity = true;
    params_good.minCircularity = .82;
    params_good.maxCircularity = 1;
    params_acceptable.filterByCircularity = true;
    params_acceptable.minCircularity = .6;
    params_acceptable.maxCircularity = .85;
    params_bad.filterByCircularity = true;
    params_bad.minCircularity = .5;
    params_bad.maxCircularity = .8;

    // Filter by Convexity
    params_good.filterByConvexity = true;
    params_good.minConvexity = .95;
    params_good.maxConvexity = 1;
    params_acceptable.filterByConvexity = true;
    params_acceptable.minConvexity = .7;
    params_acceptable.maxConvexity = 1;
    params_bad.filterByConvexity = true;
    params_bad.minConvexity = .4;
    params_bad.maxConvexity = 1;

    // Filter by Inertia
    params_good.filterByInertia = true;
    params_good.minInertiaRatio = .7;
    params_good.maxInertiaRatio = 1;
    params_acceptable.filterByInertia = true;
    params_acceptable.minInertiaRatio = .7;
    params_acceptable.maxInertiaRatio = 1;
    params_bad.filterByInertia = true;
    params_bad.minInertiaRatio = .4;
    params_bad.maxInertiaRatio = .8;

#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

    // Set up detector with params
    SimpleBlobDetector blobby(params);

    // You can use the detector this way
    blobby.detect(image, keypoints);

#else

    // Set up detector with params
    Ptr<SimpleBlobDetector> blobby_good = SimpleBlobDetector::create(params_good);
    Ptr<SimpleBlobDetector> blobby_acceptable = SimpleBlobDetector::create(params_acceptable);
    Ptr<SimpleBlobDetector> blobby_bad = SimpleBlobDetector::create(params_bad);

    // SimpleBlobDetector::create creates a smart pointer.
    // So you need to use arrow ( ->) instead of dot ( . )
    // detector->detect( im, keypoints);

#endif

    // Detect blobs.
    std::vector<KeyPoint> keypoints_good;
    std::vector<KeyPoint> keypoints_acceptable;
    std::vector<KeyPoint> keypoints_bad;
    blobby_good->detect(image, keypoints_good);
    blobby_acceptable->detect(image, keypoints_acceptable);
    blobby_bad->detect(image, keypoints_bad);

    // Draw detected blobs as red circles.
    // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
    Mat im_with_keypoints;

    if(bottomArea < 100 && topArea < 100)
    {
        if(!keypoints_good.empty())
        {
            drawKeypoints(image_original, keypoints_good, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
            cout << keypoints_good.size() << " Good!" << endl;
            cv::putText(im_with_keypoints, "Cookie!", cvPoint(175,150),2, 2.5, cvScalar(255), 1, CV_AA); //FONT_HERSHEY_DUPLEX == 2
        }
        else if(!keypoints_acceptable.empty())
        {
            drawKeypoints(image_original, keypoints_acceptable, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
            cout << keypoints_acceptable.size() << " Acceptable." << endl;
        }
        else if(!keypoints_bad.empty())
        {
            drawKeypoints(image_original, keypoints_bad, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
            cout << keypoints_bad.size() << " Bad!" << endl;
        }
        else
        {
            drawKeypoints(image_original, keypoints_bad, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
        }
        imshow("Team 10", im_with_keypoints);
    }
    else
    {
        imshow("Team 10", image_original);
        std::cout << bottomArea << ", " << topArea << std::endl;
    }


    waitKey(0);

    return 0;
}

