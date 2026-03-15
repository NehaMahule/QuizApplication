#include <iostream>
#include "Database.h"
#include "ErrorHandler.h"

using namespace std;

int main() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "           CLEAR ALL USERS UTILITY\n";
    cout << string(60, '=') << "\n";
    cout << "  WARNING: This will delete ALL:\n";
    cout << "   • Users\n";
    cout << "   • Quizzes\n";
    cout << "   • Questions\n";
    cout << "   • Attempts\n";
    cout << "   • Feedback\n";
    cout << "\nThis action CANNOT be undone!\n\n";
    
    cout << "Type 'DELETE ALL' to confirm: ";
    string confirmation;
    cin >> confirmation;
    
    if (confirmation != "DELETE ALL") {
        cout << "Operation cancelled.\n";
        return 0;
    }
    
    Database db;
    if (!db.openDatabase()) {
        cout << "Failed to open database.\n";
        return 1;
    }
    
    // Create new admin
    cout << "\nCreate new admin account:\n";
    cout << "Enter username for new admin: ";
    string adminName;
    cin.ignore();
    getline(cin, adminName);
    
    if (adminName.empty()) {
        adminName = "admin";
    }
    
    cout << "Enter password for admin: ";
    string adminPass;
    getline(cin, adminPass);
    
    if (adminPass.empty()) {
        adminPass = "admin123";
    }
    
    // Start transaction
    cout << "\nClearing database...\n";
    db.executeQuery("BEGIN TRANSACTION;");
    
    // Clear all data
    db.executeQuery("DELETE FROM attempts;");
    db.executeQuery("DELETE FROM feedback;");
    db.executeQuery("DELETE FROM questions;");
    db.executeQuery("DELETE FROM quizzes;");
    db.executeQuery("DELETE FROM users;");
    
    // Create new admin
    string createAdmin = "INSERT INTO users (username, password, role) VALUES ('" + 
                         db.sanitizeInput(adminName) + "', '" + 
                         db.sanitizeInput(adminPass) + "', 'admin');";
    
    if (db.executeQuery(createAdmin)) {
        db.executeQuery("COMMIT;");
        cout << "\n Database cleared successfully!\n";
        cout << "\nNew admin created:\n";
        cout << "  Username: " << adminName << "\n";
        cout << "  Password: " << adminPass << "\n";
        cout << "\nPlease save these credentials!\n";
    } else {
        db.executeQuery("ROLLBACK;");
        cout << "\n Failed to clear database.\n";
    }
    
    db.closeDatabase();
    
    return 0;
}