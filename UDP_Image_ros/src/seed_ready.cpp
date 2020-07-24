#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include "config.h"
#include "PracticalSocket.h" 
#include <iostream>
#include <cstdlib>

using namespace std;

UDPSocket sock;
unsigned short servPort;
string servAddress;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "ready");
    ros::NodeHandle nh;

    servAddress = IP_SEED;
    servPort = Socket::resolveService(PORT_SEED, "udp");
    ros::Rate loop_rate(5);
    ros::Time t = ros::Time::now();
    cout << t << endl;
    while (nh.ok())
    {
        char ibuf[5] = {'r', 'e', 'a', 'd', 'y'};
        sock.sendTo(ibuf, 5, servAddress, servPort);
        loop_rate.sleep();
    }
    ros::spin();
}