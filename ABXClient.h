#ifndef ABXCLIENT_H
#define ABXCLIENT_H

#include <string>
#include "writefile.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  // Link against Winsock library
#else
#include <sys/socket.h>             // Unix/Linux (Preffered but I'm not having Linux installed in my personal laptop hance added both windows/linux)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

class ABXClient {
private:
    SOCKET sockfd;                // Socket file descriptor
    std::string serverAddress;  // Server IP address
    int serverPort;             // Server port number
    int expectedPacketSequence = 0;  // Track the expected packet sequence

public:
    WRITEFILE writefile;    //For writing into log file

    // Constructor
    ABXClient(const std::string& address, int port);

    // Destructor
    ~ABXClient();

    // Connect to the server
    bool connectToServer();

    // Send a request to stream all packets
    void sendStreamAllPacketsRequest() const;

    // Send a request to resend a specific packet by sequence number
    void sendResendPacketRequest(int sequence) const;

    // Function to parse and print the received packet data
    void processPacketData(const uint8_t* data);

    // Receive raw packet data from the server
    bool receivePacketData(uint8_t* buffer, size_t bufferSize) const;

    // Close the connection to the server
    void closeConnection();
};

#endif // ABXCLIENT_H
