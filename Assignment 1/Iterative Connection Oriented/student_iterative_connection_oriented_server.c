#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_NAME_LENGTH 50
#define FILENAME "student.txt"
#define PORT 8080
#define MAX_CLIENTS 10

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
    int server_fd, new_socket, valread;
    int opt = 1;
    int addrlen = sizeof(address);
    char file_data[MAX_NAME_LENGTH * 2 * sizeof(int) * 2];
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
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully.\n");

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Socket binded to port %d.\n", PORT);

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection established.\n");

        // Loop to keep reading data until client closes connection
        while (1)
        {
            // Read incoming data
            char buffer[1024] = {0};
            valread = read(new_socket, buffer, 1024);
            // Check if connection closed by client
            if (valread == 0)
            {
                printf("Connection closed by client.\n");
                break;
            }
            printf("Data received from client: %s\n", buffer);

            // Parse student data from incoming message
            sscanf(buffer, "%d %s %s %s", &student.serialNumber, student.regNumber, student.firstName, student.lastName);

            // Check if serial number or registration number already exists
            if (checkIfSerialNumberExists(student.serialNumber))
            {
                char error_message[] = "Error: Student with the same serial number already exists.";
                send(new_socket, error_message, strlen(error_message), 0);
            }
            else if (checkIfRegNumberExists(student.regNumber))
            {
                char error_message[] = "Error: Student with the same registration number already exists.";
                send(new_socket, error_message, strlen(error_message), 0);
            }
            else
            {
                // Append new student data to file
                fprintf(file, "%d\t\t\t\t\t\t %s\t\t\t\t\t\t %s %s\n", student.serialNumber, student.regNumber, student.firstName, student.lastName);
                fflush(file);
                char success_message[] = "Student Added Successfully";
                send(new_socket, success_message, strlen(success_message), 0);
            }
        }
    }

        // Close socket
        close(new_socket);
    

    // Close file
    fclose(file);

    return 0;
}
