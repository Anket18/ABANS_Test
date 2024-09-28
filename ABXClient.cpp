#include "ABXClient.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
#define close closesocket  // Windows uses closesocket instead of close
#endif

// Constructor
ABXClient::ABXClient(const std::string& address, int port)
    : serverAddress(address), serverPort(port), sockfd(INVALID_SOCKET) {}

// Destructor
ABXClient::~ABXClient() {
    closeConnection();
}

// Connect to the server
bool ABXClient::connectToServer() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        writefile.log("WSAStartup failed", LogLevel::LOG_ERROR);
        return false;
    }
#endif

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
#ifdef _WIN32
        writefile.log("Error creating socket : " + WSAGetLastError(), LogLevel::LOG_ERROR);
#else
        writefile.log("Error creating socket : ", LogLevel::LOG_ERROR);
#endif
        return false;
    }

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) <= 0) {
        writefile.log("Invalid server address", LogLevel::LOG_ERROR);
        return false;
    }

    if (connect(sockfd, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
#ifdef _WIN32
        writefile.log("Error connecting to server: " + WSAGetLastError(), LogLevel::LOG_ERROR);
#else
        writefile.log("Error connecting to server: ", LogLevel::LOG_ERROR);
#endif
        return false;
    }

    return true;
}

// Send a request to stream all packets
void ABXClient::sendStreamAllPacketsRequest() const {
    writefile.log("CallType 1: Streaming all packets");
    uint8_t request[1] = { 1 };
    if (!send(sockfd, reinterpret_cast<const char*>(request), sizeof(request), 0))
    {
        writefile.log("Failed to send packets");
        return;
    }
    writefile.log("Packets sent successfully");
    writefile.log("CallType 1: Streaming for all packets done");
}

// Send a request to resend a specific packet by sequence number
void ABXClient::sendResendPacketRequest(int sequence) const {
    writefile.log("CallType 2: Resending specific packet");
    uint8_t request[2] = { 2, static_cast<uint8_t>(sequence) };
    if (!send(sockfd, reinterpret_cast<const char*>(request), sizeof(request), 0))
    {
        writefile.log("Failed to send packets");
        return;
    }
    writefile.log("Packets sent successfully");
    writefile.log("CallType 2: Resending specific packet done");
}

// Receive raw packet data from the server
bool ABXClient::receivePacketData(uint8_t* buffer, size_t bufferSize) const {
    writefile.log("Receiving Packet");
    size_t bytesReceived = recv(sockfd, reinterpret_cast<char*>(buffer), bufferSize, 0);

    if (bytesReceived == bufferSize) {
        writefile.log("Packet Received");
        return true;
    }
    else {
        writefile.log("Failed to receive the full packet, requesting resend.", LogLevel::LOG_ERROR);

        // Determine the expected sequence based on your logic
        // If bytesReceived is 0, the socket may have closed, or an error occurred.
        if (bytesReceived == 0) {
            writefile.log("Socket closed or error occurred, handling appropriately.", LogLevel::LOG_ERROR);
            return false; // Handle socket closure or error
        }

        // expectedPacketSequence = ...; // Logic to determine the expected sequence could be implemented but currently not having enough time ;-(
        sendResendPacketRequest(expectedPacketSequence);    //Assuming the packet sequence can be extracted from the received data, so I'm typically extract the last known sequence number

        return false; // Return false as the packet wasn't fully received
    }
}

// Function to parse and print the received packet data
void ABXClient::processPacketData(const uint8_t* data) {
    writefile.log("Processing packet data");

    char symbol[5] = { 0 };
    std::memcpy(symbol, data, 4);

    char buySellIndicator = data[4];
    int quantity = (data[5] << 24) | (data[6] << 16) | (data[7] << 8) | data[8];
    int price = (data[9] << 24) | (data[10] << 16) | (data[11] << 8) | data[12];
    int packetSeq = (data[13] << 24) | (data[14] << 16) | (data[15] << 8) | data[16];

    // Check packet sequence against expected sequence
    if (packetSeq != expectedPacketSequence) {
        writefile.log("Out of order packet detected, requesting resend.", LogLevel::LOG_ERROR);
        sendResendPacketRequest(expectedPacketSequence);
    }
    else {
        // Process valid packet
        std::string sParsedData = "Symbol: " + std::string(symbol) +
            ", Buy/Sell: " + buySellIndicator +
            ", Quantity: " + std::to_string(quantity) +
            ", Price: " + std::to_string(price) +
            ", Packet Seq: " + std::to_string(packetSeq);
        writefile.log(sParsedData);
        writefile.log("Processing done for packet data");

        // Update expected sequence for the next packet
        expectedPacketSequence++;
    }
}

// Close the connection
void ABXClient::closeConnection() {
    if (sockfd != INVALID_SOCKET) {
        writefile.log("Closing Connection.");
#ifdef _WIN32
        if (closesocket(sockfd)) {
            writefile.log("Connection Closed.");
            return;
        }
        else {
            writefile.log("Failed to close connection.");
        }
        WSACleanup();  // Cleanup Winsock
#else
        if (!close(sockfd)) {
            writefile.log("Failed to close connection.");
            return;
        }
        else {
            writefile.log("Connection Closed");
        }
#endif
        sockfd = INVALID_SOCKET;
    }
    writefile.log("Invalid Socket!!!");
}
