#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <h264_encoder.h>
#include <h264_decoder.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include "decoder_test.h"
#include "config.h"
#include "PracticalSocket.h"
#include <thread>
#include <vector>

using namespace std;
using namespace cv;

#define BUF_LEN 65540 // MAX UDP packet size = 65535 = 65515+20(header)

vector<unsigned char> arr;

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "server_node");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<sensor_msgs::Image>("/camera/front120", 1000);
    ros::Rate loop_rate(25);
    sensor_msgs::ImagePtr out(new sensor_msgs::Image);

    H264Decode decoder;
    UDPSocket sock(PORT_RECV);
    char buffer[BUF_LEN];      // Buffer for echo string
    int recvMsgSize;           // Size of received message
    string sourceAddress;      // Address of datagram source
    unsigned short sourcePort; // Port of datagram source
    clock_t last_cycle = clock();
    int start_len = Need_ZED == 0 ? 26 : 25;
    int pic_num = 0;
    int video_num = 0;
    int decode_num = 0; // 只统计解码的图像个数

    while (nh.ok())
    {
        string time_stamp = "";
        memset(buffer, 0, sizeof(buffer));
        do
        {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            cout << "recvMsgSize = " << recvMsgSize << endl;
            if (recvMsgSize == start_len)
            {
                string start_flag = "";
                for (int i = 0; i < 5; i++)
                    start_flag += buffer[i];

                time_stamp = "";
                for (int i = 6; i < start_len; i++)
                    time_stamp += buffer[i];

                if (start_flag == "start")
                    break;
            }
        } while (1);
        cout << "ok!" << endl;
        int total_pack;
        if (Need_ZED)
        {
            do
            {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            } while (recvMsgSize > sizeof(int));
            total_pack = (int)buffer[0];
        }
        else
            total_pack = (int)buffer[5];

        unsigned char *longbuf = new unsigned char[PACK_SIZE * total_pack];
        int cnt = 0;
        for (int i = 0; i < total_pack; i++)
        {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            cnt += recvMsgSize;
            if (recvMsgSize != PACK_SIZE)
            {
                cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                continue;
            }
            memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
        }
        printf("解码编号 : %d\n", ++decode_num);
        cout << "解码时间戳 = " << time_stamp << endl;
        printf("解码数据块数 : %d\n", total_pack);
        printf("解码总数据量 : %d\n", cnt);

        for (int i = 0; i < cnt; i++)
            arr.push_back(longbuf[i]);
        if (++pic_num == SAVE_PICTURE_NUMBER && SAVE_FLAG)
        {
            ofstream fo;
            string name = to_string(++video_num);
            reverse(name.begin(), name.end());
            while (name.size() < 5)
            {
                name += "0";
            }
            reverse(name.begin(), name.end());
            string root_path = H264_SAVE_PATH;
            string context = root_path + "/Encode_" + name + ".h264";
            fo.open(context);
            fo << total_pack;
            for (int i = 0; i < time_stamp.size(); i++)
                fo << time_stamp[i];
            for (vector<unsigned char>::iterator i = arr.begin(), end = arr.end(); i != end; ++i)
                fo << (*i);
            arr.clear();
            pic_num = 0;
            fo.close();
        }

        // 新版本的解码部分
        decoder.decode(longbuf, cnt);
        cv::Mat result = decoder.getMat();
        cv_bridge::CvImage cvImage;
        cvImage.image = result;
        cvImage.encoding = sensor_msgs::image_encodings::BGR8;

        uint32_t sec, usec;
        string tmp_sec = "", tmp_usec = "";
        for (int i = 0; i < 10; i++)
            tmp_sec += time_stamp[i];
        for (int i = 11; i <= 19; i++)
            tmp_usec += time_stamp[i];

        istringstream is1(tmp_sec);
        is1 >> sec;
        istringstream is2(tmp_usec);
        is2 >> usec;
        ros::Time stamp = ros::Time(sec, usec);
        cvImage.header.stamp = stamp;
        sensor_msgs::ImagePtr img_msg = cvImage.toImageMsg();
        // sensor_msgs::ImagePtr img_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", result).toImageMsg();
        pub.publish(img_msg);
        loop_rate.sleep();

        // decoder.play();

        // clock_t next_cycle = clock();
        // double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
        // // cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;
        // last_cycle = next_cycle;
        delete longbuf;
    }
    return 0;
}
