#include "Admin.h"
#include "sqlite3.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstring> 
#include <algorithm> 

using namespace std;

Admin::Admin(IDatabase* db, int id)
{
    database = db;
    adminId = id;
}

void Admin::adminMenu()
{
    int choice;

    do
    {
        cout << "\n" << string(50, '=') << "\n";
        cout << "        ADMIN DASHBOARD\n";
        cout << string(50, '=') << "\n";
        cout << "|------------------------------------------------|\n";
        cout << "|  QUIZ MANAGEMENT                               |\n";
        cout << "|------------------------------------------------|\n";
        cout << "|  1. Create New Quiz                            |\n";
        cout << "|  2. View My Quizzes                            |\n";
        cout << "|  3. Edit Quiz                                  |\n";
        cout << "|  4. Delete Quiz                                |\n";
        cout << "|  5. Clone Quiz                                 |\n";
        cout << "|------------------------------------------------|\n";
        cout << "|  QUESTION MANAGEMENT                           |\n";
        cout << "|------------------------------------------------|\n";
        cout << "|  6. Add Question to Quiz                       |\n";
        cout << "|  7. View Questions in Quiz                     |\n";
        cout << "|  8. Edit Question                              |\n";
        cout << "|  9. Delete Question                            |\n";
        cout << "| 10. Search Question Bank                       |\n";
        cout << "|------------------------------------------------|\n";
        cout << "|  USER MANAGEMENT                               |\n";
        cout << "|------------------------------------------------|\n";
        cout << "| 11. View All Users                             |\n";
        cout << "| 12. Promote User to Admin                      |\n";
        cout << "| 13. Delete User                                |\n";
        cout << "|------------------------------------------------|\n";
        cout << "|  ANALYTICS & EXPORT                            |\n";
        cout << "|------------------------------------------------|\n";
        cout << "| 14. View Quiz Analytics                        |\n";
        cout << "| 15. Export Quiz Results                        |\n";
        cout << "|------------------------------------------------|\n";
        cout << "|  SYSTEM                                        |\n";
        cout << "|------------------------------------------------|\n";
        cout << "| 16. View System Logs                           |\n";
        cout << "| 17. Backup Database                            |\n";
        cout << "| 18. Help                                       |\n";
        cout << "| 19. Logout                                     |\n";
        cout << "|------------------------------------------------|\n";
        cout << "Enter your choice: ";
        cin >> choice;

        try {
            switch (choice)
            {
                case 1: createQuiz(); break;
                case 2: viewAllQuizzes(); break;
                case 3: editQuiz(); break;
                case 4: deleteQuiz(); break;
                case 5: cloneQuiz(); break;
                case 6: addQuestion(); break;
                case 7: viewQuestionsByQuizId(); break;
                case 8: editQuestion(); break;
                case 9: deleteQuestion(); break;
                case 10: searchQuestionBank(); break;
                case 11: viewAllUsers(); break;
                case 12: promoteUserToAdmin(); break;
                case 13: deleteUser(); break;
                case 14: viewQuizAnalytics(); break;
                case 15: exportQuizResults(); break;
                case 16: viewSystemLogs(); break;
                case 17: backupDatabase(); break;
                case 18: showHelp(); break;
                case 19: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice! Please try again.\n";
            }
        }
        catch (const exception& e) {
            ErrorHandler::logError(string("Admin menu error: ") + e.what(), ErrorHandler::ERROR_LEVEL);
            cout << "\nAn error occurred. Please try again.\n";
        }

    } while (choice != 19);
}

void Admin::showHelp()
{
    cout << "\n" << string(50, '=') << "\n";
    cout << "                HELP & TIPS\n";
    cout << string(50, '=') << "\n";
    cout << "CREATING QUIZZES:\n";
    cout << "  - Give clear, descriptive titles\n";
    cout << "  - Categorize properly (e.g., C++, Python, History)\n";
    cout << "  - Set appropriate time limits\n\n";
    
    cout << "ADDING QUESTIONS:\n";
    cout << "  - Write clear questions\n";
    cout << "  - Make options distinct\n";
    cout << "  - Add explanations for learning\n\n";
    
    cout << "MANAGING USERS:\n";
    cout << "  - Monitor user performance\n";
    cout << "  - Promote active users to admins\n";
    cout << "  - Remove inactive/spam accounts\n\n";
    
    cout << "ANALYTICS:\n";
    cout << "  - Track quiz popularity\n";
    cout << "  - Identify difficult questions\n";
    cout << "  - Export data for analysis\n\n";
    
    cout << "Press Enter to continue...";
    cin.ignore();
    cin.get();
}

bool Admin::verifyQuizOwnership(int quizId)
{
    sqlite3_stmt* stmt;
    string query = "SELECT admin_id FROM quizzes WHERE quiz_id = " + to_string(quizId) + ";";
    
    stmt = database->prepareStatement(query);
    if (!stmt) return false;
    
    bool owns = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int ownerId = sqlite3_column_int(stmt, 0);
        owns = (ownerId == adminId);
    }
    
    sqlite3_finalize(stmt);
    
    if (!owns) {
        cout << "\nYou don't have permission to modify this quiz.\n";
    }
    
    return owns;
}

