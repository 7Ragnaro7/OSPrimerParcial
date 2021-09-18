#include <time.h>

#define TIME_SIZE 27
#define PATH_NAME  "msg.h"
#define PROJECT_ID 70421

typedef struct msgtime {
    time_t start_time;
    time_t end_time;
} msgtime;