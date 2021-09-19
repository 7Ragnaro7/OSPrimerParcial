#include "msg.h"

void error_exit(int msg_id, sqlite3* db){
    sqlite3_close(db);
    int msg_rc = msgctl(msg_id, IPC_RMID, 0);
    if (msg_rc == -1) {
        printf("msgctl(IPC_RMID) failed\n");
    }
    else{
        printf("mailbox closed\n");
    }
    exit(-1);
}


void msg_receiver(int msg_id){
    msgtime horarios;
    long int msgtype;
    FILE* f;
    char * errmsg;
    char uuid[33];
    struct tm * start_time;
    struct tm * end_time;
    sqlite3* db;
    int msg_rc;
    sqlite3_open(DB_PATH, &db);
    int rc = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS PROCESSES(UUID varchar(35), START varchar(35), END varchar(35));", NULL, NULL, &errmsg);
    if(rc != SQLITE_OK){
        printf("Error at creating table: %s\n", errmsg);
        sqlite3_free(errmsg);
        error_exit(msg_id, db);
    }
    
    while(1){
        msg_rc = msgrcv(msg_id, (void *)&horarios, sizeof(msgtime), msgtype, 0);
        if(msg_rc == -1){
            printf("msgrcv fail\n");
            error_exit(msg_id, db);
        }
        //Generar uuid
        f = popen("uuidgen -r", "r");
        fgets(uuid, sizeof(uuid), f);
        pclose(f);
        
        printf("%s\n", uuid);
        start_time = localtime(&horarios.start_time);
        end_time = localtime(&horarios.end_time);
        //Quitar /n
        char* str_start_time = asctime(start_time);
        str_start_time[strlen(str_start_time) - 1] = 0;

        char* str_end_time = asctime(end_time);
        str_end_time[strlen(str_end_time) - 1] = 0;
        
        char sqlinsert[150];
        snprintf(sqlinsert, 150, "INSERT INTO PROCESSES VALUES('%s', '%s', '%s');", uuid, str_start_time, str_end_time);
        rc = sqlite3_exec(db, sqlinsert, NULL, NULL, &errmsg);
        if(rc != SQLITE_OK){
            printf("Error: %s\n", errmsg);
            sqlite3_free(errmsg);
            error_exit(msg_id, db);
        }
        // Mensajes Debug
        printf("%s\n", sqlinsert);
        printf ("Inicio: %s", asctime(start_time));
        printf ("Final: %s", asctime(end_time));
    }
}

void msg_sender(int msg_id, int conn_fd){
    msgtime horarios;
    time(&horarios.start_time);
    request_handle(conn_fd);
    close_or_die(conn_fd);
    time(&horarios.end_time);
    int msg_rc = msgsnd(msg_id, (void *)&horarios, sizeof(msgtime), 0);
    if(msg_rc == -1) {
        printf("msgsnd fail\n");
        exit(-1);
    }
    //Mensajes debug
    printf("hijo %d ,,, padre %d\n", getpid(), getppid());
    exit(0);
}