void Admin::createQuiz()
{
    string title, description, category, difficulty;
    int timeLimit;

    cout << "\n" << string(50, '=') << "\n";
    cout << "        CREATE NEW QUIZ\n";
    cout << string(50, '=') << "\n";
    
    cout << "Enter Quiz Title: ";
    cin.ignore();
    getline(cin, title);

    if (title.empty()) {
        cout << "Title cannot be empty. Quiz creation cancelled.\n";
        return;
    }

    cout << "Enter Description: ";
    getline(cin, description);

    cout << "Enter Category (e.g., C++, Python, Java, History): ";
    getline(cin, category);

    cout << "Enter Difficulty Level (Easy/Medium/Hard): ";
    getline(cin, difficulty);
    
    while (difficulty != "Easy" && difficulty != "Medium" && difficulty != "Hard") {
        cout << "Invalid difficulty. Please enter Easy, Medium, or Hard: ";
        getline(cin, difficulty);
    }

    cout << "Enter Time Limit in minutes: ";
    cin >> timeLimit;
    
    while (timeLimit < 1 || timeLimit > 180) {
        cout << "Time limit must be between 1 and 180 minutes: ";
        cin >> timeLimit;
    }

    string query = "INSERT INTO quizzes(admin_id, title, description, category, difficulty, time_limit) "
                   "VALUES(" + to_string(adminId) + ", '" + database->sanitizeInput(title) + "', '" + 
                   database->sanitizeInput(description) + "', '" + 
                   database->sanitizeInput(category) + "', '" + 
                   database->sanitizeInput(difficulty) + "', " + to_string(timeLimit) + ");";

    if(database->executeQuery(query))
    {
        int newQuizId = database->getLastInsertRowId();
        cout << "\nQuiz created successfully with ID: " << newQuizId << "\n";
        cout << "You can now add questions to this quiz.\n";
        
        ErrorHandler::logError("Admin " + to_string(adminId) + " created quiz: " + title, ErrorHandler::INFO);
    }
    else
    {
        cout << "\n Failed to create quiz.\n";
    }
}

void Admin::viewAllQuizzes()
{
    cout << "\n" << string(100, '=') << "\n";
    cout << "                    MY QUIZZES\n";
    cout << string(100, '=') << "\n";

    sqlite3_stmt* stmt;

    string query = "SELECT quizzes.quiz_id, title, description, category, difficulty, time_limit, "
                   "COUNT(q.question_id) as question_count, "
                   "(SELECT COUNT(*) FROM attempts WHERE quiz_id = quizzes.quiz_id) as attempt_count, "
                   "COALESCE((SELECT AVG(score) FROM attempts WHERE quiz_id = quizzes.quiz_id), 0) as avg_score "
                   "FROM quizzes "
                   "LEFT JOIN questions q ON quizzes.quiz_id = q.quiz_id "
                   "WHERE admin_id = " + to_string(adminId) + " "
                   "GROUP BY quizzes.quiz_id ORDER BY quizzes.quiz_id DESC;";

    stmt = database->prepareStatement(query);

    if(!stmt)
    {
        cout << "Failed to retrieve quizzes.\n";
        return;
    }

    bool found = false;
    cout << "\n" << left;
    cout << setw(5) << "ID" << " | " 
         << setw(35) << "Title" << " | " 
         << setw(15) << "Category" << " | " 
         << setw(10) << "Difficulty" << " | " 
         << setw(8) << "Time" << " | " 
         << setw(9) << "Questions" << " | " 
         << setw(8) << "Attempts" << " | " 
         << "Avg Score\n";
    cout << string(120, '-') << "\n";

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = true;
        int quizId = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* description = (const char*)sqlite3_column_text(stmt, 2);
        const char* category = (const char*)sqlite3_column_text(stmt, 3);
        const char* difficulty = (const char*)sqlite3_column_text(stmt, 4);
        int timeLimit = sqlite3_column_int(stmt, 5);
        int qCount = sqlite3_column_int(stmt, 6);
        int attemptCount = sqlite3_column_int(stmt, 7);
        double avgScore = sqlite3_column_double(stmt, 8);

        string shortTitle = string(title).substr(0, 32) + (strlen(title) > 32 ? "..." : "");
        
        cout << setw(5) << quizId << " | " 
             << setw(35) << shortTitle << " | " 
             << setw(15) << category << " | " 
             << setw(10) << difficulty << " | " 
             << setw(8) << timeLimit << " | " 
             << setw(9) << qCount << " | " 
             << setw(8) << attemptCount << " | " 
             << fixed << setprecision(1) << avgScore << "%\n";
    }

    cout << string(120, '-') << "\n";

    if(!found)
    {
        cout << "You haven't created any quizzes yet.\n";
    }

    sqlite3_finalize(stmt);
}

void Admin::editQuiz()
{
    int quizId;
    string newTitle, newDescription, newCategory, newDifficulty;
    int newTimeLimit = 0;

    cout << "\n" << string(50, '=') << "\n";
    cout << "        EDIT QUIZ\n";
    cout << string(50, '=') << "\n";

    viewAllQuizzes();

    cout << "\nEnter Quiz ID to edit (0 to cancel): ";
    cin >> quizId;
    
    if (quizId == 0) {
        cout << "Edit cancelled.\n";
        return;
    }

    if (!verifyQuizOwnership(quizId)) {
        return;
    }

    cout << "\n(Leave blank to keep current value)\n";
    cout << "Enter New Title: ";
    cin.ignore();
    getline(cin, newTitle);

    cout << "Enter New Description: ";
    getline(cin, newDescription);

    cout << "Enter New Category: ";
    getline(cin, newCategory);

    cout << "Enter New Difficulty (Easy/Medium/Hard): ";
    getline(cin, newDifficulty);

    if (!newDifficulty.empty() && 
        newDifficulty != "Easy" && newDifficulty != "Medium" && newDifficulty != "Hard") {
        cout << "Invalid difficulty. Keeping original.\n";
        newDifficulty = "";
    }

    cout << "Enter New Time Limit in minutes (0 to keep current): ";
    cin >> newTimeLimit;

    // Build dynamic update query
    string query = "UPDATE quizzes SET ";
    bool first = true;

    if (!newTitle.empty()) {
        query += "title = '" + database->sanitizeInput(newTitle) + "'";
        first = false;
    }
    if (!newDescription.empty()) {
        if (!first) query += ", ";
        query += "description = '" + database->sanitizeInput(newDescription) + "'";
        first = false;
    }
    if (!newCategory.empty()) {
        if (!first) query += ", ";
        query += "category = '" + database->sanitizeInput(newCategory) + "'";
        first = false;
    }
    if (!newDifficulty.empty()) {
        if (!first) query += ", ";
        query += "difficulty = '" + database->sanitizeInput(newDifficulty) + "'";
        first = false;
    }
    if (newTimeLimit > 0) {
        if (!first) query += ", ";
        query += "time_limit = " + to_string(newTimeLimit);
        first = false;
    }

    if (first) {
        cout << "\nNo changes made.\n";
        return;
    }

    query += " WHERE quiz_id = " + to_string(quizId) + " AND admin_id = " + to_string(adminId) + ";";

    if(database->executeQuery(query))
    {
        cout << "\n Quiz updated successfully\n";
        ErrorHandler::logError("Admin " + to_string(adminId) + " updated quiz ID: " + to_string(quizId), ErrorHandler::INFO);
    }
    else
    {
        cout << "\n Failed to update quiz\n";
    }
}

