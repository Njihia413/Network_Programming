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
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    struct Student student;

    // Create a file pointer for the text file
    FILE *filePointer;
    char filename[] = "student_details.txt";

    // Create a socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the specified port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Bind the socket to the specified port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Loop to continuously receive data from clients and update the text file
    while(1)
    {
        memset(&student, 0, sizeof(student));
        memset(buffer, 0, sizeof(buffer));

        // Receive the student's details from the client
        if ((valread = recvfrom(server_fd, buffer, 1024, 0, (struct sockaddr *)&address, &addrlen)) < 0)
        {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        // Extract the student's details from the buffer
        sscanf(buffer, "%d %s %s %s", &student.serialNumber, student.regNumber, student.firstName, student.lastName);

        // Open the text file in append mode
        filePointer = fopen(filename, "a");

        // Write the student's details to the text file
        fprintf(filePointer, "%d %s %s %s\n", student.serialNumber, student.regNumber, student.firstName, student.lastName);
        fclose(filePointer);

        printf("Student data saved successfully.\n");
    }

    return 0;
}
