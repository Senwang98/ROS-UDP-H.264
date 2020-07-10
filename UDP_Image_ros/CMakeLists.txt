cmake_minimum_required(VERSION 2.8.3)
project(udp)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS
   roscpp
   rospy
   std_msgs
   genmsg
   message_generation
   sensor_msgs
   cv_bridge
   image_transport  
)
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

## Declare ROS messages and services
#add_message_files (DIRECTORY msg FILES ActuatorControl.msg)

## Declare a catkin package
catkin_package(
  INCLUDE_DIRS include
  DEPENDS system_lib opencv
)

include_directories(include ${catkin_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_executable(client src/Client.cpp src/PracticalSocket.cpp src/h264_encoder.cpp src/h264_decoder.cpp)
target_link_libraries(client ${catkin_LIBRARIES} ${OpenCV_LIBS} ${catkin_LIBRARIES} x264 avcodec avutil swscale)

add_executable(server src/Server.cpp src/PracticalSocket.cpp src/h264_encoder.cpp src/h264_decoder.cpp)
target_link_libraries(server ${catkin_LIBRARIES} ${OpenCV_LIBS} ${catkin_LIBRARIES} x264 avcodec avutil swscale)

add_executable(publish_image src/camera_publish.cpp)
target_link_libraries(publish_image ${catkin_LIBRARIES} ${OpenCV_LIBS})

add_executable(receive_decode_image src/receive_decode_image.cpp)
target_link_libraries(receive_decode_image ${catkin_LIBRARIES} ${OpenCV_LIBS})

add_executable(test src/test.cpp src/decoder_test.cpp)
target_link_libraries(test ${catkin_LIBRARIES} ${OpenCV_LIBS} ${catkin_LIBRARIES} x264 avcodec avutil swscale)

#cmake_minimum_required(VERSION 2.8)
#project( lan_vid_pseudostream )
#find_package( OpenCV REQUIRED )
#add_executable( server Server.cpp PracticalSocket.cpp )
#target_link_libraries( server ${OpenCV_LIBS} )
#add_executable( client Client.cpp PracticalSocket.cpp )
#target_link_libraries( client ${OpenCV_LIBS} )
