#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include "config.h"
#include <iostream>
#include <cstdlib>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;
static int i = 0;

void Callback(const sensor_msgs::ImageConstPtr &cam_image)
{
    cv_bridge::CvImagePtr cv_ptr;
    ros::Time stamp = cam_image->header.stamp;
    cout << "显示端时间戳 = " << stamp << endl;
    Mat frame;
    cv_ptr = cv_bridge::toCvCopy(cam_image, sensor_msgs::image_encodings::BGR8);
    frame = cv_ptr->image;
    if (frame.rows > 0 && frame.cols > 0)
    {
        cv::resize(frame, frame, cv::Size(960, 640));
        namedWindow("result", CV_WINDOW_NORMAL);
        imshow("result", frame);
        waitKey(20);
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "receive_decode_image");
    ros::NodeHandle nh;
    ros::Subscriber img = nh.subscribe<sensor_msgs::Image>("/camera/front120", 1000, Callback);
    ros::Rate loop_rate(1);
    ros::spin();
}