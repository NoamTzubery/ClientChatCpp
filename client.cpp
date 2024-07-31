#include "client.hpp"
#include <string>
#include <thread>
#include <cstdint>
#include <iomanip>




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

uint32_t Client::parseSize(const char* buffer) {
    std::string bufferStr(buffer);
    size_t pos = bufferStr.find(SEPARATER);
    if (pos == std::string::npos) {
        std::cerr << "Failed to parse message size" << std::endl;
        return 0;
    }
    return static_cast<uint32_t>(std::stoul(bufferStr.substr(0, pos)));
}

void Client::parseTime(const char* timeStr, uint32_t& hours, uint32_t& minutes) {
    std::string timeStrStd(timeStr);
    size_t pos = timeStrStd.find(':');
    if (pos == std::string::npos) {
        std::cerr << "Failed to parse message hours" << std::endl;
        return;
    }
    hours = static_cast<uint32_t>(std::stoul(timeStrStd.substr(0, pos)));
    minutes = static_cast<uint32_t>(std::stoul(timeStrStd.substr(pos + 1)));
}

std::string Client::parseMessage(const char* messageStart, uint32_t msgSize) {
    return std::string(messageStart, msgSize);
}

void Client::ReceiveMessagesThread() {
    char buffer[BUFFER];
    int bytesReceived;

    while (true) {
        memset(buffer, 0, BUFFER);
        bytesReceived = recv(_client_fd, buffer, BUFFER, 0);

        if (bytesReceived > 0) {
            uint32_t msgSize = parseSize(buffer);
            if (msgSize == 0) continue;

            std::string bufferStr(buffer);
            size_t timePos = bufferStr.find(SEPARATER) + 1;
            std::string timeStr = bufferStr.substr(timePos);
            uint32_t msgHours, msgMinutes;
            parseTime(timeStr.c_str(), msgHours, msgMinutes);

            size_t messagePos = timeStr.find(SEPARATER) + 1;
            std::string message = parseMessage(timeStr.c_str() + messagePos, msgSize);

            std::cout << "[" << std::setfill('0') << std::setw(TIME_VIEW_SIZE) << msgHours
                << ":" << std::setfill('0') << std::setw(TIME_VIEW_SIZE) << msgMinutes
                << "]: " << message << std::endl;
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
