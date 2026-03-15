#ifndef INTERFACES_H
#define INTERFACES_H

#include <string>
#include "sqlite3.h"

// Database abstraction
class IDatabase
{
public:
    virtual ~IDatabase() = default;
    virtual bool openDatabase() = 0;
    virtual void closeDatabase() = 0;
    virtual bool executeQuery(const std::string& query) = 0;
    virtual void createTables() = 0;
    virtual sqlite3* getDB() = 0;
    virtual sqlite3_stmt* prepareStatement(const std::string& query) = 0;
    virtual int getLastInsertRowId() = 0;
    virtual std::string sanitizeInput(const std::string& input) = 0;
};

// Authentication abstraction
class IAuthentication
{
public:
    virtual ~IAuthentication() = default;
    virtual bool registerUser() = 0;
    virtual bool registerAdmin() = 0;
    virtual bool loginUser() = 0;
    virtual bool loginAdmin() = 0;
    virtual bool usernameExists(const std::string& username) = 0;
    virtual std::string hashPassword(const std::string& password) = 0;
    virtual int getUserId(const std::string& username) = 0;
};

// Admin abstraction
class IAdmin
{
public:
    virtual ~IAdmin() = default;
    virtual void adminMenu() = 0;
    virtual void setAdminId(int id) = 0;
    
    // Quiz Management
    virtual void createQuiz() = 0;
    virtual void viewAllQuizzes() = 0;
    virtual void editQuiz() = 0;
    virtual void deleteQuiz() = 0;
    virtual void cloneQuiz() = 0;
    
    // Question Management
    virtual void addQuestion() = 0;
    virtual void viewQuestionsByQuizId() = 0;
    virtual void editQuestion() = 0;
    virtual void deleteQuestion() = 0;
    virtual void searchQuestionBank() = 0;
    
    // User Management
    virtual void viewAllUsers() = 0;
    virtual void deleteUser() = 0;
    virtual void promoteUserToAdmin() = 0;
    
    // Analytics & Export
    virtual void viewQuizAnalytics() = 0;
    virtual void exportQuizResults() = 0;
};

// Analytics abstraction
class IAnalytics
{
public:
    virtual ~IAnalytics() = default;
    virtual void showLeaderboard() = 0;
    virtual void userAnalytics(int userId) = 0;
    virtual void bestCategory(int userId) = 0;
    virtual void weakCategory(int userId) = 0;
    virtual void showGlobalStats() = 0;
};

// User abstraction
class IUser
{
public:
    virtual ~IUser() = default;
    virtual void userMenu() = 0;
    virtual void browseQuizzes() = 0;
    virtual void selectQuizByCategory() = 0;
    virtual void selectQuizByDifficulty() = 0;
    virtual void attemptQuiz() = 0;
    virtual void viewQuizHistory() = 0;
    virtual void viewPerformance() = 0;
    virtual void getAchievements() = 0;
};

// QuizEngine abstraction
class IQuizEngine
{
public:
    virtual ~IQuizEngine() = default;
    virtual int startQuiz(int quizId, int userId) = 0;
};

#endif