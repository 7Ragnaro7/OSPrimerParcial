#ifndef __MSG__
#define __MSG__

#include <stdio.h>
#include <time.h>
#include <sys/msg.h>
#include "sqlite3/sqlite3.h"
#include "request.h"
#include "io_helper.h"

#define TIME_SIZE 27
#define PATH_NAME  "msg.h"
#define PROJECT_ID 70421
#define DB_PATH "/home/ragnaro7/Documentos/SistemasOperativos/Proyecto/sqlite3/dbProject.db"

typedef struct msgtime {
    time_t start_time;
    time_t end_time;
} msgtime;

void msg_receiver(int msg_id);

void msg_sender(int msg_id, int conn_fd);

#endif // __MSG__