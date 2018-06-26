#include "client_server.h"

pthread_t client_thread;
int client_socket_fd;

void clean() {
    printf("%c%cCleaning\n", 8, 8);
    pthread_cancel(client_thread);
    shutdown(client_socket_fd, SHUT_RDWR);
    close(client_socket_fd);
}

void finish(int signal) {

    clean();
    printf("%c%cExiting \n", 8, 8);
    exit(EXIT_SUCCESS);

}

struct server_init_info {
    char* client_name;
    int port_num;
    char* socket_path;
};

void *run_client_name(void *args) {

    struct server_init_info* init_info = (struct server_init_info*) args;


    struct sockaddr_un server_socket;
    struct sockaddr_un sa;
    strcpy(server_socket.sun_path, init_info -> socket_path);
    server_socket.sun_family = AF_UNIX;

    printf(KBLU "Connecting to socket with SOCK_STREAM" RESET "\n");
    client_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connect(client_socket_fd, (struct sockaddr *)&server_socket, sizeof(server_socket)) == -1) {
        perror(KRED "Couldn't connect to socket" RESET );
        finish(0);
    }
    socklen_t *sa_len = (socklen_t*) malloc(sizeof(socklen_t));
    *sa_len = sizeof(sa);
    if(getsockname(client_socket_fd, (struct sockaddr *)&sa, sa_len) == -1) {
        perror(KRED "Couldn't read socket adress" RESET );
        finish(0);
    }
    printf(KBLU "Got socket with adress %s" RESET "\n", sa.sun_path);
    printf("Socket file descriptor = %d\n", client_socket_fd);

    struct message* buf = (struct message*) malloc(sizeof(struct message));
    buf -> msg_type = 0;
    strcpy(buf -> string_msg, init_info -> client_name);
    printf("Sending client name: %s\n", buf -> string_msg);
    send(client_socket_fd, buf, sizeof(struct message), 0);
    while(1) {
        if(recv(client_socket_fd, buf, sizeof(struct message), MSG_WAITALL) == 0) {
            continue;
        }
        printf("Recived msg with type: %d\n", buf -> msg_type);
        if(buf -> msg_type == 11) {
            finish(0);
        }
        else if(buf -> msg_type == 0) {
            float answer;
            if(strcmp(buf -> string_msg, "ADD") == 0)
                answer = (float)buf -> int_msg1 + (float)buf -> int_msg2;
            else if(strcmp(buf -> string_msg, "DIV") == 0)
                answer = (float)buf -> int_msg1 / (float)buf -> int_msg2;
            else if(strcmp(buf -> string_msg, "MUL") == 0)
                answer = (float)buf -> int_msg1 * (float)buf -> int_msg2;
            else if(strcmp(buf -> string_msg, "SUB") == 0)
                answer = (float)buf -> int_msg1 - (float)buf -> int_msg2;
            else
                answer = 0;
            buf -> msg_type = 1;
            buf -> answer = answer;
            send(client_socket_fd, buf, sizeof(struct message), 0);
        }
    }
    return NULL;
}

void *run_client(void *args) {

    struct server_init_info* init_info = (struct server_init_info*) args;


    struct sockaddr_in server_socket;
    struct sockaddr_in sa;
    server_socket.sin_addr.s_addr = INADDR_ANY;
    server_socket.sin_family = AF_INET;
    server_socket.sin_port = htons(init_info -> port_num);

    printf(KBLU "Connecting to socket with SOCK_STREAM" RESET "\n");
    client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(client_socket_fd, (struct sockaddr *)&server_socket, sizeof(server_socket)) == -1) {
        printf("Couldn't connect to socket\n");
        exit(0);
    }
    socklen_t *sa_len = (socklen_t*) malloc(sizeof(socklen_t));
    *sa_len = sizeof(sa);

    /*  getsockname() returns the current address to which the socket sockfd
       is bound, in the buffer pointed to by addr. */
    if(getsockname(client_socket_fd, (struct sockaddr *)&sa, sa_len) == -1) {
        printf("Couldn't read socket adress\n");
        exit(0);
    }
    printf(KBLU "Got socket with adress %s:%d" RESET "\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
    printf("Socket file descriptor = %d\n", client_socket_fd);

    struct message* buf = (struct message*) malloc(sizeof(struct message));
    buf -> msg_type = 0;
    strcpy(buf -> string_msg, init_info -> client_name);
    printf("Sending client name: %s\n", buf -> string_msg);
    send(client_socket_fd, buf, sizeof(struct message), 0);
    while(1) {
        if(recv(client_socket_fd, buf, sizeof(struct message), MSG_WAITALL) == 0) {
            continue;
        }
        printf("Recived msg with type: %d\n", buf -> msg_type);
        if(buf -> msg_type == 11) {
            finish(0);
        }
        else if(buf -> msg_type == 0) {
            float answer;
            if(strcmp(buf -> string_msg, "ADD") == 0)
                answer = (float)buf -> int_msg1 + (float)buf -> int_msg2;
            else if(strcmp(buf -> string_msg, "DIV") == 0)
                answer = (float)buf -> int_msg1 / (float)buf -> int_msg2;
            else if(strcmp(buf -> string_msg, "MUL") == 0)
                answer = (float)buf -> int_msg1 * (float)buf -> int_msg2;
            else if(strcmp(buf -> string_msg, "SUB") == 0)
                answer = (float)buf -> int_msg1 - (float)buf -> int_msg2;
            else
                answer = 0;
            buf -> msg_type = 1;
            buf -> answer = answer;
            send(client_socket_fd, buf, sizeof(struct message), 0);
        }
    }
    return NULL;
}

int main( int argc, char* argv[] ) {

    struct sigaction grand_finale;
    grand_finale.sa_handler = finish;
    grand_finale.sa_flags = 0;
    sigemptyset(&grand_finale.sa_mask);
    sigaction(SIGINT, &grand_finale, NULL);

    struct server_init_info server_info;
    server_info.client_name = argv[1];
    int server_type = strtol(argv[2], NULL, 10);
    if(server_type)
        server_info.port_num = strtol(argv[3], NULL, 10);
    else
        server_info.socket_path = argv[3];

    if(server_type){
        pthread_create(&client_thread, NULL, run_client, &server_info);
    }
    else {
        pthread_create(&client_thread, NULL, run_client_name, &server_info);
    }
    pthread_join(client_thread, NULL);


    clean();
    return 0;
}
