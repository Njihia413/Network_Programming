#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int master_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char response[1024] = {0};

    // Creating socket file descriptor
    if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
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

    while (1) {
        printf("Waiting for data...\n");

        while (1) {
            // Receive data from the client
            valread = recvfrom(master_socket, buffer, 1024, 0, (struct sockaddr *)&address, (socklen_t*)&addrlen);
            printf("Data received from client: %s\n", buffer);

            // Create a slave process to handle the client request
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
                sendto(master_socket, response, strlen(response), 0, (struct sockaddr *)&address, addrlen);
                printf("Response sent to client: %s\n", response);

                // Clear the buffer
                memset(buffer, 0, sizeof(buffer));
            }
        }

        // Close the slave process
        exit(EXIT_SUCCESS);

    }

    // Close master the socket
    close(master_socket);

    return 0;
}