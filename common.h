/*
 * common.h
 *
 *  Created on: 2022-01-09 20:19
 *      Author: Jack Chen <redchenjs@live.com>
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include <stdint.h>
#include <netinet/in.h>

typedef struct {
    uint32_t flags;
    uint32_t room_id;
    in_addr_t addr_s;
    in_port_t port_s;
    in_addr_t addr_a;
    in_port_t port_a;
    in_addr_t addr_b;
    in_port_t port_b;
    uint32_t stream_a;
    uint32_t stream_b;
} room_info_t;

#endif /* INC_COMMON_H_ */
