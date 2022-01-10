/*
 * server.c
 *
 *  Created on: 2021-12-16 16:03
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "db.h"
#include "common.h"

#define PORT 1234
#define BLOG 5
#define DATA_MAX_LEN 512

static int add_client(room_info_t *room_info, in_addr_t addr, in_port_t port)
{
    if (room_info->addr_a == 0) {
        if ((addr == room_info->addr_b) && (port == room_info->port_b)) {
            return 0;
        }

        room_info->flags = 0;
        room_info->addr_a = addr;
        room_info->port_a = port;

        db_update_room(room_info);

        return 0;
    }

    if (room_info->addr_b == 0) {
        if ((addr == room_info->addr_a) && (port == room_info->port_a)) {
            return 0;
        }

        room_info->flags = 1;
        room_info->addr_b = addr;
        room_info->port_b = port;

        db_update_room(room_info);

        return 0;
    }

    return -1;
}

static int del_client(room_info_t *room_info, in_addr_t addr, in_port_t port)
{
    if ((addr == room_info->addr_a) && (port == room_info->port_a)) {
        room_info->flags = 0;
        room_info->addr_a = 0;
        room_info->port_a = 0;

        db_update_room(room_info);

        return 0;
    }

    if ((addr == room_info->addr_b) && (port == room_info->port_b)) {
        room_info->flags = 1;
        room_info->addr_b = 0;
        room_info->port_b = 0;

        db_update_room(room_info);

        return 0;
    }

    return -1;
}

void client_handle(int connfd, struct sockaddr_in *client)
{
    int recvlen;
    char data_sent[DATA_MAX_LEN] = {0};
    char data_recv[DATA_MAX_LEN] = {0};
    uint32_t room_id = 0;
    room_info_t *p_info = NULL;
    time_t tm;

    srand((unsigned)time(&tm));

    while (1) {
        if ((recvlen = recv(connfd, data_recv, sizeof(data_recv), 0)) == -1) {
            perror("failed to receive data!");
            exit(1);
        }

        room_id = *(uint32_t *)(data_recv + 1);
        p_info = db_search_room(room_id);

        if (p_info == NULL) {
            room_info_t room_info = {
                .flags = 0,
                .room_id = room_id,
                .addr_s = inet_addr("192.168.1.147"),
                .port_s = atoi("8554"),
                .addr_a = 0,
                .port_a = 0,
                .addr_b = 0,
                .port_b = 0,
                .stream_a = rand(),
                .stream_b = rand()
            };
            db_create_room(&room_info);
            p_info = db_search_room(room_id);
        }

        if (data_recv[0] == 'j') {
            if (add_client(p_info, client->sin_addr.s_addr, client->sin_port) != 0) {
                send(connfd, p_info, sizeof(room_info_t), 0);
                continue;
            }
        }

        break;
    }

    while (p_info->flags != 2) {
        free(p_info);
        p_info = db_search_room(room_id);
        if ((p_info->addr_a != 0) && (p_info->addr_b != 0)) {
            p_info->flags = 2;
        } else {
            p_info->flags = 1;
        }
        send(connfd, p_info, sizeof(room_info_t), 0);
        sleep(1);
    }

    while (1) {
        if ((recvlen = recv(connfd, data_recv, sizeof(data_recv), 0)) == -1) {
            perror("failed to receive data!");
            exit(1);
        }

        if (data_recv[0] == 'q') {
            p_info = db_search_room(room_id);
            del_client(p_info, client->sin_addr.s_addr, client->sin_port);
            free(p_info);
            p_info = NULL;

            p_info = db_search_room(room_id);
            if ((p_info->addr_a == 0) && (p_info->addr_b == 0)) {
                db_remove_room(p_info->room_id);
            }

            break;
        }
    }

    free(p_info);
    p_info = NULL;

    close(connfd);
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int sockfd, connfd;
    struct sockaddr_in server = {0};
    struct sockaddr_in client = {0};
    int sockopt = SO_REUSEADDR;
    int socklen = sizeof(client);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("failed to create socket!");
        exit(1);
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("failed to bind port!");
        exit(1);
    }

    if (listen(sockfd, BLOG) == -1) {
        perror("failed to listen port!");
        exit(1);
    }

    while (1) {
        if ((connfd = accept(sockfd, (struct sockaddr *)&client, &socklen)) == -1) {
            perror("failed to accept connection!");
            exit(1);
        }

        printf("received a connection from %s:%d\n", inet_ntoa(client.sin_addr), client.sin_port);

        if ((pid = fork()) > 0) {
            close(connfd);
            continue;
        } else if (pid == 0) {
            close(sockfd);
            client_handle(connfd, &client);
            exit(0);
        } else {
            perror("failed to fork new process!");
            exit(0);
        }

        sleep(1);
    }

    close(sockfd);

    return 0;
}
