#ifndef CLIENT_HPP
#define CLIENT_HPP
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <ctime>

#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
constexpr int PORT = 12345;   ///< Port number for connecting to the server.
constexpr int BUFFER = 4025;  ///< Buffer size for sending and receiving messages.
constexpr const char* CONNECTION_IP = "127.0.0.1"; ///< IP address to connect to

/**
 * @class Client
 * @brief Manages a network client using Winsock.
 *
 * This class handles connecting to a server, sending/receiving messages, and managing network communication.
 */
class Client {
public:
    /**
     * @brief Constructor: Initializes the client object.
     */
    Client();

    /**
     * @brief Destructor: Cleans up resources.
     */
    ~Client();

    /**
     * @brief Initializes the Winsock library.
     *
     * Sets up the Winsock API for network communication.
     */
    void initializeWinsock();

    /**
     * @brief Creates a socket for TCP communication.
     *
     * Configures the socket and handles creation errors.
     */
    void createSocket();

    /**
     * @brief Connects the client to the server.
     *
     * @param serverAddress The server's address.
     */
    void connectToServer(const char* serverAddress);

    /**
     * @brief Sends a message to the server.
     *
     * @param message The message to send.
     */
    void sendMessage(const char* message);

    /**
     * @brief Starts client communication processes.
     *
     * Initializes Winsock, creates a socket, connects, and starts threads for communication.
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

    /**
     * @brief Starts the client connection using the provided server IP.
     *
     * @param serverIp The IP address of the server.
     */
    void start(const char* serverIp);

private:
    SOCKET _client_fd;           ///< Client socket file descriptor.
    sockaddr_in _serverAddr;     ///< Server address structure.
    static constexpr auto PORT = 12345;   ///< Port number for connecting.
    static constexpr auto BUFFER = 4025;  ///< Buffer size for messages.
    static constexpr char SEPARATER = '$'; ///< Separator for message components.
    static constexpr auto TIME_VIEW_SIZE = 2; ///< Length of time format "HH:MM".
    static constexpr auto FAILED = -1; ///< When functions fail they return -1

    /**
     * @brief Parses the message size from the buffer.
     *
     * @param buffer The buffer containing message data.
     * @return The size of the message.
     */
    uint32_t parseSize(const char* buffer);

    /**
     * @brief Parses the hours and minutes from the time string.
     *
     * @param timeStr The time string in "HH:MM" format.
     * @param hours Reference for storing parsed hours.
     * @param minutes Reference for storing parsed minutes.
     */
    void parseTime(const char* timeStr, uint32_t& hours, uint32_t& minutes);

    /**
     * @brief Parses the message content from the buffer.
     *
     * @param messageStart Pointer to the start of the message content.
     * @param msgSize The size of the message content.
     * @return The message as a string.
     */
    std::string parseMessage(const char* messageStart, uint32_t msgSize);
};

#endif // CLIENT_HPP
