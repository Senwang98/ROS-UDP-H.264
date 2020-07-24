#include "decoder_test.h"
#include <fstream>
#include <ros/ros.h>

#include "config.h"
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#include <h264_encoder.h>
#include <h264_decoder.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

using namespace std;
using namespace cv;

unsigned char buf[700000];

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "test");
    ros::NodeHandle nh;

    H264Decode decoder;

    std::ifstream fin("/home/wangsen/桌面/Encode_result/encode_output.h264", std::ios_base::binary);
    // std::ifstream fin("/home/wangsen/桌面/encode_output_1920_1080.h264", std::ios_base::binary);
    fin.seekg(0, std::ios::end);
    int len = fin.tellg();
    fin.seekg(0, std::ios::beg);

    fin.read((char *)buf, len);
    decoder.decode(buf, len);
    decoder.play();
}