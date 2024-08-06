#ifndef CLIENT_HPP
#define CLIENT_HPP
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <ctime>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

static constexpr auto CONNECTION_IP = "127.0.0.1"; ///< IP address to connect to

/**
 * @class Client
 * @brief Manages a network client using Winsock.
 */
class Client {
public:
    Client();  
    ~Client(); 

    /**
     * @brief Starts the client connection using the provided server IP.
     *
     * @param serverIp The IP address of the server.
     */
    void start(const std::string& serverIp);

private:
    SOCKET _client_fd;           ///< Client socket file descriptor.
    sockaddr_in _serverAddr;     ///< Server address structure.
    static constexpr auto PORT = 12345;   ///< Port number for connecting.
    static constexpr char SEPARATER = '$'; ///< Separator for message components.
    static constexpr auto TIME_VIEW_SIZE = 2; ///< Length of time format "HH:MM".
    static constexpr auto FAILED = -1; ///< Indicator for function failure.
    static constexpr auto NEXT_BYTE = 1;///< Indicator to move to the next byte in parsing.
    static constexpr auto MAJOR_VERSION = 2; ///< the version of winsocket
    static constexpr auto MINOR_VERSION = 2; ///< the minor version of winsocket
    static constexpr auto NULL_TERMINATOR = '\0'; ///< null terminator.

    /**
     * @brief Parses the message size from the buffer.
     *
     * @param buffer The buffer containing message data.
     * @return The size of the message.
     */
    uint32_t parseSize(const std::string& buffer);

    /**
     * @brief Parses the hours and minutes from the time string.
     *
     * @param timeStr The time string in "HH:MM" format.
     * @param hours Reference for storing parsed hours.
     * @param minutes Reference for storing parsed minutes.
     */
    void parseTime(const std::string& timeStr, uint32_t& hours, uint32_t& minutes);

    /**
     * @brief Parses the message content from the buffer.
     *
     * @param messageStart Pointer to the start of the message content.
     * @param msgSize The size of the message content.
     * @return The message as a string.
     */
    std::string parseMessage(const std::string& messageStr, uint32_t msgSize);

    /**
    * @brief Initializes the Winsock library.
    */
    void initializeWinsock();

    /**
     * @brief Creates a socket for TCP communication.
     */
    void createSocket();

    /**
     * @brief Connects the client to the server.
     *
     * @param serverIp The server's IP address.
     */
    void connectToServer(const std::string& serverIp);

    /**
     * @brief Sends a message to the server.
     *
     * @param message The message to send.
     */
    void sendMessage(const std::string& message);

    /**
     * @brief Starts client communication processes.
     */
    void run();

    /**
     * @brief Thread function: Receives messages from the server.
     */
    void receiveMessagesThread();

    /**
     * @brief Thread function: Sends messages to the server.
     */
    void sendMessageThread();

};

#endif // CLIENT_HPP
