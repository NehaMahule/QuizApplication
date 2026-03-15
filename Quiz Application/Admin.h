#ifndef ADMIN_H
#define ADMIN_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Interfaces.h"
#include "ErrorHandler.h"

class Admin : public IAdmin
{
private:
    IDatabase* database;
    int adminId;

    // Helper methods
    bool verifyQuizOwnership(int quizId);
    void displayQuizStats();
    void showHelp();

public:
    Admin(IDatabase* db, int id = 1);
    void setAdminId(int id) { adminId = id; }

    void adminMenu();

    // Quiz Management
    void createQuiz();
    void viewAllQuizzes();
    void editQuiz();
    void deleteQuiz();
    void cloneQuiz();
    
    // Question Management
    void addQuestion();
    void viewQuestionsByQuizId();
    void editQuestion();
    void deleteQuestion();
    void searchQuestionBank();
    
    // User Management
    void viewAllUsers();
    void deleteUser();
    void promoteUserToAdmin();
    
    // Analytics & Export
    void viewQuizAnalytics();
    void exportQuizResults();
    
    // System Management
    void viewSystemLogs();
    void backupDatabase();
};

#endif