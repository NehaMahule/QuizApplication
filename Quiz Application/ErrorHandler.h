#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

class ErrorHandler
{
private:
    static const std::string LOG_FILE;
    
    // Private constructor to prevent instantiation
    ErrorHandler() {}

public:
    // Error levels
    enum ErrorLevel
    {
        INFO = 0,
        WARNING = 1,
        ERROR_LEVEL = 2,
        CRITICAL = 3
    };

    // Log error to file and console
    static void logError(const std::string& message, ErrorLevel level = ERROR_LEVEL);
    
    // Log database errors
    static void logDatabaseError(const std::string& operation, const std::string& errorMsg);
    
    // Log authentication errors
    static void logAuthError(const std::string& username, const std::string& reason);
    
    // Log quiz errors
    static void logQuizError(int quizId, const std::string& errorMsg);
    
    // Get timestamp
    static std::string getTimestamp();
    
    // Get error level string
    static std::string getLevelString(ErrorLevel level);
    
    // View error log
    static void viewErrorLog();
    
    // Clear error log
    static void clearErrorLog();
};

#endif