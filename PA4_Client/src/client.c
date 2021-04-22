#include "client.h"

void createLogFile(void) {
    pid_t p = fork();
    if (p == 0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", ACCESSPERMS);
    logfp = fopen("log/log_client.txt", "w");
}


int main(int argc, char *argv[]) {

    // Process input arguments
    char *folderName = argv[1];
    int clients = atoi(argv[2]);
    char *serverIP = argv[3];
    char *serverPort = argv[4];

    // Create the log file
    createLogFile();

    // Spawn client processes
    for (int i = 0; i < clients; i++){
        pid_t client = fork();
        if (client == 0){
            char clientID = i + '1';
            execl("communicate", "communicate", &clientID, folderName, serverIP, serverPort, NULL);
        }
        else if (client < 0) { // error creating child
            fprintf(stderr, "ERROR: failed to fork while spawning client processes\n");
			exit(EXIT_FAILURE);
		}
    }

    // TO-DO: Wait for all client processes to terminate
    for (int i = 0; i < clients; i++){
		pid_t terminated_pid = wait(NULL);
		if (terminated_pid == -1){ // error waiting for child
			fprintf(stderr, "ERROR: failed to wait for client proccess\n");
			exit(EXIT_FAILURE);
		}
	}

    // close log file
    fclose(logfp);

    return EXIT_SUCCESS;

}
