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
#define BUF_LEN 65540 // MAX UDP packet size = 65535 = 65515+20(header)

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "server_node");
    ros::NodeHandle nh;
    ros::Publisher pub = nh.advertise<sensor_msgs::Image>("/camera/front120", 1000);

    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <Server Port>" << endl;
        exit(1);
    }

    unsigned short servPort = atoi(argv[1]); // port

    // char *path = "/home/wangsen/桌面/Encode_result/encode_NV12.h264";
    char *path = "/home/wangsen/桌面/Encode_result/encode_output.h264";
    FILE *fp;
    if ((fp = fopen(path, "r+b")) == NULL)
        cout << "文件打开失败！" << endl;

    
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    printf("len = %d\n", len);
    int tmp_n = int(len / PACK_SIZE);
    if(tmp_n * PACK_SIZE < len)
        tmp_n++;
    uint8_t *longbuf = new uint8_t[PACK_SIZE * tmp_n];
    for (int i = len; i < PACK_SIZE * tmp_n; i++)
        longbuf[i] = uint8_t(0);
    rewind(fp);

    // cout << "fread(&longbuf, sizeof(uint8_t), len, fp) = " << fread(longbuf, sizeof(uint8_t), len, fp) << endl;

    if (fread(longbuf, sizeof(uint8_t), len, fp) != len)
    {
        cout << "读取失败!" << endl;
    }
    fclose(fp);
    // for(int i=len;i<PACK_SIZE;i++)
    //     longbuf[i] = (uint8_t)0;

    sensor_msgs::ImagePtr out(new sensor_msgs::Image);
    H264Decoder *decoder = new H264Decoder(FRAME_WIDTH, FRAME_HEIGHT);
    
    len = decoder->decode(len, longbuf, out);
    cout << "ok" << endl;
    if (out->width > 0 && out->height > 0)
    {
        cout << "ok" << endl;
        cv_bridge::CvImagePtr cvout = cv_bridge::toCvCopy(out);
        cv::Mat img = cvout->image;
        cv::imshow("decoded image display", img);
        cv::imwrite("/home/wangsen/桌面/ans.bmp", img);
        cv::waitKey(0);
    }
    delete decoder;


    // UDPSocket sock(servPort);

    // char buffer[BUF_LEN];      // Buffer for echo string
    // int recvMsgSize;           // Size of received message
    // string sourceAddress;      // Address of datagram source
    // unsigned short sourcePort; // Port of datagram source

    // clock_t last_cycle = clock();

    // // ros::Rate loop_rate(20);
    // while (nh.ok())
    // {
    //     memset(buffer, 0, sizeof(buffer));
    //     do
    //     {
    //         recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            
    //     } while (recvMsgSize > sizeof(int));

    //     // cout << "recvMsgSize = " << recvMsgSize << " sizeof(int) = " << sizeof(int) << endl;
    //     // cout << "strlen(buffer) = " << strlen(buffer) << endl;
    //     int total_pack = ((int *)buffer)[0];
    //     cout << "total pack = " << total_pack << endl;

    //     // cout << "sourceAddress:" << sourceAddress << " sourcePort:" << sourcePort << endl;
    //     // cout << "expecting length of packs:" << total_pack << endl;

    //     uint8_t *longbuf = new uint8_t[PACK_SIZE * total_pack];
    //     int cnt = 0;
    //     for (int i = 0; i < total_pack; i++)
    //     {
    //         recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
    //         cnt += recvMsgSize;
    //         // if (i == 0)
    //         //     cout << "recvMsgSize = " << recvMsgSize << " cnt = " << cnt << " string = " << ((int *)buffer)[i] << endl;
    //         // cout << "recvMsgSize = " << cnt << endl;
    //         if (recvMsgSize != PACK_SIZE)
    //         {
    //             cerr << "Received unexpected size pack:" << recvMsgSize << endl;
    //             continue;
    //         }
    //         memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
    //     }

    //     cout << "Recv size = " << cnt << endl;
    //     // cout << "**************" << endl;
    //     // cout << endl;

    //     // cout << "Received packet from " << sourceAddress << ":" << sourcePort << endl;
    //     // 开始解码
    //     sensor_msgs::ImagePtr out(new sensor_msgs::Image);
    //     H264Decoder *decoder = new H264Decoder(FRAME_WIDTH, FRAME_HEIGHT);
    //     // H264Decoder decoder(FRAME_WIDTH, FRAME_HEIGHT);
    //     int len = decoder->decode(cnt, longbuf, out);
    //     // std::cout << "Image decoded, decoded image size is: " << out->width << "x" << out->height << std::endl;

    //     if (out->width > 0 && out->height > 0)
    //     {
    //         cv_bridge::CvImagePtr cvout = cv_bridge::toCvCopy(out);
    //         cv::Mat img = cvout->image;
    //         cv::imshow("decoded image display", img);
    //         cv::waitKey(30);

    //         // topic 发送
    //         // sensor_msgs::ImagePtr img_msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", img).toImageMsg();
    //         // pub.publish(img_msg);
    //         // loop_rate.sleep();
    //     }

    //     clock_t next_cycle = clock();
    //     double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
    //     // cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

    //     // cout << next_cycle - last_cycle;
    //     last_cycle = next_cycle;

    //     delete longbuf;
    //     delete decoder;
    // }

    return 0;
}
