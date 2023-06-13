#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Prompt the user for input
    int num1, num2;
    char op;
    printf("Enter the first number: ");
    scanf("%d", &num1);
    printf("Choose the operator you would like to use:\n");
    printf("+ (Addition)\n");
    printf("- (Subtraction)\n");
    printf("* (Multiplication)\n");
    printf("/ (Division)\n");
    scanf(" %c", &op);
    printf("Enter the second number: ");
    scanf("%d", &num2);

    // Check if the operator entered is valid
    int validOperator = 1; // Flag variable to indicate operator validity

    switch (op) {
        case '+':
        case '-':
        case '*':
        case '/':
            break;
        default:
            printf("Invalid operator\n");
            validOperator = 0; // Set the flag to indicate invalid operator
    }

    // If the operator is invalid, exit the program
    if (!validOperator) {
        return -1;
    }

    // Construct the request string
    snprintf(buffer, 1024, "%d %c %d", num1, op, num2);

    // Send the request to the server
    if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nSendto failed\n");
        return -1;
    }

    // Receive the response from the server
    socklen_t serv_addr_len = sizeof(serv_addr);
    if (recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&serv_addr, &serv_addr_len) < 0) {
        printf("\nRecvfrom failed\n");
        return -1;
    }

    // Extract the result from the response
    int result = atoi(buffer);

    // Display the result to the user
    printf("Result: %d\n\n", result);

    return 0;
}
