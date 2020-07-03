#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include "config.h"
#include "PracticalSocket.h" // For UDPSocket and SocketException
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()

#include <h264_encoder.h>
#include <h264_decoder.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

UDPSocket sock;
int jpegqual = ENCODE_QUALITY; // Compression Parameter

//Mat frame,send;

vector<uchar> encoded;
clock_t last_cycle;
unsigned short servPort;
string servAddress;

void Callback(const sensor_msgs::ImageConstPtr &cam_image)
{
    // cout << "OK" << endl;
    cv_bridge::CvImagePtr cv_ptr;

    Mat frame, send;

    static int i = 0;
    // cout << "frame = " << i++ << endl;
    cv_ptr = cv_bridge::toCvCopy(cam_image, sensor_msgs::image_encodings::BGR8);

    if (i == 0)
        last_cycle = clock();

    frame = cv_ptr->image;
    // cv::imshow("client", frame);
    // cv::waitKey(30);

    ros::Time::init();
    std_msgs::Header header;
    cv_bridge::CvImage cvmsg(header, "bgr8", frame);
    sensor_msgs::ImagePtr msg = cvmsg.toImageMsg();
    // 开始编码
    ros_h264_streamer::H264Encoder encoder(msg->width, msg->height, 20, 30, 1, msg->encoding);
    ros::Time before_encoding = ros::Time::now();
    ros_h264_streamer::H264EncoderResult res = encoder.encode(msg);
    ros::Time after_encoding = ros::Time::now();

    ros::Duration encoding_duration = after_encoding - before_encoding;
    // std::cout << "Image encoded, encoded size is " << res.frame_size << std::endl;
    // std::cout << "Encoding took " << encoding_duration << std::endl;

    int total_pack = 1 + (res.frame_size) / PACK_SIZE;
    // cout << "**********" << endl;
    // cout << "Total pack = " << total_pack << endl;
    // cout << "Total size = " << res.frame_size << endl;
    // cout << endl;

    int ibuf[1];
    ibuf[0] = total_pack;
    sock.sendTo(ibuf, sizeof(int), servAddress, servPort);
    // cout << "servAddress:" << servAddress << " servPort:" << servPort << endl;

    for (int i = 0; i < total_pack; i++)
        sock.sendTo(&res.frame_data[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);

    // waitKey(FRAME_INTERVAL);

    clock_t next_cycle = clock();
    double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
    // cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

    // cout << next_cycle - last_cycle;
    last_cycle = next_cycle;

    // // 开始解码
    // sensor_msgs::ImagePtr out(new sensor_msgs::Image);
    // ros_h264_streamer::H264Decoder decoder(msg->width, msg->height);

    // ros::Time before_decoding = ros::Time::now();
    // int len = decoder.decode(res.frame_size, res.frame_data, out);
    // ros::Time after_decoding = ros::Time::now();

    // std::cout << "Image decoded, decoded image size is: " << out->width << "x" << out->height << std::endl;
    // std::cout << "Decoding took " << (after_decoding - before_decoding) << std::endl;

    // cv_bridge::CvImagePtr cvout = cv_bridge::toCvCopy(out);
    // std::cout << "Saving to disk" << std::endl;
    // cv::imshow("test", cvout->image);
    // cv::waitKey(30);

    // resize(frame, send, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);
    // vector<int> compression_params;
    // compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    // compression_params.push_back(jpegqual);

    // imencode(".jpg", send, encoded, compression_params);
    // //  imshow("send", send);
    // //  cvWaitKey(1);
    // int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

    // int ibuf[1];
    // ibuf[0] = total_pack;
    // sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

    // for (int i = 0; i < total_pack; i++)
    //     sock.sendTo(&encoded[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);

    // // waitKey(FRAME_INTERVAL);

    // clock_t next_cycle = clock();
    // double duration = (next_cycle - last_cycle) / (double)CLOCKS_PER_SEC;
    // cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

    // cout << next_cycle - last_cycle;
    // last_cycle = next_cycle;

    // if (i++ > 5)
    //     i = 5;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "client_node");
    ros::NodeHandle nh;

    servAddress = argv[1];
    servPort = Socket::resolveService(argv[2], "udp");

    ros::Subscriber right_im_sub = nh.subscribe<sensor_msgs::Image>("/camera/image", 1000, Callback);
    ros::Rate loop_rate(0.01);
    ros::spin();
}

// int main(int argc, char * argv[]) {
//     if ((argc < 3) || (argc > 3)) { // Test for correct number of arguments
//         cerr << "Usage: " << argv[0] << " <Server> <Server Port>\n";
//         exit(1);
//     }

//     string servAddress = argv[1]; // First arg: server address
//     unsigned short servPort = Socket::resolveService(argv[2], "udp");

//     try {
//         UDPSocket sock;
//         int jpegqual =  ENCODE_QUALITY; // Compression Parameter

//         Mat frame, send;
//         vector < uchar > encoded;
//         VideoCapture cap(0); // Grab the camera
//         namedWindow("send", CV_WINDOW_AUTOSIZE);
//         if (!cap.isOpened()) {
//             cerr << "OpenCV Failed to open camera";
//             exit(1);
//         }

//         clock_t last_cycle = clock();
//         while (1) {
//             cap >> frame;
//             if(frame.size().width==0)continue;//simple integrity check; skip erroneous data...
//             resize(frame, send, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);
//             vector < int > compression_params;
//             compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
//             compression_params.push_back(jpegqual);

//             imencode(".jpg", send, encoded, compression_params);
//             imshow("send", send);
//             int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

//             int ibuf[1];
//             ibuf[0] = total_pack;
//             sock.sendTo(ibuf, sizeof(int), servAddress, servPort);

//             for (int i = 0; i < total_pack; i++)
//                 sock.sendTo( & encoded[i * PACK_SIZE], PACK_SIZE, servAddress, servPort);

//             waitKey(FRAME_INTERVAL);

//             clock_t next_cycle = clock();
//             double duration = (next_cycle - last_cycle) / (double) CLOCKS_PER_SEC;
//             cout << "\teffective FPS:" << (1 / duration) << " \tkbps:" << (PACK_SIZE * total_pack / duration / 1024 * 8) << endl;

//             cout << next_cycle - last_cycle;
//             last_cycle = next_cycle;
//         }
//         // Destructor closes the socket

//     } catch (SocketException & e) {
//         cerr << e.what() << endl;
//         exit(1);
//     }

//     return 0;
// }
