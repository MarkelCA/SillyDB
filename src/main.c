#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "./utils.h"
#include "./controller.h"

#include <stdio.h>

int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set the SO_REUSEADDR socket option
    int reuse = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1) {
        perror("Setting SO_REUSEADDR failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080); // Port number

    // Bind the socket to the address
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    // Accept a connection
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
    if (clientSocket == -1) {
        perror("Accepting connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection established with client.\n");

    // Receive and send messages
    char buffer[1024];
    memset(&buffer, 0, sizeof(buffer));
    while (1) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            perror("Connection closed or error");
            break;
        }

        // Delete the \n char at the end
        size_t lineBreakPos = strcspn(buffer, "\n");
        if (lineBreakPos < bytesRead) {
            buffer[lineBreakPos] = '\0';
        }

        // Delete the \r char at the end
        size_t carriagePos = strcspn(buffer, "\r");
        if (carriagePos < bytesRead) {
            buffer[carriagePos] = '\0';
        }

        char** tokens = str_split(buffer, ' ');

        char* result = run_command(tokens);
        printf("result:%s\n", result);
        size_t value_lenght = strlen(result) + 4; // +4 because of the \n and \0 chars and the quotes
        char* final_result = (char*)malloc(sizeof(char)* (value_lenght) );

        memset(final_result, 0, value_lenght);

        strcpy(final_result, "\"");
        strcat(final_result, result);
        strcat(final_result, "\"");
        strcat(final_result, "\n\0");


        // Echo back the message
        send(clientSocket, final_result, value_lenght, 0);

        free(tokens);
        free(result);
        tokens = NULL;
        result = NULL;
    }

    // Close the sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}