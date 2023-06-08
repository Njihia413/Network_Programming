#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char response[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket binded to port %d.\n", PORT);

    // Start listening for client connections
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    int client_num = 0;  // variable to keep track of the client number

    while (1) {
        // Accept a new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Client %d connected.\n", client_num+1);

        // Create a child process to handle the client request
        if (fork() == 0) {
            close(server_fd);  // child process doesn't need the listening socket

            while (1) {
                // Receive data from the client
                valread = read(new_socket, buffer, 1024);
                if (valread == 0) {
                    printf("Client number %d disconnected.\n", client_num+1);
                    break;
                }
                printf("Data received from client %d: %s\n", client_num+1, buffer);

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
                send(new_socket, response, strlen(response), 0);
                printf("Response sent to client %d: %s\n", client_num+1, response);

                // Clear the buffer
                memset(buffer, 0, sizeof(buffer));
            }

            close(new_socket);  // close the client socket in the child process
            exit(EXIT_SUCCESS);
        }

        client_num++;
        close(new_socket);  // close the client socket in the parent process
    }

    // Close the listening socket
    close(server_fd);

    return 0;
}
