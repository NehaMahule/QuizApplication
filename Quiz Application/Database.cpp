#include "Database.h"
#include "ErrorHandler.h"

using namespace std;

Database::Database()
{
    db = nullptr;
}

Database::~Database()
{
    closeDatabase();
}

bool Database::openDatabase()
{
    int rc = sqlite3_open("quizz.db", &db);

    if (rc)
    {
        ErrorHandler::logDatabaseError("openDatabase", sqlite3_errmsg(db));
        cout << "Error: Cannot open database\n";
        return false;
    }

    cout << "Database opened successfully\n";
    return true;
}

void Database::closeDatabase()
{
    if (db)
    {
        sqlite3_close(db);
        cout << "Database closed\n";
    }
}

static bool columnExists(sqlite3* db, const string& table, const string& column)
{
    string sql = "PRAGMA table_info('" + table + "');";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        if (stmt) sqlite3_finalize(stmt);
        return false;
    }

    bool found = false;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char* colName = sqlite3_column_text(stmt, 1);
        if (colName && column == reinterpret_cast<const char*>(colName))
        {
            found = true;
            break;
        }
    }

    sqlite3_finalize(stmt);
    return found;
}

static void ensureColumn(sqlite3* db, const string& table, const string& column, const string& definition)
{
    if (!columnExists(db, table, column))
    {
        string sql = "ALTER TABLE " + table + " ADD COLUMN " + column + " " + definition + ";";
        sqlite3_exec(db, sql.c_str(), 0, 0, nullptr);
    }
}

bool Database::executeQuery(const string& query)
{
    char* errMsg = 0;

    int rc = sqlite3_exec(db, query.c_str(), 0, 0, &errMsg);

    if (rc != SQLITE_OK)
    {
        ErrorHandler::logDatabaseError("executeQuery", errMsg);
        cout << "SQL Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

void Database::createTables()
{
    string usersTable =
    "CREATE TABLE IF NOT EXISTS users ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "username TEXT UNIQUE,"
    "password TEXT,"
    "role TEXT DEFAULT 'user',"
    "created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "last_login TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "total_attempts INTEGER DEFAULT 0,"
    "total_score INTEGER DEFAULT 0,"
    "achievements TEXT DEFAULT '');";

    executeQuery(usersTable);

    string quizzesTable =
    "CREATE TABLE IF NOT EXISTS quizzes ("
    "quiz_id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "admin_id INTEGER,"
    "title TEXT,"
    "description TEXT,"
    "category TEXT,"
    "difficulty TEXT,"
    "time_limit INTEGER,"
    "created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "times_taken INTEGER DEFAULT 0,"
    "avg_score REAL DEFAULT 0,"
    "FOREIGN KEY(admin_id) REFERENCES users(id));";

    executeQuery(quizzesTable);

    ensureColumn(db, "quizzes", "admin_id", "INTEGER");
    ensureColumn(db, "quizzes", "title", "TEXT");
    ensureColumn(db, "quizzes", "description", "TEXT");
    ensureColumn(db, "quizzes", "category", "TEXT");
    ensureColumn(db, "quizzes", "difficulty", "TEXT");
    ensureColumn(db, "quizzes", "time_limit", "INTEGER");

    string questionsTable =
    "CREATE TABLE IF NOT EXISTS questions ("
    "question_id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "quiz_id INTEGER,"
    "question_text TEXT,"
    "option1 TEXT,"
    "option2 TEXT,"
    "option3 TEXT,"
    "option4 TEXT,"
    "correct_answer INTEGER,"
    "explanation TEXT,"
    "FOREIGN KEY(quiz_id) REFERENCES quizzes(quiz_id) ON DELETE CASCADE);";

    executeQuery(questionsTable);

    string attemptsTable =
    "CREATE TABLE IF NOT EXISTS attempts ("
    "attempt_id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "user_id INTEGER,"
    "quiz_id INTEGER,"
    "score INTEGER,"
    "percentage REAL,"
    "time_taken INTEGER,"
    "date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,"
    "FOREIGN KEY(quiz_id) REFERENCES quizzes(quiz_id) ON DELETE CASCADE);";

    executeQuery(attemptsTable);

    string feedbackTable =
    "CREATE TABLE IF NOT EXISTS feedback ("
    "feedback_id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "user_id INTEGER,"
    "quiz_id INTEGER,"
    "rating INTEGER,"
    "comment TEXT,"
    "date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
    "FOREIGN KEY(user_id) REFERENCES users(id),"
    "FOREIGN KEY(quiz_id) REFERENCES quizzes(quiz_id));";

    executeQuery(feedbackTable);

    // Check if default admin exists
    string checkAdmin = "SELECT COUNT(*) FROM users WHERE role = 'admin';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, checkAdmin.c_str(), -1, &stmt, NULL);
    
    if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) == 0) {
        // Create default admin
        string createAdmin = "INSERT INTO users (username, password, role) VALUES "
                             "('admin', 'admin123', 'admin');";
        executeQuery(createAdmin);
        cout << "\n Default admin created!\n";
        cout << "   Username: admin\n";
        cout << "   Password: admin123\n";
        cout << "   Please change these after first login.\n\n";
    }
    sqlite3_finalize(stmt);

    cout << "All tables created successfully\n";
}

sqlite3* Database::getDB()
{
    return db;
}

sqlite3_stmt* Database::prepareStatement(const std::string& query)
{
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if(rc != SQLITE_OK)
    {
        ErrorHandler::logDatabaseError("prepareStatement", sqlite3_errmsg(db));
        cout << "SQL Prepare Error: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }
    return stmt;
}

int Database::getLastInsertRowId()
{
    return (int)sqlite3_last_insert_rowid(db);
}

string Database::sanitizeInput(const string& input)
{
    string sanitized = input;
    size_t pos = 0;
    while ((pos = sanitized.find("'", pos)) != string::npos) {
        sanitized.replace(pos, 1, "''");
        pos += 2;
    }
    return sanitized;
}