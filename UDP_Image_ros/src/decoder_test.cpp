#include "decoder_test.h"
using namespace std;

void H264Decode::init()
{

    matReady = false;

    avcodec_register_all();
    av_init_packet(&avpkt);

    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c)
    {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    if (avcodec_open2(c, codec, NULL) < 0)
    {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame)
    {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }

    frame_count = 0;

    //存储解码后转换的RGB数据
    pFrameBGR = av_frame_alloc();
}

H264Decode::~H264Decode()
{
    avcodec_close(c);
    av_free(c);
    av_free(frame);
    av_free(pFrameBGR);
    sws_freeContext(img_convert_ctx);
    delete (out_buffer);
}

void H264Decode::decode(unsigned char *inputbuf, size_t size)
{

    avpkt.size = size;
    if (avpkt.size == 0)
        return;

    avpkt.data = inputbuf;

    int len, got_frame;

    len = avcodec_decode_video2(c, frame, &got_frame, &avpkt);
    if (got_frame == 0)
    {
        len = avcodec_decode_video2(c, frame, &got_frame, &avpkt);
        // cout << "第二次解码开始！" << endl;
    }
    // cout << "got_frame = " << got_frame << endl;
    // cout << "len = " << len << endl;

    if (len < 0)
    {
        matReady = false;
        fprintf(stderr, "Error while decoding frame %d\n", frame_count);
        frame_count++;

        return;
    }
    if (out_buffer == nullptr)
    {
        BGRsize = avpicture_get_size(AV_PIX_FMT_BGR24, c->width, c->height);
        out_buffer = (uint8_t *)av_malloc(BGRsize);
        avpicture_fill((AVPicture *)pFrameBGR, out_buffer, AV_PIX_FMT_BGR24, c->width, c->height);

        img_convert_ctx = sws_getContext(c->width, c->height, c->pix_fmt,
                                         c->width, c->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,
                                         NULL);
        pCvMat.create(cv::Size(c->width, c->height), CV_8UC3);
    }
    if (got_frame)
    {
        matReady = true;
        sws_scale(img_convert_ctx, (const uint8_t *const *)frame->data,
                  frame->linesize, 0, c->height, pFrameBGR->data, pFrameBGR->linesize);

        memcpy(pCvMat.data, out_buffer, BGRsize);

        //        printf("decoding frame: %d\n",frame_count);
        frame_count++;
    }
    else
    {
        matReady = false;
    }
    if (avpkt.data)
    {
        avpkt.size -= len;
        avpkt.data += len;
    }
}

static int cnt = 0;
void H264Decode::play()
{
    // cout << "matReady = " << matReady << endl;
    if (matReady)
    {
        // cv::imshow("decode", pCvMat);
        cv::Mat tmp;
        cv::resize(pCvMat, tmp, cv::Size(600, 400));

        cv::imshow("decode", tmp);
        // cout << "size = " << pCvMat.rows << " " << pCvMat.cols << endl;
        // cv::imwrite("/home/wangsen/UDP/src/UDP_Image_ros/image/" + to_string(cnt++) + ".bmp", pCvMat);
        cv::waitKey(5);
    }
}

H264Decode::H264Decode()
{
    init();
}

cv::Mat H264Decode::getMat()
{
    if (matReady)
    {
        return pCvMat;
    }
    else
    {
        return cv::Mat();
    }
}
