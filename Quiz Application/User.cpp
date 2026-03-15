#include "User.h"
#include "sqlite3.h"
#include <iomanip>
#include <cstring>  // Add this for strlen
#include <algorithm>
#include <random>
#include "QuizEngine.h"
#include "Analytics.h"

using namespace std;

User::User(IDatabase* db, int id)
{
    database = db;
    userId = id;
}

void User::userMenu()
{
    int choice;

    do
    {
        cout << "\n" << string(50, '=') << "\n";
        cout << "        USER DASHBOARD\n";
        cout << string(50, '=') << "\n";
        cout << "1. Browse All Quizzes\n";
        cout << "2. Search Quizzes by Category\n";
        cout << "3. Search Quizzes by Difficulty\n";
        cout << "4. Take a Quiz\n";
        cout << "5. View My Quiz History\n";
        cout << "6. View My Performance\n";
        cout << "7. My Achievements\n";
        cout << "8. View Leaderboard\n";
        cout << "9. Logout\n";
        cout << string(50, '-') << "\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
                browseQuizzes();
                break;

            case 2:
                selectQuizByCategory();
                break;

            case 3:
                selectQuizByDifficulty();
                break;

            case 4:
                attemptQuiz();
                break;

            case 5:
                viewQuizHistory();
                break;

            case 6:
                viewPerformance();
                break;
                
            case 7:
                getAchievements();
                break;

            case 8: {
                Analytics analytics(database);
                analytics.showLeaderboard();
                break;
            }

            case 9:
                cout << "Logging out...\n";
                break;

            default:
                cout << "Invalid choice.\n";
        }

    } while(choice != 9);
}

void User::browseQuizzes()
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query = "SELECT quiz_id, title, description, category, difficulty, time_limit, "
                   "(SELECT COUNT(*) FROM questions WHERE quiz_id = quizzes.quiz_id) as question_count "
                   "FROM quizzes ORDER BY category, difficulty;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(100, '=') << "\n";
    cout << "                    AVAILABLE QUIZZES\n";
    cout << string(100, '=') << "\n";
    cout << left << setw(6) << "ID" << " | " << setw(30) << "Title" << " | " 
         << setw(15) << "Category" << " | " << setw(10) << "Difficulty" << " | " 
         << setw(8) << "Time" << " | " << "Questions\n";
    cout << string(100, '-') << "\n";

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        int quizId = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* description = (const char*)sqlite3_column_text(stmt, 2);
        const char* category = (const char*)sqlite3_column_text(stmt, 3);
        const char* difficulty = (const char*)sqlite3_column_text(stmt, 4);
        int timeLimit = sqlite3_column_int(stmt, 5);
        int questionCount = sqlite3_column_int(stmt, 6);
        
        string shortTitle = string(title).substr(0, 27) + (strlen(title) > 27 ? "..." : "");

        cout << left << setw(6) << quizId << " | " << setw(30) << shortTitle << " | " 
             << setw(15) << category << " | " << setw(10) << difficulty << " | " 
             << setw(8) << timeLimit << " min | " << questionCount << "\n";
    }

    cout << string(100, '=') << "\n";
    sqlite3_finalize(stmt);
}

void User::selectQuizByCategory()
{
    string category;

    cout << "\nEnter Category (e.g., C++, Python, Java): ";
    cin >> category;

    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query = "SELECT quiz_id, title, difficulty, time_limit, "
                   "(SELECT COUNT(*) FROM questions WHERE quiz_id = quizzes.quiz_id) as question_count "
                   "FROM quizzes WHERE category LIKE '%" + database->sanitizeInput(category) + "%';";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(80, '=') << "\n";
    cout << "        QUIZZES IN CATEGORY: " << category << "\n";
    cout << string(80, '=') << "\n";

    bool found = false;
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = true;
        int quizId = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* difficulty = (const char*)sqlite3_column_text(stmt, 2);
        int timeLimit = sqlite3_column_int(stmt, 3);
        int questionCount = sqlite3_column_int(stmt, 4);

        cout << "Quiz ID: " << quizId << "\n";
        cout << "Title: " << title << "\n";
        cout << "Difficulty: " << difficulty << "\n";
        cout << "Time Limit: " << timeLimit << " minutes\n";
        cout << "Questions: " << questionCount << "\n";
        cout << string(40, '-') << "\n";
    }

    if (!found) {
        cout << "No quizzes found in this category.\n";
    }

    sqlite3_finalize(stmt);
}

