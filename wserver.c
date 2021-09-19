#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include <time.h>
#include "msg.h"
#include <sys/msg.h>
#include "sqlite3.h"



char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
	long int msgtype;
    char *root_dir = default_root;
    int port = 10000;
	pid_t process_worker;
	pid_t process_manager;
    msgtime horarios;
	
    while ((c = getopt(argc, argv, "d:p:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);
    
	//Abrir el mailbox
	key_t key = ftok(PATH_NAME, PROJECT_ID);
	int msg_id = msgget(key, 0666 | IPC_CREAT);
	if(msg_id == -1){
		printf("msgget error\n");
		exit(-1);
	}
	// Crear un nuevo fork que es el que va a escuchar peticiones 
	// si es el padre > 0 si no hace el proceso de escuchar
	process_manager = fork_or_die();
	if(process_manager == 0){
		FILE* f;
		int msg_rc;
		char * errmsg;
		char uuid[33];
		struct tm * start_time;
		struct tm * end_time;
		sqlite3* db;
		sqlite3_open("/home/ragnaro7/Documentos/SistemasOperativos/Proyecto/dbProject.db", &db);
		const char sqlstmt[] = "CREATE TABLE IF NOT EXISTS request("
							"uuid varchar(35), "
							"start varchar(35),"
							"end varchar(35)";
		int rc = sqlite3_exec(db, ("CREATE TABLE IF NOT EXISTS PROCESSES(UUID varchar(35), START varchar(35), END varchar(35));"), NULL, NULL, &errmsg);
		if(rc != SQLITE_OK){
			printf("Error at creating table: %s\n", errmsg);
			sqlite3_close(db);
			msg_rc = msgctl(msg_id, IPC_RMID, 0);
			if (msg_rc == -1) {
				printf("msgctl(IPC_RMID) failed\n");
			}
			printf("mailbox closed\n");
			exit(-1);
		}
		
		while(1){
			msg_rc = msgrcv(msg_id, (void *)&horarios, sizeof(msgtime), msgtype, 0);
			if(msg_rc == -1){
				printf("msgrcv fail\n");
				msg_rc = msgctl(msg_id, IPC_RMID, 0); 
				if (msg_rc == -1) {
					printf("msgctl(IPC_RMID) failed\n");
				}
				sqlite3_close(db);
				printf("mailbox closed\n");
				exit(-1);
			}
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
			printf("%s", sqlinsert);
			rc = sqlite3_exec(db, sqlinsert, NULL, NULL, &errmsg);
			if(rc != SQLITE_OK){
				printf("Error: %s\n", errmsg);
			}
			printf ("Inicio: %s", asctime(start_time));
			printf ("Final: %s", asctime(end_time));
		}
	}
	else{
		int listen_fd = open_listen_fd_or_die(port);
    	while (1){
			struct sockaddr_in client_addr;
			int client_len = sizeof(client_addr);
			int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
			process_worker = fork_or_die();
			//child process
			if(process_worker == 0){
				//time_t start_rawtime;
				time(&horarios.start_time);
				request_handle(conn_fd);
				close_or_die(conn_fd);
				//time_t end_rawtime;
				time(&horarios.end_time);
				int msg_rc = msgsnd(msg_id, (void *)&horarios, sizeof(msgtime), 0);
				if(msg_rc == -1) {
					printf("msgsnd fail\n");
					exit(-1);
				}
				printf("hijo %d ,,, padre %d\n", getpid(), getppid());
				exit(0);
			}
    	}
	}
    return 0;
}


    


 
