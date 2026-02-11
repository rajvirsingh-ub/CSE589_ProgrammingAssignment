#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <server-ip> <server-port>\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    char *end = NULL;
    long port_long = strtol(argv[2], &end, 10);
    if (!end || *end != '\0' || port_long <= 0 || port_long > 65535) {
        fprintf(stderr, "Invalid port: %s\n", argv[2]);
        return 1;
    }

    // TODO: Create a TCP socket (AF_INET, SOCK_STREAM).
    int client_id = socket(AF_INET, SOCK_STREAM,0);
    if(client_id < 0){
        perror("socket");return 1;
    }
    // TODO: Populate sockaddr_in with server IP/port.
    struct sockaddr_in server_address;
    memset(&server_address,0 , sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons((uint16_t)port_long);
    int rc = inet_pton(AF_INET, argv[1], &server_address.sin_addr);
    if(rc==0){
        fprintf(stderr,"Invalid IP address : %s\n", argv[1]);
        close(client_id);return 1;
    }else if(rc <0 ){
        perror("inet_pton");
        close(client_id);return 1;
    }
    // TODO: Connect to the server.
    if(connect(client_id,(struct sockaddr *)&server_address,sizeof(server_address)) < 0){
        perror("connect");
        close(client_id);
        return 1;
    }

    // TODO: Read from stdin in a loop (read()) and send in chunks.
    enum { BUFFER_SIZE = 4096} ;
    char buffer[BUFFER_SIZE];

    for (;;) {
        ssize_t num_read = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (num_read == 0) {
            break;
        }
        if (num_read < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("read");
            close(client_id);
            return 1;
        }

        ssize_t total_sent = 0;
        while (total_sent < num_read) {
            ssize_t num_sent = send(client_id,
                                 buffer + total_sent,
                                 (size_t)(num_read - total_sent),
                                 0);
            if (num_sent < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("send");
                close(client_id);
                return 1;
            }
            total_sent += num_sent;
        }
    } 
    // TODO: For each chunk, send the *exact bytes* you read.
    //   - Use send()/write() in a loop to handle partial sends.
    //   - Do NOT add newlines, prefixes, or other formatting.
    // The test harness compares server stdout byte-for-byte with stdin input.

    // TODO: Close the socket before exiting.

    if(close(client_id) <0 ){
        perror("close");
        return 1;
    }
    return 0;
}
