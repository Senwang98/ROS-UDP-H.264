#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include "config.h"
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>
#include <cstdlib>

#include <h264_encoder.h>
#include <h264_decoder.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

UDPSocket sock;
vector<uchar> encoded;
clock_t last_cycle;
unsigned short servPort;
string servAddress;
int encode_num = 0;
static int i = 0;

void Callback(const sensor_msgs::ImageConstPtr &cam_image)
{
    cv_bridge::CvImagePtr cv_ptr;
    Mat frame, send;
    cv_ptr = cv_bridge::toCvCopy(cam_image, sensor_msgs::image_encodings::BGR8);
    if (i == 0)
        last_cycle = clock();
    frame = cv_ptr->image;

    ros::Time::init();
    std_msgs::Header header;
    cv_bridge::CvImage cvmsg(header, "bgr8", frame);
    sensor_msgs::ImagePtr msg = cvmsg.toImageMsg();

    // 开始编码
    fclose(stderr);
    H264Encoder *encoder = new H264Encoder(msg->width, msg->height, 20, 30, 1, msg->encoding);
    ros::Time before_encoding = ros::Time::now();
    H264EncoderResult res = encoder->encode(msg);
    ros::Time after_encoding = ros::Time::now();

    // // 文件的保存
    // char *path = "/home/wangsen/桌面/encode_YUV420P.h264";
    // FILE *fp;
    // if ((fp = fopen(path, "wb")) == NULL)
    // {
    //     cout << "文件打开失败！" << endl;
    //     exit(0);
    // }
    // if (fwrite(res.frame_data, sizeof(uint8_t), res.frame_size, fp) != 1)
    // {
    //     cout << "frame_size = " << res.frame_size << endl;
    //     cout << "写入成功！" << endl;
    // }
    // fclose(fp);

    // 发送
    int total_pack = 1 + (res.frame_size) / PACK_SIZE;

    int ibuf[1];
    ibuf[0] = total_pack;
    ros::Time time = ros::Time::now();
    std::stringstream sstream;
    string sec;
    sstream << time;
    sstream >> sec;

    printf("编码编号 = %d\n", ++encode_num);
    printf("编码数据块数 = %d\n", total_pack);
    cout << "编码时间戳 = " << sec << endl;

    char st[26] = "start1594797026.163053312";
    for (int i = 5; i <= 25; i++)
        st[i] = sec[i - 5];
    sock.sendTo(st, 25, servAddress, servPort);
    sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

    for (int i = 0; i < total_pack; i++)
    {
        sock.sendTo(&res.frame_data[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);
    }

    // 计算FPS与码率
    // clock_t next_cycle = clock();
    // double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
    // cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;
    // cout << next_cycle - last_cycle;
    // last_cycle = next_cycle;
    delete encoder;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "client_node");
    ros::NodeHandle nh;
    servAddress = IP_self;
    servPort = Socket::resolveService(PORT_ZED, "udp");
    ros::Subscriber right_im_sub = nh.subscribe<sensor_msgs::Image>("/camera/image", 1000, Callback);
    ros::spin();
}