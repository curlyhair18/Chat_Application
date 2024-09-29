#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>  // Include this header for inet_addr
#include <unistd.h>
#include <thread>

#define PORT 8080  // Change to the same port as the server

void receive_messages(int client_socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);  // Receive reply
        if (bytes_received > 0) {
            std::cout << "Server: " << buffer << std::endl;  // Print received message
        } else {
            std::cerr << "Error receiving reply from server." << std::endl;
            break;  // Exit loop on error
        }
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error: Socket creation failed!" << std::endl;
        return -1;
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Connect to localhost

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Connection to server failed!" << std::endl;
        close(client_socket);
        return -1;
    }

    std::cout << "Connected to server!" << std::endl;

    // Start a thread to receive messages
    std::thread(receive_messages, client_socket).detach();

    // Send messages in a loop
    std::string message;
    while (true) {
        std::cout << "Enter message (or /list to see connected clients, /exit to disconnect): ";
        std::getline(std::cin, message);  // Get user input

        if (message == "/exit") {
            send(client_socket, message.c_str(), message.size(), 0);  // Send exit command
            break;  // Exit loop
        } else {
            send(client_socket, message.c_str(), message.size(), 0);  // Send message
        }
    }

    // Close the socket
    close(client_socket);
    std::cout << "Client disconnected." << std::endl;  // Notify disconnection
    return 0;
}