void Admin::deleteQuiz()
{
    int quizId;

    cout << "\n" << string(50, '=') << "\n";
    cout << "        DELETE QUIZ\n";
    cout << string(50, '=') << "\n";

    viewAllQuizzes();

    cout << "\nEnter Quiz ID to delete (0 to cancel): ";
    cin >> quizId;
    
    if (quizId == 0) {
        cout << "Deletion cancelled.\n";
        return;
    }

    if (!verifyQuizOwnership(quizId)) {
        return;
    }

    cout << "\n WARNING: This will delete ALL questions and attempt records for this quiz!\n";
    cout << "This action CANNOT be undone.\n";
    cout << "Type 'DELETE' to confirm: ";
    string confirm;
    cin >> confirm;

    if(confirm == "DELETE")
    {
        // Start transaction
        database->executeQuery("BEGIN TRANSACTION;");
        
        bool success = true;
        
        // Delete attempts first
        string deleteAttemptsQuery = "DELETE FROM attempts WHERE quiz_id = " + to_string(quizId) + ";";
        success = success && database->executeQuery(deleteAttemptsQuery);
        
        // Delete questions
        string deleteQuestionsQuery = "DELETE FROM questions WHERE quiz_id = " + to_string(quizId) + ";";
        success = success && database->executeQuery(deleteQuestionsQuery);
        
        // Delete quiz
        string deleteQuizQuery = "DELETE FROM quizzes WHERE quiz_id = " + to_string(quizId) + 
                                 " AND admin_id = " + to_string(adminId) + ";";
        success = success && database->executeQuery(deleteQuizQuery);

        if(success)
        {
            database->executeQuery("COMMIT;");
            cout << "\n Quiz and all related data deleted successfully\n";
            ErrorHandler::logError("Admin " + to_string(adminId) + " deleted quiz ID: " + to_string(quizId), ErrorHandler::INFO);
        }
        else
        {
            database->executeQuery("ROLLBACK;");
            cout << "\n Failed to delete quiz\n";
        }
    }
    else
    {
        cout << "Deletion cancelled.\n";
    }
}

void Admin::cloneQuiz()
{
    int quizId;
    
    cout << "\n" << string(50, '=') << "\n";
    cout << "        CLONE QUIZ\n";
    cout << string(50, '=') << "\n";
    
    viewAllQuizzes();
    
    cout << "\nEnter Quiz ID to clone (0 to cancel): ";
    cin >> quizId;
    
    if (quizId == 0) {
        cout << "Clone cancelled.\n";
        return;
    }
    
    if (!verifyQuizOwnership(quizId)) {
        return;
    }
    
    string newTitle;
    cout << "Enter new title for cloned quiz: ";
    cin.ignore();
    getline(cin, newTitle);
    
    if (newTitle.empty()) {
        cout << "Title cannot be empty. Clone cancelled.\n";
        return;
    }
    
    // Start transaction
    database->executeQuery("BEGIN TRANSACTION;");
    
    // Get original quiz details
    sqlite3_stmt* stmt;
    string query = "SELECT title, description, category, difficulty, time_limit FROM quizzes WHERE quiz_id = " + to_string(quizId) + ";";
    stmt = database->prepareStatement(query);
    
    if (!stmt || sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "Failed to fetch quiz details.\n";
        database->executeQuery("ROLLBACK;");
        if (stmt) sqlite3_finalize(stmt);
        return;
    }
    
    string description = (const char*)sqlite3_column_text(stmt, 1);
    string category = (const char*)sqlite3_column_text(stmt, 2);
    string difficulty = (const char*)sqlite3_column_text(stmt, 3);
    int timeLimit = sqlite3_column_int(stmt, 4);
    
    sqlite3_finalize(stmt);
    
    // Create new quiz
    string insertQuiz = "INSERT INTO quizzes(admin_id, title, description, category, difficulty, time_limit) "
                        "VALUES(" + to_string(adminId) + ", '" + database->sanitizeInput(newTitle) + "', '" +
                        database->sanitizeInput(description) + "', '" + 
                        database->sanitizeInput(category) + "', '" + 
                        database->sanitizeInput(difficulty) + "', " + to_string(timeLimit) + ");";
    
    if (!database->executeQuery(insertQuiz)) {
        cout << "Failed to create cloned quiz.\n";
        database->executeQuery("ROLLBACK;");
        return;
    }
    
    int newQuizId = database->getLastInsertRowId();
    
    // Clone questions
    string cloneQuestions = "INSERT INTO questions(quiz_id, question_text, option1, option2, option3, option4, correct_answer, explanation) "
                           "SELECT " + to_string(newQuizId) + ", question_text, option1, option2, option3, option4, correct_answer, explanation "
                           "FROM questions WHERE quiz_id = " + to_string(quizId) + ";";
    
    if (!database->executeQuery(cloneQuestions)) {
        cout << "Failed to clone questions.\n";
        database->executeQuery("ROLLBACK;");
        return;
    }
    
    database->executeQuery("COMMIT;");
    cout << "\n Quiz cloned successfully! New Quiz ID: " << newQuizId << "\n";
}

