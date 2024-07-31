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

/**
 * @class Client
 * @brief Manages a network client using Winsock.
 *
 * This class encapsulates the functionality required for a client to connect
 * to a server, send and receive messages, and manage network communication.
 */
class Client {
public:
    /**
     * @brief Default constructor for the Client class.
     *
     * Initializes the client object and sets up necessary resources.
     */
    Client();

    /**
     * @brief Destructor for the Client class.
     *
     * Cleans up resources used by the Client object.
     */
    ~Client();

    /**
     * @brief Initializes the Winsock library.
     *
     * Sets up the Winsock API for network communication. Handles initialization
     * errors and exits if initialization fails.
     */
    void InitializeWinsock();

    /**
     * @brief Creates a socket for the client.
     *
     * Configures the socket for TCP communication and stores its file descriptor.
     * Handles errors related to socket creation.
     */
    void CreateSocket();

    /**
     * @brief Connects the client to a server.
     *
     * Establishes a connection to the server using the provided address.
     *
     * @param serverAddress The address of the server to connect to.
     */
    void ConnectToServer(const char* serverAddress);

    /**
     * @brief Sends a message to the server.
     *
     * Sends a specified message to the connected server.
     *
     * @param message The message to send to the server.
     */
    void SendMessage(const char* message);

    /**
     * @brief Starts the client and handles communication.
     *
     * Initializes Winsock, creates a socket, connects to the server, and
     * starts threads for sending and receiving messages.
     */
    void Run();

    /**
     * @brief Thread function for receiving messages from the server.
     *
     * Listens for and processes incoming messages from the server in a separate
     * thread.
     */
    void ReceiveMessagesThread();

    /**
     * @brief Thread function for sending messages to the server.
     *
     * Sends messages to the server in a separate thread.
     */
    void SendMessageThread();

private:
    SOCKET _client_fd;          ///< File descriptor for the client socket.
    sockaddr_in _serverAddr;    ///< Address structure for the server.
    static constexpr int PORT = 12345;   ///< Port number for connecting to the server.
    static constexpr int BUFFER = 4025;  ///< Buffer size for sending and receiving messages.
    static constexpr char SEPARATER = '$';
    static constexpr int TIME_VIEW_SIZE = 2;
    /**
     * @brief Parses the message size from the received buffer.
     *
    * @param buffer The buffer containing the message data.
    * @return The size of the message.
    */
    uint32_t parseSize(const char* buffer);

    /**
     * @brief Parses the hours and minutes from the received time string.
     *
     * @param timeStr The time string in the format "HH:MM".
     * @param hours Reference to store the parsed hours.
     * @param minutes Reference to store the parsed minutes.
     */
    void parseTime(const char* timeStr, uint32_t& hours, uint32_t& minutes);

    /**
     * @brief Parses the message content from the received buffer.
     *
     * @param messageStart The start of the message content in the buffer.
     * @param msgSize The size of the message content.
     * @return The parsed message content as a string.
     */
    std::string parseMessage(const char* messageStart, uint32_t msgSize);
};

#endif // CLIENT_HPP
