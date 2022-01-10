/*
 * client.c
 *
 *  Created on: 2021-12-16 16:11
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "common.h"

#define PORT 1234
#define DATA_MAX_LEN 512

int main(int argc, char *argv[])
{
    int role;
    int sockfd;
    int recvlen;
    struct hostent *host;
    struct sockaddr_in server = {0};
    int state = 0;
    char data_send[DATA_MAX_LEN] = {0};
    char data_recv[DATA_MAX_LEN] = {0};
    room_info_t *p_info = NULL;
    pid_t pid_ffmpeg, pid_ffplay;

    if (argc != 2) {
        printf("Usage: %s ip\n", argv[0]);
        exit(1);
    }

    if ((host = gethostbyname(argv[1])) == (void *)-1) {
        perror("failed to parse hostname!");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("failed to create socket!");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *)host->h_addr_list[0]);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("failed to connect to host!");
        exit(1);
    }

    while (1) {
        data_send[0] = 'j';
        printf(">> 输入房间号: ");
        scanf("%ld", (uint32_t *)(data_send + 1));
        send(sockfd, data_send, strlen(data_send), 0);

        if ((recvlen = recv(sockfd, data_recv, sizeof(data_recv), 0)) == -1) {
            perror("failed to receive data!");
            exit(1);
        }

        p_info = (room_info_t *)data_recv;
        if (p_info->flags == 0) {
            printf("<= 无法加入会议室%d, 请重试\n", p_info->room_id);
        } else if (p_info->flags == 1) {
            role = 0;

            printf("<= 进入会议室%d...\n", p_info->room_id);
            printf("== 等待对方加入...\n");

            while (1) {
                if ((recvlen = recv(sockfd, data_recv, sizeof(data_recv), 0)) == -1) {
                    perror("failed to receive data!");
                    exit(1);
                }

                if (p_info->flags == 2) {
                    printf("== 连接成功\n");
                    break;
                }
            }

            break;
        } else if (p_info->flags == 2) {
            role = 1;
            printf("== 连接成功\n");
            break;
        }
    }

    if ((pid_ffmpeg = fork()) == 0) {
        char url[DATA_MAX_LEN] = {0};
        struct in_addr addr_s = {.s_addr = p_info->addr_s};
        snprintf(url, sizeof(url), "rtsp://%s:%d/%d", inet_ntoa(addr_s), p_info->port_s, role ? p_info->stream_a : p_info->stream_b);
        execlp("ffmpeg", "ffmpeg", "-vaapi_device", "/dev/dri/renderD128", "-f", "v4l2", "-input_format", "mjpeg", "-video_size", "1280x720", "-i", "/dev/video0",
                "-f", "alsa", "-i", "pulse", "-vf", "format=nv12|vaapi,hwupload", "-c:v", "hevc_vaapi", "-c:a", "aac", "-f", "rtsp", url, NULL);
    }

    sleep(3);

    if ((pid_ffplay = fork()) == 0) {
        char url[DATA_MAX_LEN] = {0};
        struct in_addr addr_s = {.s_addr = p_info->addr_s};
        snprintf(url, sizeof(url), "rtsp://%s:%d/%d", inet_ntoa(addr_s), p_info->port_s, role ? p_info->stream_b : p_info->stream_a);
        execlp("ffplay", "ffplay", "-fflags", "nobuffer", "-flags", "low_delay", "-framedrop", "-strict", "experimental", url, NULL);
    }

    while (1) {
        printf(">> 输入命令(q键退出): ");
        scanf("%s", data_send);

        if (data_send[0] == 'q') {
            memcpy(data_send + 1, &(p_info->room_id), 4);
            data_send[5] = 0;
            send(sockfd, data_send, strlen(data_send), 0);

            break;
        }
    }

    kill(pid_ffmpeg, SIGKILL);
    kill(pid_ffplay, SIGKILL);

    close(sockfd);

    return 0;
}
