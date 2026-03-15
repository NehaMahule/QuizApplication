#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include <memory>
#include "Interfaces.h"
#include "Database.h"
#include "Authentication.h"
#include "Admin.h"
#include "Analytics.h"
#include "User.h"
#include "QuizEngine.h"

class ServiceFactory
{
public:
    static std::unique_ptr<IDatabase> createDatabase()
    {
        return std::unique_ptr<IDatabase>(new Database());
    }

    static std::unique_ptr<IAuthentication> createAuthentication(IDatabase* db)
    {
        return std::unique_ptr<IAuthentication>(new Authentication(db));
    }

    static std::unique_ptr<IAdmin> createAdmin(IDatabase* db)
    {
        return std::unique_ptr<IAdmin>(new Admin(db));
    }

    static std::unique_ptr<IAnalytics> createAnalytics(IDatabase* db)
    {
        return std::unique_ptr<IAnalytics>(new Analytics(db));
    }

    static std::unique_ptr<IUser> createUser(IDatabase* db, int userId)
    {
        return std::unique_ptr<IUser>(new User(db, userId));
    }

    static std::unique_ptr<IQuizEngine> createQuizEngine(IDatabase* db)
    {
        return std::unique_ptr<IQuizEngine>(new QuizEngine(db));
    }
};

#endif