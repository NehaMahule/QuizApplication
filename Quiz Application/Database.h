#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <algorithm>
#include "sqlite3.h"
#include "Interfaces.h"

class Database : public IDatabase
{
private:
    sqlite3* db;

public:
    Database();
    ~Database();

    bool openDatabase();
    void closeDatabase();

    bool executeQuery(const std::string& query);
    void createTables();
    
    sqlite3* getDB();
    
    // Helper methods
    sqlite3_stmt* prepareStatement(const std::string& query);
    int getLastInsertRowId();
    std::string sanitizeInput(const std::string& input);
};

#endif