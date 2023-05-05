#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_NAME_LENGTH 50
#define PORT 8080

struct Student
{
    /* Student Properties */
    int serialNumber;
    char regNumber[20];
    char firstName[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
};

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    struct Student student;

    // Get student details from user input
    printf("Enter the student's details:\n");
    printf("Serial Number: ");
    scanf("%d", &student.serialNumber);
    printf("Registration Number: ");
    scanf("%s", student.regNumber);
    printf("First Name: ");
    scanf("%s", student.firstName);
    printf("Last Name: ");
    scanf("%s", student.lastName);

    // Create a socket file descriptor
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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

    // Convert the student details to a buffer
    sprintf(buffer, "%d %s %s %s", student.serialNumber, student.regNumber, student.firstName, student.lastName);

    // Send the buffer to the server
    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    printf("Student data sent successfully.\n");

    return 0;
}
