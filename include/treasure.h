#ifndef TREASURE_H
#define TREASURE_H

#define USERNAME_MAX 50
#define CLUE_MAX 256

typedef struct {
    int treasure_id;
    char username[USERNAME_MAX];
    float latitude;
    float longitude;
    char clue[CLUE_MAX];
    int value;
} treasure_t;

#endif
