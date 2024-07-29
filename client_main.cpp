#include "client.hpp"

int main() {
    Client client;

    client.InitializeWinsock();
    client.CreateSocket();
    client.ConnectToServer("127.0.0.1");     
    client.Run();

    return 0;
}
