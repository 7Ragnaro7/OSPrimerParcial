#include "msg.h"

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	pid_t process_worker;
	pid_t process_manager;
	
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
		msg_receiver(msg_id);
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
				msg_sender(msg_id, conn_fd);
			}
    	}
	}
    return 0;
}


    


 
