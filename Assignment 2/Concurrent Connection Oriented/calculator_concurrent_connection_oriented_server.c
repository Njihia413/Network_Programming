#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 10

void handle_client(int client_socket, int client_number) {
    char buffer[1024] = {0};
    char response[1024] = {0};

    printf("Client %d connected.\n", client_number);

    while (1) {
        // Receive data from the client
        int valread = read(client_socket, buffer, 1024);
        if (valread == 0) {
            printf("Client %d disconnected.\n", client_number);
            break;
        }
        printf("Data received from client %d: %s\n", client_number, buffer);

        // Parse the input from the client
        int num1, num2;
        char op;
        sscanf(buffer, "%d %c %d", &num1, &op, &num2);

        // Perform the requested operation
        int result;
        switch (op) {
            case '+':
                result = num1 + num2;
                break;
            case '-':
                result = num1 - num2;
                break;
            case '*':
                result = num1 * num2;
                break;
            case '/':
                result = num1 / num2;
                break;
            default:
                result = 0;
        }

        // Construct the response string
        snprintf(response, 1024, "%d\n", result);

        // Send the response back to the client
        send(client_socket, response, strlen(response), 0);
        printf("Response sent to client %d: %s\n", client_number, response);

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));
    }

    close(client_socket);
}

int main(int argc, char const *argv[]) {
    int master_socket, slave_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int client_count = 0;

    // Creating socket file descriptor
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Master Socket created successfully.\n");

    // Forcefully attaching socket to the port 8080
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port 8080
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Master Socket binded to port %d.\n", PORT);

    // Start listening for client connections
    if (listen(master_socket, MAX_CLIENTS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // Accept a new client connection
        if ((slave_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // Increment client count and create a slave process to handle the client request
        client_count++;
        if (fork() == 0) {
            close(master_socket);  // slave process doesn't need the listening socket

            handle_client(slave_socket, client_count);

            exit(EXIT_SUCCESS);
        }

        close(slave_socket);  // close the slave socket in the parent process
    }

    // Close the listening socket
    close(master_socket);

    return 0;
}
