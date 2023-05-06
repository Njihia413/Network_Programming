#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 4096

int main() {
    char server_address[100];
    int server_port = 80;
    char request[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    int bytes_received;
    int socket_fd;
    struct sockaddr_in server;

    printf("Enter the web server URL (e.g. www.example.com): ");
    fgets(server_address, 100, stdin);
    strtok(server_address, "\n");

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);

    struct hostent *he = gethostbyname(server_address);
    if (he == NULL) {
        printf("Error: Could not resolve host %s\n", server_address);
        return 1;
    }

    memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("connect");
        return 1;
    }

    sprintf(request, "GET / HTTP/1.1\r\nHost: %s\r\n\r\n", server_address);

    if (send(socket_fd, request, strlen(request), 0) < 0) {
        perror("send");
        return 1;
    }

    while ((bytes_received = recv(socket_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, sizeof(char), bytes_received, stdout);
    }

    if (bytes_received < 0) {
        perror("recv");
        return 1;
    }

    close(socket_fd);

    return 0;
}
