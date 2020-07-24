#define FRAME_INTERVAL (1000 / 30)
// 一次UDP发送的字节数
#define PACK_SIZE 20000
// 编码质量(未使用)
#define ENCODE_QUALITY 80

/* IP::Port*/
// 本机IP
#define IP_self "127.0.0.1"
// ready节点需要发送对象的IP
#define IP_SEED "192.168.30.68"
// ready发送对象的端口
#define PORT_SEED "8889"
// 接收编码数据的端口
#define PORT_RECV 8888
#define PORT_ZED "8888"

// 0不使用ZED相机测试，1表示使用ZED相机测试（实际应用为0）
#define Need_ZED 0
// ZED相机分辨率
// #define FRAME_HEIGHT 376
// #define FRAME_WIDTH 672

// 实际的编码分辨率
#define FRAME_HEIGHT 1280
#define FRAME_WIDTH 1920

// 0不保存，1保存成.h264文件
#define SAVE_FLAG 0
// 保存的目录，格式: H264_SAVE_PATH/Encode_xxxxx.h264
#define H264_SAVE_PATH "/home/wangsen/桌面/video/"
// 每隔 'SAVE_PICTURE_NUMBER' 张保存一次文件
#define SAVE_PICTURE_NUMBER 100