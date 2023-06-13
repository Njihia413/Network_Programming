#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_NAME_LENGTH 50
#define FILENAME "student.txt"
#define PORT 8080
#define BUFFER_SIZE 1024

struct Student
{
    /* Student Properties */
    int serialNumber;
    char regNumber[20];
    char firstName[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
};

int checkIfSerialNumberExists(int serialNumber)
{
    FILE *file;
    char file_data[MAX_NAME_LENGTH * 2 * sizeof(int) * 2];
    file = fopen(FILENAME, "r");
    while (fgets(file_data, sizeof(file_data), file) != NULL)
    {
        struct Student temp_student;
        sscanf(file_data, "%d\t\t\t\t\t\t %s\t\t\t\t\t\t %s %s\n", &temp_student.serialNumber, temp_student.regNumber, temp_student.firstName, temp_student.lastName);
        if (temp_student.serialNumber == serialNumber)
        {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int checkIfRegNumberExists(char *regNumber)
{
    FILE *file;
    char file_data[MAX_NAME_LENGTH * 2 * sizeof(int) * 2];
    file = fopen(FILENAME, "r");
    while (fgets(file_data, sizeof(file_data), file) != NULL)
    {
        struct Student temp_student;
        sscanf(file_data, "%d\t\t\t\t\t\t %s\t\t\t\t\t\t %s %s\n", &temp_student.serialNumber, temp_student.regNumber, temp_student.firstName, temp_student.lastName);
        if (strcmp(temp_student.regNumber, regNumber) == 0)
        {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int main()
{
    struct sockaddr_in address;
    int master_socket;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    struct Student student;
    FILE *file;

    // Open the file in Append Mode
    file = fopen(FILENAME, "a");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        exit(1);
    }

    // Check if file is empty
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (file_size == 0)
    {
        // Write Column Headers
        fprintf(file, "Serial Number\t\t\t Registration Number\t\t\t\t\t Name\n");
    }
    rewind(file);

    // Create socket file descriptor
    if ((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    printf("Master Socket created successfully.\n");

    // Set address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified address and port
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Master Socket binded to port %d.\n", PORT);

    while (1)
    {
        printf("Waiting for data...\n");
        // Receive data from the client
        memset(buffer, 0, BUFFER_SIZE);
        if (recvfrom(master_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&address, (socklen_t *)&addrlen) < 0)
        {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        // Fork a slave process to handle the client request
        pid_t slave_pid = fork();

        if (slave_pid < 0)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (slave_pid == 0)
        {
            // Child process
            // Loop to keep reading data until client closes connection
            while (1)
            {
                printf("Data received from client: %s\n", buffer);

                // Parse student data from incoming message
                sscanf(buffer, "%d %s %s %s", &student.serialNumber, student.regNumber, student.firstName, student.lastName);

                // Check if serial number or registration number already exists
                if (checkIfSerialNumberExists(student.serialNumber))
                {
                    char errorMsg[] = "Error: Student with the same serial number already exists.\n";
                    sendto(master_socket, errorMsg, strlen(errorMsg), 0, (struct sockaddr *)&address, addrlen);
                }
                else if (checkIfRegNumberExists(student.regNumber))
                {
                    char errorMsg[] = "Error: Student with the same registration number already exists.\n";
                    sendto(master_socket, errorMsg, strlen(errorMsg), 0, (struct sockaddr *)&address, addrlen);
                }
                else
                {
                    // Append new student data to file
                    fprintf(file, "%d\t\t\t\t\t\t %s\t\t\t\t\t\t %s %s\n", student.serialNumber, student.regNumber, student.firstName, student.lastName);
                    fflush(file);
                    char successMsg[] = "Student Added Successfully\n";
                    sendto(master_socket, successMsg, strlen(successMsg), 0, (struct sockaddr *)&address, addrlen);
                    printf("\n");
                }

                // Clear the buffer
                memset(buffer, 0, BUFFER_SIZE);

                // Receive data from the client
                if (recvfrom(master_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&address, (socklen_t *)&addrlen) < 0)
                {
                    perror("recvfrom failed");
                    exit(EXIT_FAILURE);
                }
            }

            // Terminate the slave process
            exit(EXIT_SUCCESS);
        }
    }

    // Close file
    fclose(file);

    return 0;
}
