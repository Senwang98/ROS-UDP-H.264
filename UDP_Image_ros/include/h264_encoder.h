#ifndef _H_ROS_H264_ENCODER_H_
#define _H_ROS_H264_ENCODER_H_

#include <h264_api.h>
#include <sensor_msgs/Image.h>
#include <stdint.h>

struct x264_param_t;
struct x264_t;

struct H264_API H264EncoderResult
{
    H264EncoderResult() : frame_size(0), frame_data(0) {}
    int frame_size;
    uint8_t *frame_data;
};

class H264EncoderImpl;

class H264_API H264Encoder
{
public:
    // 画面质量1～100依次代表画质下降
    // FPS通过fps_num / fps_den给出
    H264Encoder(int width, int height, int quality_level, int fps_num, int fps_den, const std::string &encoding, bool streaming = true);

    H264EncoderResult encode(const sensor_msgs::ImageConstPtr &img, uint64_t pts = 0);
    // ~H264Encoder()
    // {
    //     delete impl;
    // }
    x264_param_t *GetParameters();

    x264_t *GetEncoder();

    /* Actual type: x264_picture_t */
    void *GetPicIn();

    /* Actual type: x264_picture_t */
    void *GetPicOut();

private:
    boost::shared_ptr<H264EncoderImpl> impl;
};

#endif