void Admin::addQuestion()
{
    int quizId;

    cout << "\n" << string(50, '=') << "\n";
    cout << "        ADD QUESTION TO QUIZ\n";
    cout << string(50, '=') << "\n";

    viewAllQuizzes();

    cout << "\nEnter Quiz ID to add question to (0 to cancel): ";
    cin >> quizId;
    
    if (quizId == 0) {
        cout << "Operation cancelled.\n";
        return;
    }

    if (!verifyQuizOwnership(quizId)) {
        return;
    }

    bool addMore = true;
    while(addMore)
    {
        string question, opt1, opt2, opt3, opt4, explanation;
        int correctOption;

        cout << "\n--- Question " << (addMore ? "Details" : "") << " ---\n";
        
        cout << "Enter Question Text: ";
        cin.ignore();
        getline(cin, question);
        
        if (question.empty()) {
            cout << "Question cannot be empty. Skipping...\n";
            continue;
        }

        cout << "\nOption A: ";
        getline(cin, opt1);
        cout << "Option B: ";
        getline(cin, opt2);
        cout << "Option C: ";
        getline(cin, opt3);
        cout << "Option D: ";
        getline(cin, opt4);

        if (opt1.empty() || opt2.empty() || opt3.empty() || opt4.empty()) {
            cout << "All options must be filled. Skipping...\n";
            continue;
        }

        cout << "\nEnter Correct Option (1=A, 2=B, 3=C, 4=D): ";
        cin >> correctOption;

        while(correctOption < 1 || correctOption > 4)
        {
            cout << "Invalid option! Please enter 1-4: ";
            cin >> correctOption;
        }

        cout << "Enter Explanation (optional): ";
        cin.ignore();
        getline(cin, explanation);

        string query = "INSERT INTO questions(quiz_id, question_text, option1, option2, option3, option4, correct_answer, explanation) "
                       "VALUES(" + to_string(quizId) + ", '" + database->sanitizeInput(question) + "', '" + 
                       database->sanitizeInput(opt1) + "', '" + database->sanitizeInput(opt2) + "', '" + 
                       database->sanitizeInput(opt3) + "', '" + database->sanitizeInput(opt4) + "', " + 
                       to_string(correctOption) + ", '" + database->sanitizeInput(explanation) + "');";

        if(database->executeQuery(query))
        {
            int questionId = database->getLastInsertRowId();
            cout << "\n Question added successfully with ID: " << questionId << "\n";
        }
        else
        {
            cout << "\nFailed to add question\n";
        }

        cout << "\nAdd another question? (yes/no): ";
        string choice;
        cin >> choice;
        addMore = (choice == "yes" || choice == "YES");
    }

    cout << "\nQuestion addition completed.\n";
}

void Admin::viewQuestionsByQuizId()
{
    int quizId;

    cout << "\n" << string(50, '=') << "\n";
    cout << "        VIEW QUESTIONS\n";
    cout << string(50, '=') << "\n";

    viewAllQuizzes();

    cout << "\nEnter Quiz ID to view questions (0 to cancel): ";
    cin >> quizId;
    
    if (quizId == 0) {
        return;
    }

    if (!verifyQuizOwnership(quizId)) {
        return;
    }

    sqlite3_stmt* stmt;

    // Get quiz title first
    string quizTitleQuery = "SELECT title FROM quizzes WHERE quiz_id = " + to_string(quizId) + ";";
    stmt = database->prepareStatement(quizTitleQuery);

    if(!stmt || sqlite3_step(stmt) != SQLITE_ROW)
    {
        cout << "Quiz not found.\n";
        if(stmt) sqlite3_finalize(stmt);
        return;
    }

    cout << "\nQuiz: " << (const char*)sqlite3_column_text(stmt, 0) << "\n";
    sqlite3_finalize(stmt);

    // Get all questions
    string questionQuery = "SELECT question_id, question_text, option1, option2, option3, option4, correct_answer, explanation "
                          "FROM questions WHERE quiz_id = " + to_string(quizId) + " ORDER BY question_id;";

    stmt = database->prepareStatement(questionQuery);

    if(!stmt)
    {
        cout << "Failed to retrieve questions.\n";
        return;
    }

    cout << "\n" << string(120, '=') << "\n";

    bool found = false;
    int qNum = 1;

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = true;
        int questionId = sqlite3_column_int(stmt, 0);
        const char* questionText = (const char*)sqlite3_column_text(stmt, 1);
        const char* optA = (const char*)sqlite3_column_text(stmt, 2);
        const char* optB = (const char*)sqlite3_column_text(stmt, 3);
        const char* optC = (const char*)sqlite3_column_text(stmt, 4);
        const char* optD = (const char*)sqlite3_column_text(stmt, 5);
        int correctAnswer = sqlite3_column_int(stmt, 6);
        const char* explanation = (const char*)sqlite3_column_text(stmt, 7);

        cout << "Question ID: " << questionId << "\n";
        cout << "Q" << qNum << ": " << questionText << "\n";
        cout << "  A) " << optA << "\n";
        cout << "  B) " << optB << "\n";
        cout << "  C) " << optC << "\n";
        cout << "  D) " << optD << "\n";
        cout << "  Correct Answer: " << (char)('A' + correctAnswer - 1) << "\n";
        cout << "  Explanation: " << explanation << "\n";
        cout << string(120, '-') << "\n";

        qNum++;
    }

    if(!found)
    {
        cout << "No questions in this quiz.\n";
    }

    cout << string(120, '=') << "\n";
    sqlite3_finalize(stmt);
}

