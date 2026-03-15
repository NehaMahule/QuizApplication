#ifndef ANALYTICS_H
#define ANALYTICS_H

#include <iostream>
#include <string>
#include <iomanip>
#include "Interfaces.h"

class Analytics : public IAnalytics
{
private:
    IDatabase* database;

public:
    Analytics(IDatabase* db);

    void showLeaderboard();
    void userAnalytics(int userId);
    void bestCategory(int userId);
    void weakCategory(int userId);
    void showGlobalStats();
    
private:
    void displayProgressBar(double percentage);
};

#endif