void User::selectQuizByDifficulty()
{
    string difficulty;

    cout << "\nEnter Difficulty (Easy/Medium/Hard): ";
    cin >> difficulty;

    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query = "SELECT quiz_id, title, category, time_limit, "
                   "(SELECT COUNT(*) FROM questions WHERE quiz_id = quizzes.quiz_id) as question_count "
                   "FROM quizzes WHERE difficulty='" + database->sanitizeInput(difficulty) + "';";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(80, '=') << "\n";
    cout << "        " << difficulty << " QUIZZES\n";
    cout << string(80, '=') << "\n";

    bool found = false;
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = true;
        int quizId = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* category = (const char*)sqlite3_column_text(stmt, 2);
        int timeLimit = sqlite3_column_int(stmt, 3);
        int questionCount = sqlite3_column_int(stmt, 4);

        cout << "Quiz ID: " << quizId << "\n";
        cout << "Title: " << title << "\n";
        cout << "Category: " << category << "\n";
        cout << "Time Limit: " << timeLimit << " minutes\n";
        cout << "Questions: " << questionCount << "\n";
        cout << string(40, '-') << "\n";
    }

    if (!found) {
        cout << "No quizzes found with this difficulty.\n";
    }

    sqlite3_finalize(stmt);
}

void User::attemptQuiz()
{
    int quizId;

    browseQuizzes();

    cout << "\nEnter Quiz ID to attempt (0 to cancel): ";
    cin >> quizId;
    
    if (quizId == 0) {
        return;
    }

    // Check if quiz has questions
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;
    
    string checkQuery = "SELECT COUNT(*) FROM questions WHERE quiz_id = " + to_string(quizId) + ";";
    sqlite3_prepare_v2(db, checkQuery.c_str(), -1, &stmt, NULL);
    
    int questionCount = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        questionCount = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    
    if (questionCount == 0) {
        cout << "This quiz has no questions yet.\n";
        return;
    }

    // Use QuizEngine instead of implementing here
    QuizEngine engine(database);
    int score = engine.startQuiz(quizId, userId);
    
    if (score > 0) {
        checkAndAwardAchievements();
    }
}

void User::viewQuizHistory()
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query =
    "SELECT a.attempt_id, q.title, a.score, a.time_taken, a.date "
    "FROM attempts a "
    "JOIN quizzes q ON a.quiz_id = q.quiz_id "
    "WHERE a.user_id=" + to_string(userId) + " "
    "ORDER BY a.date DESC;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(100, '=') << "\n";
    cout << "                    YOUR QUIZ HISTORY\n";
    cout << string(100, '=') << "\n";
    cout << left << setw(10) << "Attempt ID" << " | " << setw(35) << "Quiz Title" 
         << " | " << setw(8) << "Score" << " | " << setw(10) << "Time Taken" << " | " << "Date\n";
    cout << string(100, '-') << "\n";

    bool found = false;
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = true;
        int attemptId = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        int score = sqlite3_column_int(stmt, 2);
        int timeTaken = sqlite3_column_int(stmt, 3);
        const char* date = (const char*)sqlite3_column_text(stmt, 4);
        
        string shortTitle = string(title).substr(0, 32) + (strlen(title) > 32 ? "..." : "");
        string timeStr = to_string(timeTaken / 60) + "m " + to_string(timeTaken % 60) + "s";

        cout << left << setw(10) << attemptId << " | " << setw(35) << shortTitle 
             << " | " << setw(8) << score << "% | " << setw(10) << timeStr << " | " << date << "\n";
    }

    if (!found) {
        cout << "No quiz history found.\n";
    }

    cout << string(100, '=') << "\n";
    sqlite3_finalize(stmt);
}

