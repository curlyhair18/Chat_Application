#include <iostream>
#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h> // For inet_pton
#include <unistd.h>    // For close()

#define PORT 5000

// Dummy function to simulate receiving messages
void receive_messages(int client_socket) {
    // Simulate receiving messages (for testing)
    std::cout << "Simulating message receiving on socket: " << client_socket << std::endl;
}

int main() {
    int client_socket = 1; // Dummy socket value for testing

    // Start thread to receive messages from the server
    std::thread receive_thread([client_socket]() {
        receive_messages(client_socket);  // Call the function inside the lambda
    });

    receive_thread.detach();  // Allow thread to run independently

    // Simulating sending messages
    std::cout << "Sending messages...\n";

    // Wait before exiting (for demonstration purposes)
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
