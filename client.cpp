#include "client.hpp"
#include <string>
#include <thread>


Client::Client() {
}

Client::~Client() {
    closesocket(_client_fd);
    WSACleanup();
}

void Client::InitializeWinsock() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) == -1) {
        std::cout << "WSA Failed to Initialize\n";
        exit(1);
    }
    else {
        std::cout << "WSA opened Successfully\n";
    }
}

void Client::CreateSocket() {
    _client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_client_fd == INVALID_SOCKET) {
        std::cout << "Failed to create socket\n";
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "Socket created Successfully\n";
    }
}

void Client::ConnectToServer(const char* serverAddress) {
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, serverAddress, &_serverAddr.sin_addr);

    int connectResult = connect(_client_fd, (sockaddr*)&_serverAddr, sizeof(_serverAddr));
    if (connectResult == SOCKET_ERROR) {
        std::cout << "Failed to connect to server\n";
        closesocket(_client_fd);
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "Connected to server\n";
    }
}

void Client::SendMessage(const char* message) {
    int sendResult = send(_client_fd, message, strlen(message), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cout << "Failed to send message\n";
        closesocket(_client_fd);
        WSACleanup();
        exit(1);
    }
    else {
        std::cout << "Message sent\n";
    }
}

void Client::SendMessageThread() {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }
        if (message.length() > UINT_MAX) {
            std::cout << "message too long\n";
        }
        else {
            SendMessage(message.c_str());
        }
    }
}


void Client::ReceiveMessagesThread() {
    char buffer[BUFFER];
    int bytesReceived;

    while (true) {
        // Initialize buffer to store the incoming data
        memset(buffer, 0, BUFFER);
        bytesReceived = recv(_client_fd, buffer, BUFFER, 0);

        if (bytesReceived > 0) {
            // Parse the size (up until the first '$')
            char* sizeEnd = strchr(buffer, '$');
            if (sizeEnd == nullptr) {
                std::cerr << "Failed to parse message size" << std::endl;
                continue;
            }
            *sizeEnd = '\0';
            uint32_t msgSize = static_cast<uint32_t>(std::stoul(buffer));

            // Parse the hours (up until the ':')
            char* hoursStart = sizeEnd + 1;
            char* hoursEnd = strchr(hoursStart, ':');
            if (hoursEnd == nullptr) {
                std::cerr << "Failed to parse message hours" << std::endl;
                continue;
            }
            *hoursEnd = '\0';
            uint32_t msgHours = static_cast<uint32_t>(std::stoul(hoursStart));

            // Parse the minutes (up until the second '$')
            char* minutesStart = hoursEnd + 1;
            char* minutesEnd = strchr(minutesStart, '$');
            if (minutesEnd == nullptr) {
                std::cerr << "Failed to parse message minutes" << std::endl;
                continue;
            }
            *minutesEnd = '\0';
            uint32_t msgMinutes = static_cast<uint32_t>(std::stoul(minutesStart));

            // Parse the message content
            char* messageStart = minutesEnd + 1;
            std::string message(messageStart, msgSize);

            // Print the received message
            std::cout << "[" << msgHours << ":" << msgMinutes << "]: " << message << std::endl;
        }
        else if (bytesReceived == 0) {
            std::cout << "Connection closed by the server." << std::endl;
            break;
        }
        else {
            std::cerr << "Error receiving data" << std::endl;
            break;
        }
    }
}
void Client::Run() {
    std::thread sendThread(&Client::SendMessageThread, this);
    std::thread receiveThread(&Client::ReceiveMessagesThread, this);

    sendThread.join();
    receiveThread.join();
}