void Admin::editQuestion()
{
    int quizId, questionId;

    cout << "\n" << string(50, '=') << "\n";
    cout << "        EDIT QUESTION\n";
    cout << string(50, '=') << "\n";

    viewAllQuizzes();

    cout << "\nEnter Quiz ID: ";
    cin >> quizId;

    if (!verifyQuizOwnership(quizId)) {
        return;
    }

    viewQuestionsByQuizId();

    cout << "\nEnter Question ID to edit (0 to cancel): ";
    cin >> questionId;
    
    if (questionId == 0) {
        cout << "Edit cancelled.\n";
        return;
    }

    string question, opt1, opt2, opt3, opt4, explanation;
    int correctOption = 0;

    cout << "\n(Leave blank to keep current value)\n";
    cout << "Enter New Question Text: ";
    cin.ignore();
    getline(cin, question);

    cout << "Enter New Option A: ";
    getline(cin, opt1);

    cout << "Enter New Option B: ";
    getline(cin, opt2);

    cout << "Enter New Option C: ";
    getline(cin, opt3);

    cout << "Enter New Option D: ";
    getline(cin, opt4);

    cout << "Enter New Correct Option (1-4) (0 to keep current): ";
    cin >> correctOption;

    cout << "Enter New Explanation: ";
    cin.ignore();
    getline(cin, explanation);

    // Build dynamic update query
    string query = "UPDATE questions SET ";
    bool first = true;

    if (!question.empty()) {
        query += "question_text = '" + database->sanitizeInput(question) + "'";
        first = false;
    }
    if (!opt1.empty()) {
        if (!first) query += ", ";
        query += "option1 = '" + database->sanitizeInput(opt1) + "'";
        first = false;
    }
    if (!opt2.empty()) {
        if (!first) query += ", ";
        query += "option2 = '" + database->sanitizeInput(opt2) + "'";
        first = false;
    }
    if (!opt3.empty()) {
        if (!first) query += ", ";
        query += "option3 = '" + database->sanitizeInput(opt3) + "'";
        first = false;
    }
    if (!opt4.empty()) {
        if (!first) query += ", ";
        query += "option4 = '" + database->sanitizeInput(opt4) + "'";
        first = false;
    }
    if (correctOption >= 1 && correctOption <= 4) {
        if (!first) query += ", ";
        query += "correct_answer = " + to_string(correctOption);
        first = false;
    }
    if (!explanation.empty()) {
        if (!first) query += ", ";
        query += "explanation = '" + database->sanitizeInput(explanation) + "'";
        first = false;
    }

    if (first) {
        cout << "\nNo changes made.\n";
        return;
    }

    query += " WHERE question_id = " + to_string(questionId) + ";";

    if(database->executeQuery(query))
    {
        cout << "\n Question updated successfully\n";
    }
    else
    {
        cout << "\n Failed to update question\n";
    }
}

void Admin::deleteQuestion()
{
    int quizId, questionId;

    cout << "\n" << string(50, '=') << "\n";
    cout << "        DELETE QUESTION\n";
    cout << string(50, '=') << "\n";

    viewAllQuizzes();

    cout << "\nEnter Quiz ID: ";
    cin >> quizId;

    if (!verifyQuizOwnership(quizId)) {
        return;
    }

    viewQuestionsByQuizId();

    cout << "\nEnter Question ID to delete (0 to cancel): ";
    cin >> questionId;
    
    if (questionId == 0) {
        cout << "Deletion cancelled.\n";
        return;
    }

    cout << "Are you sure you want to delete this question? (yes/no): ";
    string confirm;
    cin >> confirm;

    if(confirm == "yes" || confirm == "YES")
    {
        string query = "DELETE FROM questions WHERE question_id = " + to_string(questionId) + ";";

        if(database->executeQuery(query))
        {
            cout << "\n Question deleted successfully\n";
        }
        else
        {
            cout << "\n Failed to delete question\n";
        }
    }
    else
    {
        cout << "Deletion cancelled.\n";
    }
}

void Admin::searchQuestionBank()
{
    string searchTerm;
    
    cout << "\n" << string(50, '=') << "\n";
    cout << "        SEARCH QUESTION BANK\n";
    cout << string(50, '=') << "\n";
    
    cout << "Enter search term: ";
    cin.ignore();
    getline(cin, searchTerm);
    
    if (searchTerm.length() < 3) {
        cout << "Please enter at least 3 characters.\n";
        return;
    }
    
    sqlite3_stmt* stmt;
    string query = "SELECT q.quiz_id, q.title, q.category, q.difficulty, "
                   "qs.question_id, qs.question_text "
                   "FROM quizzes q "
                   "JOIN questions qs ON q.quiz_id = qs.quiz_id "
                   "WHERE qs.question_text LIKE '%" + database->sanitizeInput(searchTerm) + "%' "
                   "ORDER BY q.quiz_id, qs.question_id LIMIT 50;";
    
    stmt = database->prepareStatement(query);
    
    if (!stmt) {
        cout << "Search failed.\n";
        return;
    }
    
    cout << "\n" << string(100, '=') << "\n";
    cout << "Search Results for: \"" << searchTerm << "\"\n";
    cout << string(100, '=') << "\n";
    
    bool found = false;
    int currentQuiz = -1;
    int resultCount = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = true;
        resultCount++;
        int quizId = sqlite3_column_int(stmt, 0);
        const char* quizTitle = (const char*)sqlite3_column_text(stmt, 1);
        const char* category = (const char*)sqlite3_column_text(stmt, 2);
        const char* difficulty = (const char*)sqlite3_column_text(stmt, 3);
        int questionId = sqlite3_column_int(stmt, 4);
        const char* questionText = (const char*)sqlite3_column_text(stmt, 5);
        
        if (quizId != currentQuiz) {
            cout << "\n Quiz " << quizId << ": " << quizTitle 
                 << " [" << category << " - " << difficulty << "]\n";
            currentQuiz = quizId;
        }
        
        cout << "   Q" << questionId << ": " << questionText << "\n";
    }
    
    if (!found) {
        cout << "No questions found matching \"" << searchTerm << "\"\n";
    } else {
        cout << "\nFound " << resultCount << " matching questions.\n";
    }
    
    sqlite3_finalize(stmt);
}

