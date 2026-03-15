#include "Analytics.h"
#include "sqlite3.h"
#include <iomanip>

using namespace std;

Analytics::Analytics(IDatabase* db)
{
    database = db;
}

void Analytics::displayProgressBar(double percentage)
{
    int barWidth = 50;
    cout << "[";
    int pos = barWidth * percentage / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << fixed << setprecision(1) << percentage << "%\n";
}

void Analytics::showLeaderboard()
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query =
    "SELECT users.username, MAX(attempts.score) as best_score, "
    "COUNT(attempts.attempt_id) as attempts, "
    "AVG(attempts.score) as avg_score "
    "FROM attempts "
    "JOIN users ON attempts.user_id = users.id "
    "WHERE users.role = 'user' "
    "GROUP BY users.username "
    "ORDER BY best_score DESC, avg_score DESC "
    "LIMIT 10;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(70, '=') << "\n";
    cout << "                 LEADERBOARD\n";
    cout << string(70, '=') << "\n";
    cout << left << setw(5) << "Rank" << " | " << setw(20) << "Username" 
         << " | " << setw(10) << "Best Score" << " | " << setw(8) << "Attempts" << " | " << "Avg Score\n";
    cout << string(70, '-') << "\n";

    int rank = 1;

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* username = (const char*)sqlite3_column_text(stmt, 0);
        int bestScore = sqlite3_column_int(stmt, 1);
        int attempts = sqlite3_column_int(stmt, 2);
        double avgScore = sqlite3_column_double(stmt, 3);
        
        string medal = "  ";
        if (rank == 1) medal = "Gold";
        else if (rank == 2) medal = "Silver";
        else if (rank == 3) medal = "Bronze";
        
        cout << left << setw(5) << (medal + " " + to_string(rank)) 
             << " | " << setw(20) << username 
             << " | " << setw(10) << bestScore << "%"
             << " | " << setw(8) << attempts 
             << " | " << fixed << setprecision(1) << avgScore << "%\n";

        rank++;
    }

    cout << string(70, '=') << "\n";
    sqlite3_finalize(stmt);
}

void Analytics::userAnalytics(int userId)
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query =
    "SELECT COUNT(*), COALESCE(AVG(score), 0), COALESCE(MAX(score), 0), COALESCE(MIN(score), 0) "
    "FROM attempts WHERE user_id=" + to_string(userId) + ";";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(50, '=') << "\n";
    cout << "        USER PERFORMANCE ANALYTICS\n";
    cout << string(50, '=') << "\n";

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        int totalAttempts = sqlite3_column_int(stmt, 0);
        double avgScore = sqlite3_column_double(stmt, 1);
        int bestScore = sqlite3_column_int(stmt, 2);
        int worstScore = sqlite3_column_int(stmt, 3);
        
        cout << "Statistics:\n";
        cout << "  Total Quizzes Attempted: " << totalAttempts << "\n";
        cout << "  Average Score: " << fixed << setprecision(1) << avgScore << "%\n";
        cout << "  Best Score: " << bestScore << "%\n";
        cout << "  Worst Score: " << worstScore << "%\n\n";
        
        if (totalAttempts > 0) {
            cout << " Performance Trend:\n";
            displayProgressBar(avgScore);
        }
    }

    sqlite3_finalize(stmt);

    bestCategory(userId);
    weakCategory(userId);
    
    // Show recent attempts
    string recentQuery = "SELECT q.title, a.score, a.date "
                        "FROM attempts a "
                        "JOIN quizzes q ON a.quiz_id = q.quiz_id "
                        "WHERE a.user_id = " + to_string(userId) + " "
                        "ORDER BY a.date DESC LIMIT 5;";
    
    sqlite3_prepare_v2(db, recentQuery.c_str(), -1, &stmt, NULL);
    
    cout << "\n Recent Attempts:\n";
    cout << string(50, '-') << "\n";
    
    bool hasRecent = false;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        hasRecent = true;
        const char* title = (const char*)sqlite3_column_text(stmt, 0);
        int score = sqlite3_column_int(stmt, 1);
        const char* date = (const char*)sqlite3_column_text(stmt, 2);
        
        cout << "  Quiz: " << title << "\n";
        cout << "  Score: " << score << "% on " << date << "\n\n";
    }
    
    if (!hasRecent) {
        cout << "  No attempts yet.\n";
    }
    
    sqlite3_finalize(stmt);
}

