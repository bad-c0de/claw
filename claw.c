#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int scanner (char *hostname, char *port);

int main (int argc, char *argv[])
{
    if (argc < 3) { fprintf(stderr, "[-] Usage: claw hostname port(s)\n"); return 1; }
    
    for (int i = 2; i < argc; i++)
    {
        scanner(argv[1], argv[i]);
    }
    
    return 0;
}

int scanner (char *hostname, char *port)
{
    int status, socket_fd;
    char request[256], response[4096];
    struct addrinfo hints, *res;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    printf("Trying port: %s\n", port);

    status = getaddrinfo(hostname, port, &hints, &res);
    if (status < 0) { fprintf(stderr, "getaddrinfo() error: %s\n", gai_strerror(status)); return 1; }
    
    socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (socket_fd < 0) { perror("[-] Failed to create Socket"); return 1; }

    status = connect(socket_fd, res->ai_addr, res->ai_addrlen);
    if (status < 0) { perror("[-] Failed to Connect"); return 1; }
    
    if (strcmp(port, "80") == 0 || strcmp(port, "443") == 0)
    {
        sprintf(request, "GET / HTTP/1.1\r\nHost: %s\r\nUser-Agent: claw/0.1\r\nAccept: */*\r\n\r\n", hostname);
    }

    else
    {
        strcat(request, "garbage\r\n\r\n");
    }
    
    send(socket_fd, request, strlen(request), 0);

    memset(response, 0, sizeof(response));
    recv(socket_fd, response, sizeof(response), 0);

    printf("%s\n", response);

    return 0;
}