void Admin::viewAllUsers()
{
    cout << "\n" << string(100, '=') << "\n";
    cout << "                    ALL USERS\n";
    cout << string(100, '=') << "\n";

    sqlite3_stmt* stmt;

    string query = "SELECT u.id, u.username, u.role, "
                   "COUNT(DISTINCT a.attempt_id) as attempts, "
                   "COALESCE(AVG(a.score), 0) as avg_score, "
                   "COALESCE(MAX(a.score), 0) as best_score, "
                   "u.created_date, u.last_login "
                   "FROM users u "
                   "LEFT JOIN attempts a ON u.id = a.user_id "
                   "GROUP BY u.id ORDER BY u.role, u.id;";

    stmt = database->prepareStatement(query);

    if(!stmt)
    {
        cout << "Failed to retrieve users.\n";
        return;
    }

    cout << "\n" << left;
    cout << setw(6) << "ID" << " | " 
         << setw(20) << "Username" << " | " 
         << setw(8) << "Role" << " | " 
         << setw(8) << "Attempts" << " | " 
         << setw(9) << "Avg Score" << " | " 
         << setw(9) << "Best Score" << " | " 
         << "Joined\n";
    cout << string(100, '-') << "\n";

    bool found = false;

    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        found = true;
        int userId = sqlite3_column_int(stmt, 0);
        const char* username = (const char*)sqlite3_column_text(stmt, 1);
        const char* role = (const char*)sqlite3_column_text(stmt, 2);
        int attempts = sqlite3_column_int(stmt, 3);
        double avgScore = sqlite3_column_double(stmt, 4);
        int bestScore = sqlite3_column_int(stmt, 5);
        const char* joined = (const char*)sqlite3_column_text(stmt, 6);

        string shortUsername = string(username).substr(0, 17) + (strlen(username) > 17 ? "..." : "");
        
        cout << setw(6) << userId << " | " 
             << setw(20) << shortUsername << " | " 
             << setw(8) << role << " | " 
             << setw(8) << attempts << " | " 
             << setw(9) << fixed << setprecision(1) << avgScore << "% | " 
             << setw(9) << bestScore << "% | " 
             << joined << "\n";
    }

    cout << string(100, '-') << "\n";

    if(!found)
    {
        cout << "No users found.\n";
    }

    sqlite3_finalize(stmt);
}

void Admin::promoteUserToAdmin()
{
    cout << "\n" << string(50, '=') << "\n";
    cout << "        PROMOTE USER TO ADMIN\n";
    cout << string(50, '=') << "\n";
    
    viewAllUsers();
    
    int userId;
    cout << "\nEnter User ID to promote (0 to cancel): ";
    cin >> userId;
    
    if (userId == 0) {
        cout << "Promotion cancelled.\n";
        return;
    }
    
    if (userId == adminId) {
        cout << "You are already an admin.\n";
        return;
    }
    
    // Check if user exists and is not already admin
    sqlite3_stmt* stmt;
    string checkQuery = "SELECT username, role FROM users WHERE id = " + to_string(userId) + ";";
    stmt = database->prepareStatement(checkQuery);
    
    if (!stmt || sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "User not found.\n";
        if (stmt) sqlite3_finalize(stmt);
        return;
    }
    
    string username = (const char*)sqlite3_column_text(stmt, 0);
    string role = (const char*)sqlite3_column_text(stmt, 1);
    sqlite3_finalize(stmt);
    
    if (role == "admin") {
        cout << "User is already an admin.\n";
        return;
    }
    
    cout << "\nPromote user '" << username << "' to admin? (yes/no): ";
    string confirm;
    cin >> confirm;
    
    if (confirm == "yes" || confirm == "YES") {
        string updateQuery = "UPDATE users SET role = 'admin' WHERE id = " + to_string(userId) + ";";
        
        if (database->executeQuery(updateQuery)) {
            cout << "\n User promoted to admin successfully!\n";
            ErrorHandler::logError("Admin " + to_string(adminId) + " promoted user " + username + " to admin", ErrorHandler::INFO);
        } else {
            cout << "\n Failed to promote user.\n";
        }
    } else {
        cout << "Promotion cancelled.\n";
    }
}

void Admin::deleteUser()
{
    cout << "\n" << string(50, '=') << "\n";
    cout << "        DELETE USER\n";
    cout << string(50, '=') << "\n";

    viewAllUsers();

    int userId;
    cout << "\nEnter User ID to delete (0 to cancel): ";
    cin >> userId;
    
    if (userId == 0) {
        cout << "Deletion cancelled.\n";
        return;
    }

    // Don't allow deleting yourself
    if (userId == adminId) {
        cout << "\n You cannot delete your own admin account.\n";
        return;
    }

    // Get username for confirmation
    sqlite3_stmt* stmt;
    string nameQuery = "SELECT username, role FROM users WHERE id = " + to_string(userId) + ";";
    stmt = database->prepareStatement(nameQuery);
    
    if (!stmt || sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "User not found.\n";
        if (stmt) sqlite3_finalize(stmt);
        return;
    }
    
    string username = (const char*)sqlite3_column_text(stmt, 0);
    string role = (const char*)sqlite3_column_text(stmt, 1);
    sqlite3_finalize(stmt);

    cout << "\n  WARNING: You are about to delete user '" << username << "' (" << role << ")\n";
    cout << "All their data and attempts will be permanently deleted.\n";
    cout << "Type 'DELETE USER' to confirm: ";
    string confirm;
    cin.ignore();
    getline(cin, confirm);

    if(confirm == "DELETE USER")
    {
        database->executeQuery("BEGIN TRANSACTION;");
        
        // Delete user attempts first
        string deleteAttemptsQuery = "DELETE FROM attempts WHERE user_id = " + to_string(userId) + ";";
        bool success = database->executeQuery(deleteAttemptsQuery);

        // Delete user
        string deleteUserQuery = "DELETE FROM users WHERE id = " + to_string(userId) + ";";
        success = success && database->executeQuery(deleteUserQuery);

        if(success)
        {
            database->executeQuery("COMMIT;");
            cout << "\n User and all their data deleted successfully\n";
            ErrorHandler::logError("Admin " + to_string(adminId) + " deleted user: " + username, ErrorHandler::INFO);
        }
        else
        {
            database->executeQuery("ROLLBACK;");
            cout << "\n Failed to delete user\n";
        }
    }
    else
    {
        cout << "Deletion cancelled.\n";
    }
}

