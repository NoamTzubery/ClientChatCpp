#include "client.hpp"
#include <stdexcept>

int main() {
    try {
        Client client;
        client.start(CONNECTION_IP);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
