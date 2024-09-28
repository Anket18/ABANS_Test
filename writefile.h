#ifndef WRITEFILE_H
#define WRITEFILE_H

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

using namespace std;

enum class LogLevel {
    LOG_INFO,
    LOG_ERROR
};

class WRITEFILE {
public:
    // Log function declaration
    void log(const string& message, LogLevel level = LogLevel::LOG_INFO) const;

private:
    // Function to get the current date and time as a string
    string getCurrentDate() const;
};

#endif