void Analytics::bestCategory(int userId)
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query =
    "SELECT quizzes.category, COALESCE(AVG(attempts.score), 0) "
    "FROM attempts "
    "JOIN quizzes ON attempts.quiz_id = quizzes.quiz_id "
    "WHERE attempts.user_id=" + to_string(userId) +
    " GROUP BY quizzes.category "
    "ORDER BY AVG(attempts.score) DESC LIMIT 1;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << " Best Category: " << (const char*)sqlite3_column_text(stmt, 0)
             << " (" << fixed << setprecision(1) << sqlite3_column_double(stmt, 1) << "%)\n";
    }

    sqlite3_finalize(stmt);
}

void Analytics::weakCategory(int userId)
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query =
    "SELECT quizzes.category, COALESCE(AVG(attempts.score), 0) "
    "FROM attempts "
    "JOIN quizzes ON attempts.quiz_id = quizzes.quiz_id "
    "WHERE attempts.user_id=" + to_string(userId) +
    " AND attempts.score > 0 "
    "GROUP BY quizzes.category "
    "ORDER BY AVG(attempts.score) ASC LIMIT 1;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        cout << " Needs Improvement: " << (const char*)sqlite3_column_text(stmt, 0)
             << " (" << fixed << setprecision(1) << sqlite3_column_double(stmt, 1) << "%)\n";
    }

    sqlite3_finalize(stmt);
}

void Analytics::showGlobalStats()
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query =
    "SELECT COUNT(DISTINCT user_id) as active_users, "
    "COUNT(*) as total_attempts, "
    "COALESCE(AVG(score), 0) as global_avg, "
    "COALESCE(MAX(score), 0) as highest_score "
    "FROM attempts;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);

    cout << "\n" << string(50, '=') << "\n";
    cout << "        GLOBAL STATISTICS\n";
    cout << string(50, '=') << "\n";

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        int activeUsers = sqlite3_column_int(stmt, 0);
        int totalAttempts = sqlite3_column_int(stmt, 1);
        double globalAvg = sqlite3_column_double(stmt, 2);
        int highestScore = sqlite3_column_int(stmt, 3);
        
        cout << "Active Users: " << activeUsers << "\n";
        cout << "Total Quiz Attempts: " << totalAttempts << "\n";
        cout << "Global Average Score: " << fixed << setprecision(1) << globalAvg << "%\n";
        cout << "Highest Score Ever: " << highestScore << "%\n";
    }

    sqlite3_finalize(stmt);
    
    // Category popularity
    string catQuery = "SELECT category, COUNT(*) as quiz_count, "
                      "COALESCE((SELECT AVG(score) FROM attempts a JOIN quizzes q2 ON a.quiz_id = q2.quiz_id WHERE q2.category = quizzes.category), 0) as avg_score "
                      "FROM quizzes "
                      "GROUP BY category "
                      "ORDER BY quiz_count DESC LIMIT 5;";
    
    sqlite3_prepare_v2(db, catQuery.c_str(), -1, &stmt, NULL);
    
    cout << "\n Popular Categories:\n";
    cout << string(50, '-') << "\n";
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* category = (const char*)sqlite3_column_text(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);
        double avgScore = sqlite3_column_double(stmt, 2);
        
        cout << "  " << category << ": " << count << " quizzes, "
             << fixed << setprecision(1) << avgScore << "% avg score\n";
    }
    
    sqlite3_finalize(stmt);
}