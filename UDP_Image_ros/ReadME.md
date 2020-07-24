## ROS-UDP-H.264-Image

**运行流程：**

##### 		1. ready节点，发送ready信号，告诉编码端开始编码

​	**`rosrun udp ready`**  

**2. 解码端：**  

​	**`rosrun udp server  `**   

**3. 显示端（如需）：**  

​	topic:  /camera/front120

​	**`rosrun udp receive_decode_image`**   



**Note:**

- ​	修改config.h参数后执行catkin_make重新编译。

