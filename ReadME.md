## ROS-UDP-H.264-Image

**运行流程：**  

**Publish camera(ZED) image：**

`rosrun udp publish_image`  

**client：**

`rosrun udp client 'server-ip' 'server-port'`  

`eg. rosrun udp client 127.0.0.1 11311`

 - 'server-ip' 代表服务器端 IP 地址 
 - 'server-port'代表服务器端 端口号

**server：**  

`rosrun udp server 'server-port' `   

`eg. rosrun udp server 11311`

 - 'server-port'代表服务器端 端口号

**Display picture：**  

`rosrun udp receive_decode_image`   