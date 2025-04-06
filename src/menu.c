#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "menu.h"

treasure_t get_treasure_details() {
    treasure_t treasure;
    
    printf("Enter Treasure ID (integer): ");
    scanf("%d", &treasure.treasure_id);
    
    printf("Enter User Name: ");
    scanf("%s", treasure.username);
    
    printf("Enter Latitude: ");
    scanf("%f", &treasure.latitude);
    
    printf("Enter Longitude: ");
    scanf("%f", &treasure.longitude);
    
    printf("Enter Clue: ");
    getchar();
    fgets(treasure.clue, CLUE_MAX, stdin);

    size_t len = strlen(treasure.clue);
    if (len > 0 && treasure.clue[len - 1] == '\n') {
        treasure.clue[len - 1] = '\0';
    }
    
    printf("Enter Value");
    scanf("%d", &treasure.value);
    
    return treasure;
}