void Admin::viewQuizAnalytics()
{
    cout << "\n" << string(50, '=') << "\n";
    cout << "        QUIZ ANALYTICS DASHBOARD\n";
    cout << string(50, '=') << "\n";
    
    viewAllQuizzes();
    
    int quizId;
    cout << "\nEnter Quiz ID for detailed analytics (or 0 for overview): ";
    cin >> quizId;
    
    if (quizId == 0) {
        // Overview stats
        sqlite3_stmt* stmt;
        string query = "SELECT COUNT(DISTINCT quiz_id), COUNT(*), COALESCE(AVG(score), 0) "
                       "FROM attempts a "
                       "JOIN quizzes q ON a.quiz_id = q.quiz_id "
                       "WHERE q.admin_id = " + to_string(adminId) + ";";
        
        stmt = database->prepareStatement(query);
        if (stmt && sqlite3_step(stmt) == SQLITE_ROW) {
            int totalQuizzes = sqlite3_column_int(stmt, 0);
            int totalAttempts = sqlite3_column_int(stmt, 1);
            double avgScore = sqlite3_column_double(stmt, 2);
            
            cout << "\n" << string(50, '=') << "\n";
            cout << "OVERALL STATISTICS\n";
            cout << string(50, '=') << "\n";
            cout << " Total Quizzes Created: " << totalQuizzes << "\n";
            cout << " Total Attempts: " << totalAttempts << "\n";
            cout << " Average Score: " << fixed << setprecision(1) << avgScore << "%\n";
        }
        sqlite3_finalize(stmt);
        
        // Most popular quizzes
        string popularQuery = "SELECT q.quiz_id, q.title, COUNT(a.attempt_id) as attempts, AVG(a.score) as avg_score "
                              "FROM quizzes q "
                              "LEFT JOIN attempts a ON q.quiz_id = a.quiz_id "
                              "WHERE q.admin_id = " + to_string(adminId) + " "
                              "GROUP BY q.quiz_id "
                              "ORDER BY attempts DESC LIMIT 5;";
        
        stmt = database->prepareStatement(popularQuery);
        if (stmt) {
            cout << "\n MOST POPULAR QUIZZES\n";
            cout << string(50, '-') << "\n";
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const char* title = (const char*)sqlite3_column_text(stmt, 1);
                int attempts = sqlite3_column_int(stmt, 2);
                double avgScore = sqlite3_column_double(stmt, 3);
                cout << "Quiz " << id << ": " << title << " - " << attempts << " attempts, " 
                     << fixed << setprecision(1) << avgScore << "% avg\n";
            }
            sqlite3_finalize(stmt);
        }
        
    } else {
        if (!verifyQuizOwnership(quizId)) {
            return;
        }
        
        // Detailed quiz stats
        sqlite3_stmt* stmt;
        string query = "SELECT a.user_id, u.username, a.score, a.date, a.time_taken "
                       "FROM attempts a "
                       "JOIN users u ON a.user_id = u.id "
                       "WHERE a.quiz_id = " + to_string(quizId) + " "
                       "ORDER BY a.score DESC, a.date DESC;";
        
        stmt = database->prepareStatement(query);
        
        if (!stmt) {
            cout << "Failed to fetch analytics.\n";
            return;
        }
        
        cout << "\n" << string(100, '=') << "\n";
        cout << "DETAILED ATTEMPT HISTORY\n";
        cout << string(100, '=') << "\n";
        cout << left << setw(8) << "User ID" << " | " << setw(20) << "Username" 
             << " | " << setw(6) << "Score" << " | " << setw(10) << "Time Taken" << " | " << "Date\n";
        cout << string(100, '-') << "\n";
        
        int totalAttempts = 0;
        int totalScore = 0;
        int highestScore = 0;
        string highestScorer;
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int userId = sqlite3_column_int(stmt, 0);
            const char* username = (const char*)sqlite3_column_text(stmt, 1);
            int score = sqlite3_column_int(stmt, 2);
            const char* date = (const char*)sqlite3_column_text(stmt, 3);
            int timeTaken = sqlite3_column_int(stmt, 4);
            
            string timeStr = to_string(timeTaken / 60) + "m " + to_string(timeTaken % 60) + "s";
            
            cout << left << setw(8) << userId << " | " << setw(20) << username 
                 << " | " << setw(6) << score << "% | " << setw(10) << timeStr << " | " << date << "\n";
            
            totalAttempts++;
            totalScore += score;
            if (score > highestScore) {
                highestScore = score;
                highestScorer = username;
            }
        }
        
        sqlite3_finalize(stmt);
        
        cout << string(100, '=') << "\n";
        if (totalAttempts > 0) {
            cout << " Summary:\n";
            cout << "  Total Attempts: " << totalAttempts << "\n";
            cout << "  Average Score: " << fixed << setprecision(1) 
                 << (double)totalScore / totalAttempts << "%\n";
            cout << "  Highest Score: " << highestScore << "% by " << highestScorer << "\n";
            
            // Question-wise analysis
            string questionQuery = "SELECT q.question_id, q.question_text, "
                                   "COUNT(CASE WHEN a.user_answer = q.correct_answer THEN 1 END) * 100.0 / COUNT(*) as correct_percentage "
                                   "FROM questions q "
                                   "LEFT JOIN answers a ON q.question_id = a.question_id "
                                   "WHERE q.quiz_id = " + to_string(quizId) + " "
                                   "GROUP BY q.question_id;";
            
            sqlite3_finalize(stmt);
        } else {
            cout << "No attempts recorded for this quiz yet.\n";
        }
    }
}

