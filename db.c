/*
 * db.c
 *
 *  Created on: 2021-12-16 16:10
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#include "common.h"

#define QUERY_MAX_LEN 512

static const char *host   = "127.0.0.1";
static const char *user   = "roomadmin";
static const char *passwd = "roompasswd";
static const char *db     = "room_db";
static unsigned int port  = 3306;

room_info_t *db_search_room(uint32_t room_id)
{
    room_info_t *room_info = NULL;
    MYSQL mysql, *conn = NULL;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    char query[QUERY_MAX_LEN] = {0};
    snprintf(query, sizeof(query), "SELECT * FROM `room_tbl` WHERE BINARY `room_id`='%d'", room_id);
    printf("%s\n", query);

    mysql_init(&mysql);

    if ((conn = mysql_real_connect(&mysql, host, user, passwd, db, port, NULL, 0)) == NULL) {
        perror("failed to connect to mysql!");
        goto exit_search;
    }

    if (mysql_query(&mysql, query) != 0) {
        perror("failed to query the database!");
        goto exit_search;
    } else {
        if ((result = mysql_store_result(&mysql)) == NULL) {
            perror("failed to store result!");
            goto exit_search;
        } else {
            if ((row = mysql_fetch_row(result)) != NULL) {
                uint32_t addr_s = 0;
                uint32_t addr_a = 0;
                uint32_t addr_b = 0;
                sscanf(row[1], "%08x", &addr_s);
                sscanf(row[3], "%08x", &addr_a);
                sscanf(row[5], "%08x", &addr_b);
                room_info_t info = {
                    .room_id = atoi(row[0]),
                    .addr_s = addr_s,
                    .port_s = atoi(row[2]),
                    .addr_a = addr_a,
                    .port_a = atoi(row[4]),
                    .addr_b = addr_b,
                    .port_b = atoi(row[6]),
                    .stream_a = atoi(row[7]),
                    .stream_b = atoi(row[8])
                };

                if ((room_info = calloc(1, sizeof(room_info_t))) != NULL) {
                    memcpy(room_info, &info, sizeof(room_info_t));
                }
            }
        }
    }

exit_search:
    mysql_free_result(result);
    mysql_close(conn);

    return room_info;
}

void db_create_room(const room_info_t *room_info)
{
    MYSQL mysql, *conn = NULL;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    char query[QUERY_MAX_LEN] = {0};
    snprintf(query, sizeof(query), "SELECT * FROM `room_tbl` WHERE BINARY `room_id`='%d'", room_info->room_id);

    mysql_init(&mysql);

    if ((conn = mysql_real_connect(&mysql, host, user, passwd, db, port, NULL, 0)) == NULL) {
        perror("failed to connect to mysql!");
        goto exit_create;
    }

    if (mysql_query(&mysql, query) != 0) {
        perror("failed to query the database!");
        goto exit_create;
    } else {
        if ((result = mysql_store_result(&mysql)) == NULL) {
            perror("failed to store result!");
            goto exit_create;
        } else {
            if ((row = mysql_fetch_row(result)) != NULL) {
                perror("item already exist!");
                goto exit_create;
            } else {
                snprintf(query, sizeof(query), "INSERT INTO `room_tbl` (`room_id`, `addr_s`, `port_s`, `stream_a`, `addr_a`, `port_a`, `stream_b`, `addr_b`, `port_b`) VALUES ('%d', '%08x', '%d', '%d', '%08x', '%d', '%d', '%08x', '%d')",
                                                room_info->room_id, room_info->addr_s, room_info->port_s, room_info->stream_a, room_info->addr_a, room_info->port_a, room_info->stream_b, room_info->addr_b, room_info->port_b);
                printf("%s\n", query);

                if (mysql_query(&mysql, query) != 0) {
                    perror("failed to query the database!");
                    goto exit_create;
                }
            }
        }
    }

exit_create:
    mysql_free_result(result);
    mysql_close(conn);
}

void db_update_room(const room_info_t *room_info)
{
    MYSQL mysql, *conn = NULL;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    char query[QUERY_MAX_LEN] = {0};
    snprintf(query, sizeof(query), "SELECT * FROM `room_tbl` WHERE BINARY `room_id`='%d'", room_info->room_id);

    mysql_init(&mysql);

    if ((conn = mysql_real_connect(&mysql, host, user, passwd, db, port, NULL, 0)) == NULL) {
        perror("failed to connect to mysql!");
        goto exit_update;
    }

    if (mysql_query(&mysql, query) != 0) {
        perror("failed to query the database!");
        goto exit_update;
    } else {
        if ((result = mysql_store_result(&mysql)) == NULL) {
            perror("failed to store result!");
            goto exit_update;
        } else {
            if ((row = mysql_fetch_row(result)) != NULL) {
                if (room_info->flags == 0) {
                    snprintf(query, sizeof(query), "UPDATE `room_tbl` SET `addr_a`='%08x', `port_a`='%d' WHERE BINARY `room_id`='%d'",
                                                    room_info->addr_a, room_info->port_a, room_info->room_id);
                } else {
                    snprintf(query, sizeof(query), "UPDATE `room_tbl` SET `addr_b`='%08x', `port_b`='%d' WHERE BINARY `room_id`='%d'",
                                                    room_info->addr_b, room_info->port_b, room_info->room_id);
                }
                printf("%s\n", query);

                if (mysql_query(&mysql, query) != 0) {
                    perror("failed to query the database!");
                    goto exit_update;
                }
            } else {
                perror("item does not exist!");
                goto exit_update;
            }
        }
    }

exit_update:
    mysql_free_result(result);
    mysql_close(conn);
}

void db_remove_room(uint32_t room_id)
{
    MYSQL mysql, *conn = NULL;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;

    char query[QUERY_MAX_LEN] = {0};
    snprintf(query, sizeof(query), "SELECT * FROM `room_tbl` WHERE BINARY `room_id`='%d'", room_id);

    mysql_init(&mysql);

    if ((conn = mysql_real_connect(&mysql, host, user, passwd, db, port, NULL, 0)) == NULL) {
        perror("failed to connect to mysql!");
        goto exit_remove;
    }

    if (mysql_query(&mysql, query) != 0) {
        perror("failed to query the database!");
        goto exit_remove;
    } else {
        if ((result = mysql_store_result(&mysql)) == NULL) {
            perror("failed to store result!");
            goto exit_remove;
        } else {
            if ((row = mysql_fetch_row(result)) != NULL) {
                snprintf(query, sizeof(query), "DELETE FROM `room_tbl` WHERE BINARY `room_id`='%d'", room_id);
                printf("%s\n", query);

                if (mysql_query(&mysql, query) != 0) {
                    perror("failed to query the database!");
                    goto exit_remove;
                }
            } else {
                perror("item does not exist!");
                goto exit_remove;
            }
        }
    }

exit_remove:
    mysql_free_result(result);
    mysql_close(conn);
}
