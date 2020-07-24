import av


def h264_to_jpg():
    input_file_name = "/home/wangsen/桌面/encode_output.h264"
    container = av.open(input_file_name)
    print("container:", container)
    print("container.streams:", container.streams)
    print("container.format:", container.format)

    for frame in container.decode(video=0):
        print("process frame: %04d (width: %d, height: %d)" %
              (frame.index, frame.width, frame.height))
        frame.to_image().save("frame-%04d.jpg" % frame.index)


if __name__ == "__main__":
    h264_to_jpg()
