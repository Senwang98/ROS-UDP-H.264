#include <stdio.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{
    int port = 8888;
    
    int udp_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket_fd < 0)
    {
        printf("Create socket failed\n");
    }

    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(udp_socket_fd, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if (ret < 0)
    {
        printf("bind fail \n");
        close(udp_socket_fd);
    }
    else
    {
        printf("recv ready \n");
    }
    

    struct sockaddr_in src_addr = {0};  // 用来存放对方的IP地址信息
    unsigned int len = sizeof(src_addr);
    unsigned char buf[20000] = {0};


    while (1)
    {
        ret = recvfrom(udp_socket_fd, buf, sizeof(buf), 0, (struct sockaddr*)&src_addr, &len);
        if (ret == -1)
            break;
        
        printf("[%s: %d] \n", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port));  //打印消息发送方的ip与端口号

        for (int i = 0; i < 100; i++)
        {   
            printf("0x%X ", buf[i]);
        }
        printf("\n\n");

		// printf("buf= %s\n", buf);

		memset(buf, 0, sizeof(buf));//清空存留消息
    }
    
    close(udp_socket_fd);
}

