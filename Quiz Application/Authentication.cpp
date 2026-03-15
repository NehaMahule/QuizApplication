#include "Authentication.h"
#include <iostream>
#include "sqlite3.h"
#include <functional>

using namespace std;

Authentication::Authentication(IDatabase* db)
{
    database = db;
}

string Authentication::hashPassword(const string& password)
{
    hash<string> hasher;
    return to_string(hasher(password));
}

bool Authentication::usernameExists(const string& username)
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query = "SELECT username FROM users WHERE username=?;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return rc == SQLITE_ROW;
}

int Authentication::getUserId(const string& username)
{
    sqlite3* db = database->getDB();
    sqlite3_stmt* stmt;

    string query = "SELECT id FROM users WHERE username=?;";

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int userId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return userId;
}

void Authentication::updateLastLogin(int userId)
{
    string query = "UPDATE users SET last_login = CURRENT_TIMESTAMP WHERE id = " + to_string(userId) + ";";
    database->executeQuery(query);
}

bool Authentication::registerUser()
{
    string username, password;

    cout << "Enter username: ";
    cin >> username;

    if(usernameExists(username))
    {
        cout << "Username already exists!\n";
        return false;
    }

    cout << "Enter password: ";
    cin >> password;

    password = hashPassword(password);

    string query = "INSERT INTO users(username, password, role) VALUES('" + 
                   database->sanitizeInput(username) + "', '" + 
                   database->sanitizeInput(password) + "', 'user');";

    if(database->executeQuery(query))
    {
        int userId = database->getLastInsertRowId();
        cout << "\n User registered successfully!\n";
        cout << "Your User ID is: " << userId << "\n";
        cout << "Please save this ID for login.\n";
        return true;
    }

    cout << "\n Registration failed.\n";
    return false;
}

bool Authentication::registerAdmin()
{
    string username, password;

    cout << "Enter admin username: ";
    cin >> username;

    if(usernameExists(username))
    {
        cout << "Username already exists!\n";
        return false;
    }

    cout << "Enter password: ";
    cin >> password;

    password = hashPassword(password);

    string query = "INSERT INTO users(username, password, role) VALUES('" + 
                   database->sanitizeInput(username) + "', '" + 
                   database->sanitizeInput(password) + "', 'admin');";

    if(database->executeQuery(query))
    {
        int adminId = database->getLastInsertRowId();
        cout << "\n Admin registered successfully!\n";
        cout << "Your Admin ID is: " << adminId << "\n";
        return true;
    }

    cout << "\n Registration failed.\n";
    return false;
}

bool Authentication::loginUser()
{
    string username, password;

    cout << "Username: ";
    cin >> username;

    cout << "Password: ";
    cin >> password;

    password = hashPassword(password);

    sqlite3_stmt* stmt;

    string query = "SELECT id FROM users WHERE username='" + 
                   database->sanitizeInput(username) + 
                   "' AND password='" + database->sanitizeInput(password) + 
                   "' AND role='user';";

    stmt = database->prepareStatement(query);

    if(!stmt)
    {
        cout << "Login failed.\n";
        return false;
    }

    int rc = sqlite3_step(stmt);
    int userId = -1;
    
    if(rc == SQLITE_ROW)
    {
        userId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if(userId != -1)
    {
        updateLastLogin(userId);
        cout << "\n Login successful! Welcome, " << username << "!\n";
        cout << "Your User ID: " << userId << "\n";
        return true;
    }

    cout << "\n Invalid credentials\n";
    return false;
}

bool Authentication::loginAdmin()
{
    string username, password;

    cout << "Admin Username: ";
    cin >> username;

    cout << "Password: ";
    cin >> password;

    password = hashPassword(password);

    sqlite3_stmt* stmt;

    string query = "SELECT id FROM users WHERE username='" + 
                   database->sanitizeInput(username) + 
                   "' AND password='" + database->sanitizeInput(password) + 
                   "' AND role='admin';";

    stmt = database->prepareStatement(query);

    if(!stmt)
    {
        cout << "Login failed.\n";
        return false;
    }

    int rc = sqlite3_step(stmt);
    int adminId = -1;
    
    if(rc == SQLITE_ROW)
    {
        adminId = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    if(adminId != -1)
    {
        updateLastLogin(adminId);
        cout << "\n Admin login successful! Welcome, " << username << "!\n";
        cout << "Your Admin ID: " << adminId << "\n";
        return true;
    }

    cout << "\n Invalid credentials\n";
    return false;
}