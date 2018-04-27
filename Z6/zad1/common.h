#ifndef COMMON_H
#define COMMON_H
#include <stdlib.h>

#define MAX_MSG_SIZE 4000
#define MAX_CLIENTS  10

#define PROJ_ID 8

#define HOME getenv("HOME")
//The resulting value is the same for all pathnames that name the same
//file, when the same value of proj_id is used.
#define SERVER_KEY ftok(HOME, PROJ_ID)

enum mType{
    CLIENTSETUP = 1, HELLO = 2, MIRROR = 3, CALC = 4, TIME = 5, END = 6,
} mType;


typedef struct msg{
    long mType;
    char messageContent[MAX_MSG_SIZE];
    pid_t receivedFrom;
} msg;

const size_t MESSAGE_SIZE = sizeof(msg)-sizeof(long);

#endif
