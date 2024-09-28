#include "ABXClient.h"
#include <iostream>
#include <vector>
#include <cstdint>

int main()
{
    try
    {
        const std::string serverAddress = "127.0.0.1";  // Replace with actual server address
        int serverPort = 3000;

        // Create an ABXClient instance
        ABXClient client(serverAddress, serverPort);

        // Connect to the server
        if (!client.connectToServer()) {
            client.writefile.log("Failed to connect to the server.", LogLevel::LOG_ERROR);
            return 1;
        }
        else {
            client.writefile.log("Connection Successful.");
        }

        // Send a request to stream all packets
        client.sendStreamAllPacketsRequest();

        // Prepare a buffer to receive packet data (each packet is 17 bytes)
        const size_t PACKET_SIZE = 17;
        uint8_t buffer[PACKET_SIZE];

        // Receive and process packets (example of receiving multiple packets)
        while (client.receivePacketData(buffer, PACKET_SIZE)) {
            client.processPacketData(buffer);
        }

        // Close the connection
        client.closeConnection();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}
