import os
import subprocess


def decode():
    input_path = '/home/wangsen/桌面/video/'
    output_path = '/home/wangsen/桌面/video/decode_video/'
    n = 0

    filename_list = os.listdir(input_path)
    filecount = []
    for item in filename_list:
        path = os.path.join(input_path, item)
        judge = os.path.isfile(path)
        if judge:
            if item.split(".")[1] == "h264":
                n += 1
                filecount.append(path)
                cmd = "ffmpeg -i " + path + ' ' + output_path + str(n) + '.avi'
                subprocess.Popen(cmd, shell=True)
                print(1)
        else:
            decode(path)


if __name__ == '__main__':
    decode()
