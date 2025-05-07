#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include "treasure.h"

int create_hunt_directory(const char *hunt_id);

int create_symlink_for_log(const char *hunt_id);

int add_treasure_to_file(const char *hunt_id, const treasure_t *treasure);

int list_treasures(const char *hunt_id);

int view_treasure(const char *hunt_id, int treasure_id);

int log_operation(const char *hunt_id,int treasure_id, const char *operation);

int remove_treasure(const char *hunt_id, int treasure_id);

int remove_hunt(const char *hunt_id);

int list_hunts();
#endif
