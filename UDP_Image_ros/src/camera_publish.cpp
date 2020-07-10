#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <istream>
#include <stdio.h>
#include <vector>
#include <stddef.h>
#include <sys/types.h>
#include <dirent.h>
#include <ros/types.h>
#include "config.h"
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "publish_image");

    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<sensor_msgs::Image>("/camera/image", 1000);

    ros::Rate loop_rate(20);

    VideoCapture capture(0);
    if (!capture.isOpened())
    {
        cout << "open camera failed. " << endl;
        return -1;
    }

    while (nh.ok())
    {
        cv::Mat test_img = cv::imread("/home/wangsen/UDP/src/UDP_Image_ros/img_1.bmp", 1);
        sensor_msgs::ImagePtr img_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", test_img).toImageMsg();
        // Mat frame;
        // capture >> frame;
        // Rect rect(0, 0, 672, 376);
        // frame = frame(rect);
        // sensor_msgs::ImagePtr img_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame).toImageMsg();
        pub.publish(img_msg);
        loop_rate.sleep();
    }

    // VideoCapture cap;
    // cap.open("/home/wangsen/桌面/encode_output.h264");
    // Mat frame;
    // cap >> frame;
    // while (!frame.empty())
    // {
    //     cv::imshow("img", frame);
    //     cap >> frame;
    //     cv::waitKey(30);
    // }
    ros::spin();
    return 0;
}