void Admin::exportQuizResults()
{
    int quizId;
    
    cout << "\n" << string(50, '=') << "\n";
    cout << "        EXPORT QUIZ RESULTS\n";
    cout << string(50, '=') << "\n";
    
    viewAllQuizzes();
    
    cout << "\nEnter Quiz ID to export (0 to cancel): ";
    cin >> quizId;
    
    if (quizId == 0) {
        cout << "Export cancelled.\n";
        return;
    }
    
    if (!verifyQuizOwnership(quizId)) {
        return;
    }
    
    // Get quiz title
    sqlite3_stmt* stmt;
    string titleQuery = "SELECT title FROM quizzes WHERE quiz_id = " + to_string(quizId) + ";";
    stmt = database->prepareStatement(titleQuery);
    
    if (!stmt || sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "Quiz not found.\n";
        if (stmt) sqlite3_finalize(stmt);
        return;
    }
    
    string quizTitle = (const char*)sqlite3_column_text(stmt, 0);
    sqlite3_finalize(stmt);
    
    // Create filename with timestamp
    time_t now = time(0);
    tm* timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeinfo);
    
    // Sanitize quiz title for filename
    string safeTitle = quizTitle;
    for (char &c : safeTitle) {
        if (!isalnum(c)) c = '_';
    }
    
    string filename = "quiz_" + to_string(quizId) + "_" + safeTitle + "_" + timestamp + ".csv";
    
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to create export file.\n";
        return;
    }
    
    // Write CSV header
    file << "User ID,Username,Score (%),Time Taken (seconds),Date\n";
    
    // Get attempt data
    string query = "SELECT u.id, u.username, a.score, a.time_taken, a.date "
                   "FROM attempts a "
                   "JOIN users u ON a.user_id = u.id "
                   "WHERE a.quiz_id = " + to_string(quizId) + " "
                   "ORDER BY a.date DESC;";
    
    stmt = database->prepareStatement(query);
    
    if (!stmt) {
        cout << "Failed to fetch attempt data.\n";
        file.close();
        return;
    }
    
    int count = 0;
    int totalScore = 0;
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int userId = sqlite3_column_int(stmt, 0);
        const char* username = (const char*)sqlite3_column_text(stmt, 1);
        int score = sqlite3_column_int(stmt, 2);
        int timeTaken = sqlite3_column_int(stmt, 3);
        const char* date = (const char*)sqlite3_column_text(stmt, 4);
        
        file << userId << "," << username << "," << score << "," << timeTaken << "," << date << "\n";
        count++;
        totalScore += score;
    }
    
    sqlite3_finalize(stmt);
    
    // Write summary
    file << "\n";
    file << "SUMMARY\n";
    file << "Quiz ID," << quizId << "\n";
    file << "Quiz Title," << quizTitle << "\n";
    file << "Total Attempts," << count << "\n";
    if (count > 0) {
        file << "Average Score," << (double)totalScore / count << "%\n";
    }
    
    file.close();
    
    cout << "\n Results exported successfully!\n";
    cout << " Filename: " << filename << "\n";
    cout << " Exported " << count << " attempt records.\n";
    
    ErrorHandler::logError("Admin " + to_string(adminId) + " exported quiz " + to_string(quizId) + " results", ErrorHandler::INFO);
}

void Admin::viewSystemLogs()
{
    cout << "\n" << string(50, '=') << "\n";
    cout << "        SYSTEM LOGS\n";
    cout << string(50, '=') << "\n";
    
    cout << "1. View Error Log\n";
    cout << "2. Clear Error Log\n";
    cout << "3. View Database Info\n";
    cout << "Enter choice: ";
    
    int choice;
    cin >> choice;
    
    switch(choice) {
        case 1:
            ErrorHandler::viewErrorLog();
            break;
        case 2:
            ErrorHandler::clearErrorLog();
            break;
        case 3: {
            sqlite3_stmt* stmt;
            string query = "SELECT "
                           "(SELECT COUNT(*) FROM users) as users, "
                           "(SELECT COUNT(*) FROM quizzes) as quizzes, "
                           "(SELECT COUNT(*) FROM questions) as questions, "
                           "(SELECT COUNT(*) FROM attempts) as attempts;";
            
            stmt = database->prepareStatement(query);
            if (stmt && sqlite3_step(stmt) == SQLITE_ROW) {
                int users = sqlite3_column_int(stmt, 0);
                int quizzes = sqlite3_column_int(stmt, 1);
                int questions = sqlite3_column_int(stmt, 2);
                int attempts = sqlite3_column_int(stmt, 3);
                
                cout << "\n DATABASE STATISTICS\n";
                cout << string(30, '-') << "\n";
                cout << "Users: " << users << "\n";
                cout << "Quizzes: " << quizzes << "\n";
                cout << "Questions: " << questions << "\n";
                cout << "Attempts: " << attempts << "\n";
            }
            sqlite3_finalize(stmt);
            break;
        }
        default:
            cout << "Invalid choice.\n";
    }
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

void Admin::backupDatabase()
{
    cout << "\n" << string(50, '=') << "\n";
    cout << "        BACKUP DATABASE\n";
    cout << string(50, '=') << "\n";
    
    time_t now = time(0);
    tm* timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeinfo);
    
    string backupFile = "backup_quizz_" + string(timestamp) + ".db";
    
    sqlite3* backupDb;
    int rc = sqlite3_open(backupFile.c_str(), &backupDb);
    
    if (rc != SQLITE_OK) {
        cout << "Failed to create backup file.\n";
        return;
    }
    
    sqlite3_backup* backup = sqlite3_backup_init(backupDb, "main", database->getDB(), "main");
    if (backup) {
        sqlite3_backup_step(backup, -1);
        sqlite3_backup_finish(backup);
    }
    
    sqlite3_close(backupDb);
    
    cout << " Database backed up successfully!\n";
    cout << " Backup file: " << backupFile << "\n";
    
    ErrorHandler::logError("Admin " + to_string(adminId) + " created database backup", ErrorHandler::INFO);
}