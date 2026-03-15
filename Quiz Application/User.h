// #ifndef USER_H
// #define USER_H

// #include <iostream>
// #include <string>
// #include <vector>
// #include "Interfaces.h"

// class User : public IUser
// {
// private:
//     IDatabase* database;
//     int userId;

//     struct Achievement {
//         string name;
//         string description;
//         bool earned;
//     };
    
//     void checkAndAwardAchievements();
//     void displayAchievements();

// public:
//     User(IDatabase* db, int id);

//     void userMenu();

//     void browseQuizzes();
//     void selectQuizByCategory();
//     void selectQuizByDifficulty();

//     void attemptQuiz();

//     void viewQuizHistory();
//     void viewPerformance();
//     void getAchievements();
// };

// #endif

#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <vector>
#include "Interfaces.h"

class User : public IUser
{
private:
    IDatabase* database;
    int userId;

    struct Achievement {
        std::string name;
        std::string description;
        bool earned;
    };
    
    void checkAndAwardAchievements();
    void displayAchievements();

public:
    User(IDatabase* db, int id);

    void userMenu();

    void browseQuizzes();
    void selectQuizByCategory();
    void selectQuizByDifficulty();

    void attemptQuiz();

    void viewQuizHistory();
    void viewPerformance();
    void getAchievements();
};

#endif