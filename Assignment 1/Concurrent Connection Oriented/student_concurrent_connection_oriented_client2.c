#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define MAX_NAME_LENGTH 50
#define PORT 8080
#define TIMEOUT_SECONDS 5

struct Student
{
    /* Student Properties */
    int serialNumber;
    char regNumber[20];
    char firstName[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
};

int main () {
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    struct Student student;

    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Set a timeout for the socket
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SECONDS;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        printf("\nError setting timeout: %s\n", strerror(errno));
        return -1;
    }

    while(1) {
        // Get the student's details from the user
        printf("Enter the student's details:\n");
        printf("Serial Number: ");
        scanf("%d", &student.serialNumber);
        printf("Registration Number: ");
        scanf("%s", student.regNumber);
        printf("First Name: ");
        scanf("%s", student.firstName);
        printf("Last Name: ");
        scanf("%s", student.lastName);

        // Send the student's details to the server
        sprintf(buffer, "%d %s %s %s", student.serialNumber, student.regNumber, student.firstName, student.lastName);
        send(sock , buffer , strlen(buffer) , 0 );
        printf("Student data sent successfully.\n");

        // Receive the response from the server
        valread = read( sock , buffer, 1024);
        printf("%s\n",buffer );
        printf("\n");
    }
    return 0;
}