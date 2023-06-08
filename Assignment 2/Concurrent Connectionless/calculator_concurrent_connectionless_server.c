#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char response[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
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

    int client_num = 0;  // variable to keep track of the client number

    while (1) {
        printf("Waiting for data...\n");

        while (1) {
            // Receive data from the client
            valread = recvfrom(server_fd, buffer, 1024, 0, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            printf("Data received from client %d: %s\n", client_num+1, buffer);

            // Create a child process to handle the client request
            if (fork() == 0) {
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
                sendto(server_fd, response, strlen(response), 0, (struct sockaddr *)&address, addrlen);
                printf("Response sent to client %d: %s\n", client_num+1, response);

                // Clear the buffer
                memset(buffer, 0, sizeof(buffer));

                // Close the child process
                exit(EXIT_SUCCESS);
            }

            client_num++;  // increment the client number after receiving data from the client
        }

    }

    // Close the socket
    close(server_fd);

    return 0;
}
