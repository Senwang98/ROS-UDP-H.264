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
    Mat frame;
    cv_ptr = cv_bridge::toCvCopy(cam_image, sensor_msgs::image_encodings::BGR8);
    frame = cv_ptr->image;
    cv::imwrite("/home/wangsen/UDP/src/UDP_Image_ros/image/" + to_string(i++) + ".jpg", frame);
    cv::imshow("client", frame);
    cv::waitKey(30);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "receive_decode_image");
    ros::NodeHandle nh;
    ros::Subscriber img = nh.subscribe<sensor_msgs::Image>("/camera/front120", 1000, Callback);
    ros::Rate loop_rate(1);
    ros::spin();
}