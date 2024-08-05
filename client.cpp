#include "client.hpp"
#include <string>
#include <thread>
#include <cstdint>
#include <iomanip>

Client::Client() {}

Client::~Client() {
    closesocket(_client_fd);
    WSACleanup();
}

void Client::initializeWinsock() {
    WSADATA ws;
    if (WSAStartup(MAKEWORD(MAJOR_VERSION, MINOR_VERSION), &ws) == FAILED) {
        throw std::runtime_error("Failed to connect to initialize window sockets");
    }
    else {
        std::cout << "WSA opened Successfully\n";
    }
}

void Client::createSocket() {
    _client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_client_fd == INVALID_SOCKET) {
        std::cout << "Failed to create socket\n";
        WSACleanup();
        throw std::runtime_error("Failed to create socket");
    }
    else {
        std::cout << "Socket created Successfully\n";
    }
}

void Client::connectToServer(const std::string& serverIp) {
    _serverAddr.sin_family = AF_INET;
    _serverAddr.sin_port = htons(PORT);
    int result = inet_pton(AF_INET, serverIp.data(), &_serverAddr.sin_addr);
    if (result == 1) {
        // Successful conversion
    }
    else if (result == 0) {
        throw std::invalid_argument("Invalid IP address format: " + std::string(serverIp));
    }
    else {
        throw std::runtime_error("inet_pton failed: " + std::string(strerror(errno)));
    }
    int connectResult = connect(_client_fd, (sockaddr*)&_serverAddr, sizeof(_serverAddr));
    if (connectResult == SOCKET_ERROR) {
        std::cout << "Failed to connect to server\n";
        closesocket(_client_fd);
        WSACleanup();
        throw std::runtime_error("Failed to connect to server");
    }
    else {
        std::cout << "Connected to server\n";
    }
}

void Client::sendMessage(const std::string& message) {
    uint32_t messageSize = htonl(static_cast<uint32_t>(message.size()));
    send(_client_fd, reinterpret_cast<char*>(&messageSize), sizeof(messageSize), 0);
    int sendResult = send(_client_fd, message.c_str(), message.size(), 0);
    if (sendResult == SOCKET_ERROR) {
        std::cout << "Failed to send message\n";
        closesocket(_client_fd);
        WSACleanup();
        throw std::runtime_error("Failed to send message");
    }
    else {
        std::cout << "Message sent\n";
    }
}

void Client::sendMessageThread() {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "exit") {
            break;
        }
        if (message.length() > UINT_MAX) {
            std::cout << "Message too long\n";
        }
        else {
            sendMessage(message);
        }
    }
}

uint32_t Client::parseSize(const std::string& buffer) {
    size_t pos = buffer.find(SEPARATER);
    if (pos == std::string::npos) {
        std::cerr << "Failed to parse message size" << std::endl;
        return 0;
    }
    return static_cast<uint32_t>(std::stoul(buffer.substr(0, pos)));
}


void Client::parseTime(const std::string& timeStr, uint32_t& hours, uint32_t& minutes) {
    size_t pos = timeStr.find(':');
    if (pos == std::string::npos) {
        std::cerr << "Failed to parse message time" << std::endl;
        return;
    }
    hours = static_cast<uint32_t>(std::stoul(timeStr.substr(0, pos)));
    minutes = static_cast<uint32_t>(std::stoul(timeStr.substr(pos + 1)));
}

std::string Client::parseMessage(const std::string& messageStr, uint32_t msgSize) {
    return messageStr.substr(0, msgSize);
}

void Client::receiveMessagesThread() {
    int bytesReceived;

    while (true) {
        // Receive the message size first
        uint32_t messageSize = 0;
        bytesReceived = recv(_client_fd, reinterpret_cast<char*>(&messageSize), sizeof(messageSize), 0);

        if (bytesReceived <= 0) {
            if (bytesReceived == 0) {
                std::cout << "Connection closed by the server." << std::endl;
            }
            else {
                std::cerr << "Error receiving data size." << std::endl;
            }
            break;
        }

        messageSize = ntohl(messageSize); // Convert to host byte order

        // Allocate a buffer for the actual message content
        std::vector<char> buffer(messageSize + 1); // +1 for null terminator
        bytesReceived = recv(_client_fd, buffer.data(), messageSize, 0);

        if (bytesReceived <= 0 || bytesReceived != static_cast<int>(messageSize)) {
            if (bytesReceived == 0) {
                std::cout << "Connection closed by the server." << std::endl;
            }
            else {
                std::cerr << "Error receiving message content or mismatched size." << std::endl;
            }
            break;
        }

        buffer[messageSize] = NULL_TERMINATOR; 
        std::string bufferStr(buffer.data(), messageSize);

        size_t timePos = bufferStr.find(SEPARATER) + 1;
        std::string timeStr = bufferStr.substr(timePos);
        uint32_t msgHours, msgMinutes;
        parseTime(timeStr, msgHours, msgMinutes);

        size_t messagePos = timeStr.find(SEPARATER) + 1;
        std::string message = parseMessage(timeStr.substr(messagePos), messageSize);

        std::cout << "[" << std::setfill('0') << std::setw(TIME_VIEW_SIZE) << msgHours
            << ":" << std::setfill('0') << std::setw(TIME_VIEW_SIZE) << msgMinutes
            << "]: " << message << std::endl;
    }
}



void Client::run() {
    std::thread sendThread(&Client::sendMessageThread, this);
    std::thread receiveThread(&Client::receiveMessagesThread, this);

    sendThread.join();
    receiveThread.join();
}

void Client::start(const std::string & serverIp) {
    initializeWinsock();
    createSocket();
    connectToServer(serverIp);
    run();
}
