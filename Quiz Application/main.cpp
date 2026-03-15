#include <iostream>
#include <memory>
#include "ServiceFactory.h"
#include "ErrorHandler.h"

using namespace std;

void displayWelcomeMessage() {
    cout << "\n";
    cout << " |--------------------------------------------------|\n";
    cout << " |         ADVANCED QUIZ APPLICATION v3.0           |\n";
    cout << " |           Test Your Knowledge!                   |\n";
    cout << " |--------------------------------------------------|\n";
    cout << "\n";
}

void displayMainMenu() {
    cout << "\n" << string(50, '=') << "\n";
    cout << "                 MAIN MENU\n";
    cout << string(50, '=') << "\n";
    cout << "1. Register\n";
    cout << "2. Login\n";
    cout << "3. View Global Statistics\n";
    cout << "4. Exit\n";
    cout << string(50, '-') << "\n";
    cout << "Enter choice: ";
}

int main()
{
    displayWelcomeMessage();
    
    // Initialize services
    auto db = ServiceFactory::createDatabase();

    if (!db->openDatabase())
    {
        ErrorHandler::logError("Failed to open database", ErrorHandler::CRITICAL);
        cout << "Failed to open database. Exiting...\n";
        return 1;
    }

    db->createTables();

    auto auth = ServiceFactory::createAuthentication(db.get());
    auto admin = ServiceFactory::createAdmin(db.get());
    auto analytics = ServiceFactory::createAnalytics(db.get());

    int choice;

    do
    {
        displayMainMenu();
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch(choice)
        {
            case 1:  // Register
            {
                int regChoice;
                cout << "\n1. Register as User\n";
                cout << "2. Register as Admin\n";
                cout << "Enter choice: ";
                cin >> regChoice;

                if(regChoice == 1)
                    auth->registerUser();
                else if(regChoice == 2)
                    auth->registerAdmin();
                else
                    cout << "Invalid choice.\n";

                break;
            }

            case 2:  // Login
            {
                int loginChoice;
                cout << "\n1. Login as User\n";
                cout << "2. Login as Admin\n";
                cout << "Enter choice: ";
                cin >> loginChoice;

                if(loginChoice == 1)  // User Login
                {
                    if(auth->loginUser())
                    {
                        int userId;
                        cout << "\nEnter your User ID: ";
                        cin >> userId;

                        auto user = ServiceFactory::createUser(db.get(), userId);
                        auto engine = ServiceFactory::createQuizEngine(db.get());

                        user->userMenu();
                    }
                }
                else if(loginChoice == 2)  // Admin Login
                {
                    if(auth->loginAdmin())
                    {
                        int adminId;
                        cout << "\nEnter your Admin ID: ";
                        cin >> adminId;
                        
                        admin->setAdminId(adminId);
                        admin->adminMenu();
                    }
                }
                else
                {
                    cout << "Invalid choice.\n";
                }
                break;
            }
            
            case 3:  // Global Statistics
            {
                analytics->showGlobalStats();
                cout << "\nPress Enter to continue...";
                cin.ignore();
                cin.get();
                break;
            }

            case 4:  // Exit
                cout << "\nThank you for using the Quiz Application!\n";
                cout << "Goodbye!\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while(choice != 4);

    db->closeDatabase();
    
    return 0;
}