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
    fprintf(stderr, "Usage: %s <listen-port>\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    char *end = NULL;
    long port_long = strtol(argv[1], &end, 10);
    if (!end || *end != '\0' || port_long <= 0 || port_long > 65535) {
        fprintf(stderr, "Invalid port: %s\n", argv[1]);
        return 1;
    }

    // TODO: Create a TCP listen socket (AF_INET, SOCK_STREAM).
    int listen_id = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_id < 0){
        perror("socket");
        return 1;
    }
    // TODO: Set SO_REUSEADDR on the listen socket.
    int yes = 1;
    if(setsockopt(listen_id,SOL_SOCKET,SO_REUSEADDR,&yes, sizeof(yes)) < 0 ){
        perror("setsockopt");
        close(listen_id);
        return 1;
    }
    
    // TODO: Bind the socket to INADDR_ANY and the given port.
    // TODO: Listen with a small backlog (e.g., 5-10).

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons((uint16_t)port_long);

    if (bind(listen_id,(struct sockaddr *)&server_address,sizeof(server_address)) < 0) {
        perror("bind");
        close(listen_id);
        return 1;
    }

    // TODO: Accept clients in an infinite loop.
    //   - For each client, read in chunks until EOF.
    //   - For each chunk, write those *exact bytes* to stdout.
    //     Use write(STDOUT_FILENO, ...) in a loop to handle partial writes.
    //   - Do NOT use printf/fputs or add separators/newlines/prefixes.
    //   - The test harness compares server stdout byte-for-byte with client input.
    // TODO: Handle EINTR and other error cases as specified.
        if (listen(listen_id, 10) < 0) {
        perror("listen");
        close(listen_id);
        return 1;
    }

    // TODO: Close the listen socket before exiting.
        enum { BUFFER_SIZE = 4096 };
        char buffer[BUFFER_SIZE];

    for (;;) {
        int client_id = accept(listen_id, NULL, NULL);
        if (client_id < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            break; 
        }

        for (;;) {
            ssize_t nread = read(client_id, buffer, sizeof(buffer));
            if (nread == 0) {
                break;
            }
            if (nread < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("read");
                break; 
            }

            ssize_t total_written = 0;
            while (total_written < nread) {
                ssize_t nwritten = write(STDOUT_FILENO,
                                         buffer + total_written,
                                         (size_t)(nread - total_written));
                if (nwritten < 0) {
                    if (errno == EINTR) {
                        continue;
                    }
                    perror("write");
                    
                    total_written = nread; 
                    break;
                }
                total_written += nwritten;
            }
        }

        close(client_id);
    }
        close(listen_id);

    return 0;
}
