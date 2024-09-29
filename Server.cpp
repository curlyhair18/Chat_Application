#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>

#define PORT 8080  // Change to the same port as the server

std::vector<int> clients;  // To keep track of connected clients
std::mutex clients_mutex;   // Mutex for thread-safe access to clients

// Function to send a message to all connected clients
void broadcast_message(const std::string& message) {
    std::lock_guard<std::mutex> lock(clients_mutex);  // Lock the mutex
    for (int client : clients) {
        send(client, message.c_str(), message.size(), 0);
    }
}

// Function to handle each client
void handle_client(int client_socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);  // Receive message from client

        if (bytes_received > 0) {
            std::string message(buffer);

            // Handle special commands
            if (message == "/list") {
                std::lock_guard<std::mutex> lock(clients_mutex);  // Lock the mutex
                std::string client_list = "Connected clients: " + std::to_string(clients.size());
                send(client_socket, client_list.c_str(), client_list.size(), 0);
            } else if (message == "/exit") {
                std::cout << "Client disconnected." << std::endl;
                break;  // Exit the loop if the client wants to disconnect
            } else {
                std::cout << "Client: " << message << std::endl;  // Print received message
                std::string reply = "Message received: " + message;  // Reply to client
                send(client_socket, reply.c_str(), reply.size(), 0);  // Send reply
            }
        } else {
            std::cout << "Client disconnected or error receiving message." << std::endl;
            break;  // Exit the loop if the client disconnects
        }
    }

    // Remove client from the list
    {
        std::lock_guard<std::mutex> lock(clients_mutex);  // Lock the mutex
        clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
    }
    close(client_socket);  // Close the client socket
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Error: Socket creation failed!" << std::endl;
        return -1;
    }

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Listen on any interface
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error: Bind failed!" << std::endl;
        close(server_socket);
        return -1;
    }

    // Start listening for incoming connections
    if (listen(server_socket, 5) < 0) {
        std::cerr << "Error: Listen failed!" << std::endl;
        close(server_socket);
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    // Accept clients in a loop
    while (true) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            std::cerr << "Error: Accept failed!" << std::endl;
            continue;  // Continue accepting other clients
        }

        std::cout << "New client connected!" << std::endl;

        // Add new client to the list
        {
            std::lock_guard<std::mutex> lock(clients_mutex);  // Lock the mutex
            clients.push_back(client_socket);
        }

        // Handle the client in a separate thread
        std::thread(handle_client, client_socket).detach();
    }

    close(server_socket);
    return 0;
}
