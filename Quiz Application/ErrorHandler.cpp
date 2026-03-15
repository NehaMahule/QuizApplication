#include "ErrorHandler.h"
#include <iomanip>

using namespace std;

const string ErrorHandler::LOG_FILE = "error_log.txt";

string ErrorHandler::getTimestamp()
{
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

string ErrorHandler::getLevelString(ErrorLevel level)
{
    switch(level)
    {
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR_LEVEL:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

void ErrorHandler::logError(const string& message, ErrorLevel level)
{
    string timestamp = getTimestamp();
    string levelStr = getLevelString(level);
    
    string logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Print to console based on severity
    if (level >= ERROR_LEVEL) {
        cerr << logMessage << endl;
    } else {
        cout << logMessage << endl;
    }
    
    // Log to file
    ofstream logFile(LOG_FILE, ios::app);
    if(logFile.is_open())
    {
        logFile << logMessage << endl;
        logFile.close();
    }
}

void ErrorHandler::logDatabaseError(const string& operation, const string& errorMsg)
{
    string message = "DATABASE ERROR - Operation: " + operation + " | Error: " + errorMsg;
    logError(message, CRITICAL);
}

void ErrorHandler::logAuthError(const string& username, const string& reason)
{
    string message = "AUTHENTICATION ERROR - User: " + username + " | Reason: " + reason;
    logError(message, WARNING);
}

void ErrorHandler::logQuizError(int quizId, const string& errorMsg)
{
    string message = "QUIZ ERROR - Quiz ID: " + to_string(quizId) + " | Error: " + errorMsg;
    logError(message, ERROR_LEVEL);
}

void ErrorHandler::viewErrorLog()
{
    ifstream logFile(LOG_FILE);
    if (!logFile.is_open()) {
        cout << "No error log found.\n";
        return;
    }
    
    cout << "\n===== ERROR LOG =====\n";
    string line;
    while (getline(logFile, line)) {
        cout << line << endl;
    }
    logFile.close();
}

void ErrorHandler::clearErrorLog()
{
    ofstream logFile(LOG_FILE, ios::trunc);
    if (logFile.is_open()) {
        logFile.close();
        cout << "Error log cleared.\n";
    }
}