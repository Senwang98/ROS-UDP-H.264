#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include "config.h"
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#include <h264_encoder.h>
#include <h264_decoder.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

using namespace std;
using namespace cv;
// 65540
#define BUF_LEN 65540 // Larger than maximum UDP packet size

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "server_node");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<sensor_msgs::Image>("/image/decode", 1000);

    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <Server Port>" << endl;
        exit(1);
    }

    unsigned short servPort = atoi(argv[1]); // port

    UDPSocket sock(servPort);

    char buffer[BUF_LEN];      // Buffer for echo string
    int recvMsgSize;           // Size of received message
    string sourceAddress;      // Address of datagram source
    unsigned short sourcePort; // Port of datagram source

    clock_t last_cycle = clock();

    ros::Rate loop_rate(20);
    while (nh.ok())
    {
        do
        {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
        } while (recvMsgSize > sizeof(int));

        // cout << "recvMsgSize = " << recvMsgSize << " sizeof(int) = " << sizeof(int) << endl;
        // cout << "strlen(buffer) = " << strlen(buffer) << endl;
        int total_pack = ((int *)buffer)[0];
        // cout << "total pack = " << total_pack << endl;

        // cout << "sourceAddress:" << sourceAddress << " sourcePort:" << sourcePort << endl;
        // cout << "expecting length of packs:" << total_pack << endl;

        uint8_t *longbuf = new uint8_t[PACK_SIZE * total_pack];
        int cnt = 0;
        for (int i = 0; i < total_pack; i++)
        {
            recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            cnt += recvMsgSize;
            // if (i == 0)
            //     cout << "recvMsgSize = " << recvMsgSize << " cnt = " << cnt << " string = " << ((int *)buffer)[i] << endl;
            if (recvMsgSize != PACK_SIZE)
            {
                cerr << "Received unexpected size pack:" << recvMsgSize << endl;
                continue;
            }
            memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
        }

        // cout << "Recv size = " << cnt << endl;
        // cout << "**************" << endl;
        // cout << endl;

        // cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;
        // 开始解码
        sensor_msgs::ImagePtr out(new sensor_msgs::Image);
        ros_h264_streamer::H264Decoder decoder(FRAME_WIDTH, FRAME_HEIGHT);
        int len = decoder.decode(cnt, longbuf, out);
        // std::cout << "Image decoded, decoded image size is: " << out->width << "x" << out->height << std::endl;

        if (out->width > 0 && out->height > 0)
        {
            cv_bridge::CvImagePtr cvout = cv_bridge::toCvCopy(out);
            cv::Mat img = cvout->image;
            // cv::imshow("decoded image display", img);
            // cv::waitKey(30);

            // topic 发送
            sensor_msgs::ImagePtr img_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img).toImageMsg();
            pub.publish(img_msg);
            loop_rate.sleep();
        }

        clock_t next_cycle = clock();
        double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
        // cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

        // cout << next_cycle - last_cycle;
        last_cycle = next_cycle;
    }
    ros::spin();
    return 0;
}
