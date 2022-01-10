/*
 * db.h
 *
 *  Created on: 2021-12-17 18:31
 *      Author: Jack Chen <redchenjs@live.com>
 */

#ifndef INC_DB_H_
#define INC_DB_H_

#include <stdint.h>

#include "common.h"

extern room_info_t *db_search_room(uint32_t room_id);
extern void db_create_room(const room_info_t *room_info);
extern void db_update_room(const room_info_t *room_info);
extern void db_remove_room(uint32_t room_id);

#endif /* INC_DB_H_ */
