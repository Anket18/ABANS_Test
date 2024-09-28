#include "writefile.h"

// Helper function to get current date and time
std::string WRITEFILE::getCurrentDate() const
{
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &now);  // Use localtime_s instead of localtime

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &localTime);
    return std::string(buffer);
}

// Log function implementation
void WRITEFILE::log(const string& message, LogLevel level) const
{
    string strLL = "";
    switch (level) {
    case LogLevel::LOG_INFO:
        strLL = "INFO";
        break;
    case LogLevel::LOG_ERROR:
        strLL = "ERROR";
        break;
    default:
        strLL = "INFO";
        break;
    }

    // To get current date and time
    std::string date = getCurrentDate();

    // Opening file in append mode
    std::ofstream logFile("logfile.txt", std::ios_base::app);
    if (logFile.is_open()) {
        // Write the log to the file
        logFile << date << " | " << strLL << " | " << message << std::endl;
        logFile.close();
    }
    else {
        std::cerr << "Unable to open log file" << std::endl;
    }

    return;
}
