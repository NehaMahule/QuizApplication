#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <string>
#include "Interfaces.h"

class Authentication : public IAuthentication
{
private:
    IDatabase* database;

public:
    Authentication(IDatabase* db);

    bool registerUser();
    bool registerAdmin();

    bool loginUser();
    bool loginAdmin();

    bool usernameExists(const std::string& username);
    std::string hashPassword(const std::string& password);
    int getUserId(const std::string& username);
    
    void updateLastLogin(int userId);
};

#endif