void User::viewPerformance()
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query =
    "SELECT COUNT(*), COALESCE(AVG(score), 0), COALESCE(MAX(score), 0), COALESCE(MIN(score), 0) "
    "FROM attempts WHERE user_id=" + to_string(userId) + ";";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(50, '=') << "\n";
    cout << "        YOUR PERFORMANCE\n";
    cout << string(50, '=') << "\n";

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        int totalAttempts = sqlite3_column_int(stmt, 0);
        double avgScore = sqlite3_column_double(stmt, 1);
        int bestScore = sqlite3_column_int(stmt, 2);
        int worstScore = sqlite3_column_int(stmt, 3);
        
        cout << " Statistics:\n";
        cout << "  Total Quizzes Taken: " << totalAttempts << "\n";
        cout << "  Average Score: " << fixed << setprecision(1) << avgScore << "%\n";
        cout << "  Best Score: " << bestScore << "%\n";
        cout << "  Worst Score: " << worstScore << "%\n";
        
        if (totalAttempts > 0) {
            // Category performance
            string catQuery = "SELECT q.category, AVG(a.score) as avg_score, COUNT(*) as attempts "
                             "FROM attempts a "
                             "JOIN quizzes q ON a.quiz_id = q.quiz_id "
                             "WHERE a.user_id = " + to_string(userId) + " "
                             "GROUP BY q.category "
                             "ORDER BY avg_score DESC;";
            
            sqlite3_stmt* catStmt;
            sqlite3_prepare_v2(db, catQuery.c_str(), -1, &catStmt, NULL);
            
            cout << "\n Performance by Category:\n";
            while (sqlite3_step(catStmt) == SQLITE_ROW) {
                const char* category = (const char*)sqlite3_column_text(catStmt, 0);
                double catAvg = sqlite3_column_double(catStmt, 1);
                int catAttempts = sqlite3_column_int(catStmt, 2);
                
                cout << "  " << category << ": " << fixed << setprecision(1) << catAvg 
                     << "% (" << catAttempts << " attempts)\n";
            }
            sqlite3_finalize(catStmt);
        }
    }

    sqlite3_finalize(stmt);
}

void User::checkAndAwardAchievements()
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;
    
    // Check total attempts
    string attemptQuery = "SELECT COUNT(*) FROM attempts WHERE user_id = " + to_string(userId) + ";";
    sqlite3_prepare_v2(db, attemptQuery.c_str(), -1, &stmt, NULL);
    
    int totalAttempts = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        totalAttempts = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    
    // Check perfect scores
    string perfectQuery = "SELECT COUNT(*) FROM attempts WHERE user_id = " + to_string(userId) + " AND score = 100;";
    sqlite3_prepare_v2(db, perfectQuery.c_str(), -1, &stmt, NULL);
    
    int perfectScores = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        perfectScores = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    
    // Award achievements
    vector<string> newAchievements;
    
    if (totalAttempts >= 1) newAchievements.push_back("First Quiz");
    if (totalAttempts >= 5) newAchievements.push_back("Quiz Enthusiast");
    if (totalAttempts >= 10) newAchievements.push_back("Quiz Master");
    if (perfectScores >= 1) newAchievements.push_back("Perfect Score");
    if (perfectScores >= 3) newAchievements.push_back("Perfectionist");
    
    // Store achievements (simplified - in real app would store in database)
    if (!newAchievements.empty()) {
        cout << "\n New Achievements Unlocked!\n";
        for (const auto& achievement : newAchievements) {
            cout << "  • " << achievement << "\n";
        }
    }
}

void User::getAchievements()
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;
    
    // Get stats
    string statsQuery = "SELECT COUNT(*), MAX(score) FROM attempts WHERE user_id = " + to_string(userId) + ";";
    sqlite3_prepare_v2(db, statsQuery.c_str(), -1, &stmt, NULL);
    
    int totalAttempts = 0;
    int bestScore = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        totalAttempts = sqlite3_column_int(stmt, 0);
        bestScore = sqlite3_column_int(stmt, 1);
    }
    sqlite3_finalize(stmt);
    
    cout << "\n" << string(50, '=') << "\n";
    cout << "        YOUR ACHIEVEMENTS\n";
    cout << string(50, '=') << "\n";
    
    vector<pair<string, bool>> achievements = {
        {"First Quiz", totalAttempts >= 1},
        {"Quiz Enthusiast", totalAttempts >= 5},
        {"Quiz Master", totalAttempts >= 10},
        {"Perfect Score", bestScore == 100},
        {"Knowledge Seeker", totalAttempts >= 3},
        {"Speed Demon", false} // Would need time-based tracking
    };
    
    for (const auto& achievement : achievements) {
        cout << (achievement.second ? " tick" : "no tick ") << achievement.first << "\n";
    }
    
    cout << "\n Progress:\n";
    cout << "  Total Quizzes: " << totalAttempts << "/10 for Quiz Master\n";
    cout << "  Best Score: " << bestScore << "% (100% needed for Perfect Score)\n";
}