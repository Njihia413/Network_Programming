#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>

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
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
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

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT);

    // Loop to continuously accept incoming connections and handle clients in separate processes
    while(1)
    {
        // Accept incoming connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }

        // Fork a new process to handle the client
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process

            // Close the listening socket in the child process
            close(server_fd);

            // Loop to receive data from the client and update the text file
            while(1)
            {
                memset(&student, 0, sizeof(student));
                memset(buffer, 0, sizeof(buffer));

                // Receive the student's details from the client
                if ((valread = read(new_socket, buffer, 1024)) < 0)
                {
                    perror("read failed");
                    exit(EXIT_FAILURE);
                }
                else if (valread == 0)
                {
                    // Client has closed the connection, so close the socket and exit
                    close(new_socket);
                    exit(EXIT_SUCCESS);
                }

                // Extract the student's details from the buffer
                sscanf(buffer, "%d %s %s %s", &student.serialNumber, student.regNumber, student.firstName, student.lastName);

                // Open the text file in append mode
                filePointer = fopen(filename, "a");

                // Write the student's details to the text file
                fprintf(filePointer, "%d %s %s %s\n", student.serialNumber, student.regNumber, student.firstName, student.lastName);
                            // Close the text file
            fclose(filePointer);

            // Send a success message to the client
             printf("Student details added successfully\n");
            char successMessage[] = "Student details added successfully\n";
            send(new_socket, successMessage, strlen(successMessage), 0);
        }
    }
    else
    {
        // Parent process

        // Close the connected socket in the parent process
        close(new_socket);

        // Wait for the child process to finish
        int status;
        wait(&status);
    }
}

return 0;
}
