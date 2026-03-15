#ifndef QUIZENGINE_H
#define QUIZENGINE_H

#include "Database.h"
#include "Interfaces.h"
#include <chrono>
#include <thread>
#include <vector>

class QuizEngine : public IQuizEngine
{
private:
    IDatabase* database;
    
    struct QuizQuestion {
        int id;
        std::string text;
        std::vector<std::string> options;
        int correctAnswer;
        std::string explanation;
    };
    
    void displayTimer(int seconds);
    void clearScreen();
    std::string getTimeString(int seconds);

public:
    QuizEngine(IDatabase* db);

    int startQuiz(int quizId, int userId);
};

#endif