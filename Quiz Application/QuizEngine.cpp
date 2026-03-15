#include "QuizEngine.h"
#include <iostream>
#include "sqlite3.h"
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>
#include <algorithm>
#include "ErrorHandler.h"

using namespace std;
using namespace chrono;

QuizEngine::QuizEngine(IDatabase* db)
{
    database = db;
}

void QuizEngine::clearScreen()
{
    // Cross-platform clear screen
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

string QuizEngine::getTimeString(int seconds)
{
    int mins = seconds / 60;
    int secs = seconds % 60;
    return to_string(mins) + ":" + (secs < 10 ? "0" : "") + to_string(secs);
}

void QuizEngine::displayTimer(int seconds)
{
    cout << "\r Time remaining: " << getTimeString(seconds) << " ";
    cout.flush();
}

int QuizEngine::startQuiz(int quizId, int userId)
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    // Get quiz details
    string quizQuery = "SELECT title, description, time_limit FROM quizzes WHERE quiz_id = " + to_string(quizId) + ";";
    stmt = database->prepareStatement(quizQuery);
    
    if (!stmt || sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "Quiz not found.\n";
        if (stmt) sqlite3_finalize(stmt);
        return 0;
    }
    
    string quizTitle = (const char*)sqlite3_column_text(stmt, 0);
    string quizDescription = (const char*)sqlite3_column_text(stmt, 1);
    int timeLimit = sqlite3_column_int(stmt, 2);
    sqlite3_finalize(stmt);

    // Fetch questions
    string questionQuery =
        "SELECT question_id, question_text, option1, option2, option3, option4, correct_answer, explanation "
        "FROM questions "
        "WHERE quiz_id = " + to_string(quizId) +
        " ORDER BY RANDOM();";

    if(sqlite3_prepare_v2(db, questionQuery.c_str(), -1, &stmt, NULL) != SQLITE_OK)
    {
        cout << "Failed to fetch questions\n";
        return 0;
    }

    vector<QuizQuestion> questions;
    
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        QuizQuestion q;
        q.id = sqlite3_column_int(stmt, 0);
        q.text = (char*)sqlite3_column_text(stmt, 1);
        q.options.push_back((char*)sqlite3_column_text(stmt, 2));
        q.options.push_back((char*)sqlite3_column_text(stmt, 3));
        q.options.push_back((char*)sqlite3_column_text(stmt, 4));
        q.options.push_back((char*)sqlite3_column_text(stmt, 5));
        q.correctAnswer = sqlite3_column_int(stmt, 6);
        q.explanation = (char*)sqlite3_column_text(stmt, 7);
        questions.push_back(q);
    }
    
    sqlite3_finalize(stmt);

    if (questions.empty()) {
        cout << "No questions available for this quiz.\n";
        return 0;
    }

    // Shuffle questions (optional)
    random_shuffle(questions.begin(), questions.end());

    clearScreen();
    cout << "\n" << string(60, '=') << "\n";
    cout << "                    QUIZ START\n";
    cout << string(60, '=') << "\n";
    cout << "Quiz: " << quizTitle << "\n";
    cout << "Description: " << quizDescription << "\n";
    cout << "Total Questions: " << questions.size() << "\n";
    cout << "Time Limit: " << timeLimit << " minutes\n";
    cout << string(60, '=') << "\n";
    
    cout << "\n Instructions:\n";
    cout << "• Read each question carefully\n";
    cout << "• Enter your answer (A, B, C, D or 1-4)\n";
    cout << "• You have " << timeLimit << " minutes total\n";
    cout << "• Each question has a 30-second time limit\n";
    cout << "\nPress Enter to start the quiz...";
    cin.ignore();
    cin.get();

    clearScreen();

    int score = 0;
    auto startTime = steady_clock::now();
    int timeLimitSeconds = timeLimit * 60;
    vector<int> userAnswers(questions.size(), 0);

    for (size_t i = 0; i < questions.size(); i++)
    {
        auto elapsed = duration_cast<seconds>(steady_clock::now() - startTime).count();
        int remaining = timeLimitSeconds - elapsed;
        
        if (remaining <= 0) {
            cout << "\n Time's up! Quiz terminated.\n";
            break;
        }

        cout << "\n" << string(60, '-') << "\n";
        cout << "Question " << (i + 1) << " of " << questions.size() << "\n";
        cout << "Time remaining: " << getTimeString(remaining) << "\n";
        cout << string(60, '-') << "\n\n";
        
        cout << questions[i].text << "\n\n";
        
        for (int j = 0; j < 4; j++) {
            cout << "  " << (char)('A' + j) << ") " << questions[i].options[j] << "\n";
        }

        int answer = 0;
        bool validInput = false;
        auto questionStart = steady_clock::now();
        
        do {
            cout << "\nYour answer (A/B/C/D or 1-4): ";
            string input;
            cin >> input;
            
            if (input.length() == 1) {
                char c = toupper(input[0]);
                if (c >= 'A' && c <= 'D') {
                    answer = c - 'A' + 1;
                    validInput = true;
                } else if (c >= '1' && c <= '4') {
                    answer = c - '0';
                    validInput = true;
                }
            }
            
            if (!validInput) {
                cout << " Invalid input! Please enter A, B, C, D or 1-4.\n";
            }
            
            // Check question time limit (30 seconds)
            auto questionElapsed = duration_cast<seconds>(steady_clock::now() - questionStart).count();
            if (questionElapsed >= 30) {
                cout << "\n Time's up for this question!\n";
                answer = 0;
                validInput = true;
            }
            
        } while (!validInput);

        userAnswers[i] = answer;

        if (answer == questions[i].correctAnswer) {
            score++;
            cout << "\n Correct!\n";
        } else if (answer > 0) {
            cout << "\n Wrong! The correct answer was " << (char)('A' + questions[i].correctAnswer - 1) << "\n";
        }
        
        if (!questions[i].explanation.empty()) {
            cout << " Explanation: " << questions[i].explanation << "\n";
        }
        
        this_thread::sleep_for(milliseconds(1500));
    }

    auto endTime = steady_clock::now();
    auto elapsed = duration_cast<seconds>(endTime - startTime).count();

    clearScreen();
    cout << "\n" << string(60, '=') << "\n";
    cout << "                 QUIZ COMPLETED!\n";
    cout << string(60, '=') << "\n\n";
    
    cout << " Final Score: " << score << "/" << questions.size() << "\n";
    
    double percentage = ((double)score / questions.size()) * 100;
    cout << " Percentage: " << fixed << setprecision(1) << percentage << "%\n";
    cout << "  Time Taken: " << getTimeString(elapsed) << "\n\n";
    
    // Achievement badges
    if (percentage == 100) {
        cout << " PERFECT SCORE! You earned the 'Perfect Score' badge!\n";
    } else if (percentage >= 90) {
        cout << " EXCELLENT! You earned the 'Quiz Master' badge!\n";
    } else if (percentage >= 75) {
        cout << " GOOD JOB! You earned the 'Knowledge Seeker' badge!\n";
    } else if (percentage >= 50) {
        cout << " Keep practicing! You're improving!\n";
    } else {
        cout << " Don't give up! Practice makes perfect!\n";
    }

    // Show correct answers
    cout << "\n Detailed Review:\n";
    for (size_t i = 0; i < questions.size(); i++) {
        if (userAnswers[i] != questions[i].correctAnswer) {
            cout << "Q" << (i + 1) << ": Correct answer was " 
                 << (char)('A' + questions[i].correctAnswer - 1) << "\n";
        }
    }

    // Save attempt
    string insertQuery =
    "INSERT INTO attempts(user_id, quiz_id, score, time_taken, percentage) VALUES("
    + to_string(userId) + ","
    + to_string(quizId) + ","
    + to_string(score) + ","
    + to_string(elapsed) + ","
    + to_string(percentage) + ");";

    char* errMsg;
    sqlite3_exec(db, insertQuery.c_str(), NULL, NULL, &errMsg);
    
    if (errMsg) {
        ErrorHandler::logDatabaseError("saveAttempt", errMsg);
        sqlite3_free(errMsg);
    } else {
        cout << "\n Attempt saved successfully!\n";
    }
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
    
    return score;
}