#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_operations.h"
#include "menu.h"

static void print_usage(const char *prog_name) {
    printf("Usage:\n");
    printf("  %s add <hunt_id>\n", prog_name);
    printf("  %s list <hunt_id>\n", prog_name);
    printf("  %s view <hunt_id> <treasure_id>\n", prog_name);
    printf("  %s remove_treasure <hunt_id> <id>\n", prog_name);
    printf("  %s remove_hunt <hunt_id>\n", prog_name);

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *command = argv[1];
    const char *hunt_id = argv[2];

    if (create_hunt_directory(hunt_id) != 0) {
        fprintf(stderr, "Failed to create or access hunt directory.\n");
        return 1;
    }

    if (create_symlink_for_log(hunt_id) != 0) {
        fprintf(stderr, "Warning: Could not create symlink for log file.\n");
    }

    if (strcmp(command, "add") == 0) {

        treasure_t new_treasure = get_treasure_details();
        if (add_treasure_to_file(hunt_id, &new_treasure) == 0) {
            printf("Treasure added successfully.\n");
        } else {
            fprintf(stderr, "Error: Could not add treasure.\n");
            return 1;
        }

    } else if (strcmp(command, "list") == 0) {
        if (list_treasures(hunt_id) != 0) {
            fprintf(stderr, "Error: Could not list treasures.\n");
            return 1;
        }

    } else if (strcmp(command, "view") == 0) {
        if (argc < 4) {
            print_usage(argv[0]);
            return 1;
        }
        int treasure_id = atoi(argv[3]);
        if (view_treasure(hunt_id, treasure_id) != 0) {
            fprintf(stderr, "Error: Treasure with ID %d not found.\n", treasure_id);
            return 1;
        }

    }else if (strcmp(command, "remove_treasure") == 0){
        if (argc < 4) {
            print_usage(argv[0]);
            return 1;
        }
        int treasure_id = atoi(argv[3]);
        if (remove_treasure(hunt_id, treasure_id) != 0) {
            return 1;
        }

    }
    else if (strcmp(command, "remove_hunt") == 0){
        if (argc < 3) {
            print_usage(argv[0]);
            return 1;
        }
        if (remove_hunt(hunt_id) != 0) {
            return 1;
        }

    }
     else {
        
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
