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

void handle_connection(int client_sock, int client_number)
{
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

    // Loop to keep reading data until client closes connection
    while (1)
    {
        // Read incoming data
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t valread = read(client_sock, buffer, BUFFER_SIZE);
        // Check if connection closed by client
        if (valread == 0)
        {
            printf("Connection closed by client %d.\n", client_number);
            break;
        }
        printf("Data received from client %d: %s\n", client_number, buffer);

        // Parse student data from incoming message
        sscanf(buffer, "%d %s %s %s", &student.serialNumber, student.regNumber, student.firstName, student.lastName);

        // Check if serial number or registration number already exists
        if (checkIfSerialNumberExists(student.serialNumber))
        {
            send(client_sock, "Error: Student with the same serial number already exists.\n", strlen("Error: Student with the same serial number already exists.\n"), 0);
        }
        else if (checkIfRegNumberExists(student.regNumber))
        {
            send(client_sock, "Error: Student with the same registration number already exists.\n", strlen("Error: Student with the same registration number already exists.\n"), 0);
        }
        else
        {
            // Append new student data to file
            fprintf(file, "%d\t\t\t\t\t\t %s\t\t\t\t\t\t %s %s\n", student.serialNumber, student.regNumber, student.firstName, student.lastName);
            fflush(file);
            send(client_sock, "Student Added Successfully\n", strlen("Student Added Successfully\n"), 0);
        }
    }

    // Close file
    fclose(file);

    // Close socket
    close(client_sock);
    exit(EXIT_SUCCESS);
}

int main()
{
    struct sockaddr_in address;
    int master_sock, slave_sock;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create master socket 
    if ((master_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    printf("Master socket created successfully.\n");

    // Set socket options
    if (setsockopt(master_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Set address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the master socket to the specified address and port
    if (bind(master_sock, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Master socket binded to port %d.\n", PORT);

    // Listen for incoming connections
    if (listen(master_sock, MAX_CLIENTS) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    int client_number = 1; // Initialize client number

    while (1)
    {
        // Accept incoming connection
        if ((slave_sock = accept(master_sock, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        printf("Connection established with client %d.\n", client_number);

        // Fork a slave process to handle the client request
        pid_t slave_pid = fork();

        if (slave_pid < 0)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (slave_pid == 0)
        {
            // Slave process
            // Close the master socket in the slave process
            close(master_sock);

            // Handle client connection
            handle_connection(slave_sock, client_number);

            // Exit the slave process
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process
            // Close the slave socket in the parent process
            close(slave_sock);
            client_number++; // Increment client number
        }
    }

    return 0;
}
