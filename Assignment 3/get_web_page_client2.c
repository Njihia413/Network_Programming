#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAXBUF 8192

int main(int argc, char *argv[]) {
    char buffer[MAXBUF];
    int sockfd, port, bytes, n;
    struct sockaddr_in server_addr;
    struct hostent *server;
    SSL_CTX *ctx;
    SSL *ssl;

    if (argc < 3) {
        fprintf(stderr, "usage %s <hostname> <port>\n", argv[0]);
        exit(0);
    }

    port = atoi(argv[2]);

    /* Create socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    /* Get server IP address */
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"Error, no such host\n");
        exit(0);
    }

    /* Set server address struct */
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&server_addr.sin_addr.s_addr,
         server->h_length);
    server_addr.sin_port = htons(port);

    /* Connect to server */
    if (connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("Error connecting");
        exit(1);
    }

    /* Create SSL context */
    SSL_load_error_strings();
    SSL_library_init();
    ctx = SSL_CTX_new(TLS_client_method());

    /* Create SSL connection */
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    /* Connect using SSL */
    if (SSL_connect(ssl) < 0) {
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    /* Send HTTP GET request */
    char request[MAXBUF];
    sprintf(request, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", argv[1]);
    SSL_write(ssl, request, strlen(request));

    /* Receive and print server response */
    bytes = SSL_read(ssl, buffer, MAXBUF);
    if (bytes <= 0) {
        fprintf(stderr, "Error receiving server response\n");
        exit(1);
    }

    /* Parse and print HTTP code and status */
    int code;
    char status[MAXBUF];
    sscanf(buffer, "HTTP/1.%*d %d %[^\r\n]", &code, status);
    printf("HTTP code: %d\n", code);
    printf("HTTP status: %s\n", status);

    
    /* Receive and print server response */
    int total_bytes = 0;
    while (1) {
        bytes = SSL_read(ssl, buffer, MAXBUF);
        if (bytes <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes, stdout);
        total_bytes += bytes;
    }

    /* Print total number of bytes received */
    fprintf(stderr, "Received %d bytes\n", total_bytes);


    /* Cleanup */
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    close(sockfd);

    return 0;
}
