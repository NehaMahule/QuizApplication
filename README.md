 Advanced Quiz Application

A comprehensive C++ quiz management system with user authentication, admin dashboard, performance analytics, and achievement system. Built with SQLite3 for persistent storage and following modern C++ design patterns.

![C++](https://img.shields.io/badge/C++-11-blue.svg)
![SQLite](https://img.shields.io/badge/SQLite-3-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20|%20Linux%20|%20Mac-lightgrey)
![License](https://img.shields.io/badge/License-MIT-yellow)

📋 Table of Contents
- [Features](#-features)
- [Screenshots](#-screenshots)
- [Architecture](#-architecture)
- [Tech Stack](#-tech-stack)
- [Installation](#-installation)
- [Usage Guide](#-usage-guide)
- [Project Structure](#-project-structure)
- [Database Schema](#-database-schema)
- [API Documentation](#-api-documentation)
- [Contributing](#-contributing)
- [License](#-license)

✨ Features

👤 User Features
- User Registration/Login - Secure authentication with password hashing
- Browse Quizzes - View all available quizzes
- Filter Quizzes - Search by category (C, C++, Python, Java, Basic Maths) or difficulty (Low, Medium, High)
- **Take Quizzes** - Timer-based quiz attempts with per-question time limits
- **Instant Feedback** - Immediate results with explanations
- **Quiz History** - Track all past attempts with scores and timestamps
- **Performance Analytics** - View statistics, best/worst categories
- **Achievement System** - Earn badges for milestones
- **Leaderboard** - Compete with other users

👑 Admin Features
- **Quiz Management** - Create, edit, delete, and clone quizzes
- **Question Bank** - Add, modify, and remove questions
- **Bulk Operations** - Add multiple questions at once
- **User Management** - View all users, promote to admin, delete users
- **Analytics Dashboard** - View quiz popularity and performance stats
- **Export Results** - Export quiz attempts to CSV
- **System Logs** - View error logs
- **Database Backup** - Create database backups

 📊 Analytics & Reporting
- **Global Leaderboard** - Top 10 users with medals (🥇🥈🥉)
- **User Performance** - Average scores, best/worst categories
- **Quiz Statistics** - Attempt counts, average scores
- **Category Analysis** - Performance breakdown by topic
- **Progress Tracking** - Visual progress bars

 🏗 Architecture

┌─────────────────────────────────────────────────────────────┐
│                      QUIZ APPLICATION                        │
├───────────────┬───────────────────────┬─────────────────────┤
│  Presentation │     Business Logic     │      Data Layer     │
│    Layer      │                       │                     │
├───────────────┼───────────────────────┼─────────────────────┤
│ • Console UI  │ • Authentication      │ • SQLite Database   │
│ • Menus       │ • Quiz Engine         │ • File I/O          │
│ • User Input  │ • Admin Panel         │ • CSV Export        │
│ • Formatting  │ • Analytics           │ • Error Logging     │
└───────────────┴───────────────────────┴─────────────────────┘
💻 Tech Stack

- **Language**: C++11
- **Database**: SQLite3 (amalgamation)
- **Build System**: GCC/MinGW
- **Design Patterns**: 
  - Factory Pattern (ServiceFactory)
  - Dependency Injection
  - Interface-based Design
  - Singleton (ErrorHandler)
  - RAII (Resource Acquisition Is Initialization)

 📦 Installation

 Prerequisites
- MinGW/GCC compiler with C++11 support
- SQLite3 (included as amalgamation)
- Windows/Linux/MacOS

 **Clone Repository**

git clone https://github.com/yourusername/quiz-application.git
cd quiz-application


**Compile on Windows (Command Prompt)**

gcc -c sqlite3.c -o sqlite3.o
g++ -std=c++11 main.cpp Database.cpp ErrorHandler.cpp Authentication.cpp Admin.cpp Analytics.cpp User.cpp QuizEngine.cpp sqlite3.o -o quizapp.exe
quizapp.exe


Compile on Windows (PowerShell)

gcc -c sqlite3.c -o sqlite3.o; g++ -std=c++11 main.cpp Database.cpp ErrorHandler.cpp Authentication.cpp Admin.cpp Analytics.cpp User.cpp QuizEngine.cpp sqlite3.o -o quizapp.exe; .\quizapp.exe

# Compile on Linux/Mac

gcc -c sqlite3.c -o sqlite3.o
g++ -std=c++11 main.cpp Database.cpp ErrorHandler.cpp Authentication.cpp Admin.cpp Analytics.cpp User.cpp QuizEngine.cpp sqlite3.o -o quizapp
./quizapp


# Usage Guide

# First Run
On first launch, the application automatically creates:
- Default admin: `admin` / `admin123`
- Required database tables

# Main Menu

==================================================
                 MAIN MENU
==================================================
1. Register
2. Login
3. View Global Statistics
4. Exit
--------------------------------------------------
Enter choice:


# User Registration
1. Select option 1 from main menu
2. Choose "Register as User"
3. Enter username (e.g., rahul, priya, amit, sanya, vikram)
4. Enter password
5. Save your User ID for future logins

# Taking a Quiz
1. Login as user
2. Browse available quizzes
3. Enter Quiz ID to attempt
4. Answer questions within time limit
5. View results and achievements

# Admin Dashboard
1. Login as admin (`admin` / `admin123`)
2. Access admin menu with 19+ options
3. Create quizzes and add questions
4. Manage users and view analytics

# 📁 Project Structure

quiz-application/
│
├── 📄 main.cpp                 # Entry point
├── 📄 Interfaces.h              # All interfaces
├── 📄 ServiceFactory.h          # Factory pattern
│
├── 📄 Database.h/cpp            # SQLite database operations
├── 📄 Authentication.h/cpp       # User authentication
├── 📄 Admin.h/cpp                # Admin functionalities
├── 📄 User.h/cpp                 # User functionalities
├── 📄 Analytics.h/cpp            # Analytics and reporting
├── 📄 QuizEngine.h/cpp           # Quiz taking logic
├── 📄 ErrorHandler.h/cpp         # Error logging
│
├── 📄 sqlite3.h/c                # SQLite amalgamation
│
├── 📄 quizz.db                   # SQLite database (created at runtime)
├── 📄 error_log.txt               # Error logs (created at runtime)
│
└── 📄 README.md                   # This file

# Database Schema

# Users Table

CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE,
    password TEXT,
    role TEXT DEFAULT 'user',
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    total_attempts INTEGER DEFAULT 0,
    total_score INTEGER DEFAULT 0,
    achievements TEXT DEFAULT ''
);


# Quizzes Table

CREATE TABLE quizzes (
    quiz_id INTEGER PRIMARY KEY AUTOINCREMENT,
    admin_id INTEGER,
    title TEXT,
    description TEXT,
    category TEXT,
    difficulty TEXT,
    time_limit INTEGER,
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    times_taken INTEGER DEFAULT 0,
    avg_score REAL DEFAULT 0,
    FOREIGN KEY(admin_id) REFERENCES users(id)
);

# Questions Table

CREATE TABLE questions (
    question_id INTEGER PRIMARY KEY AUTOINCREMENT,
    quiz_id INTEGER,
    question_text TEXT,
    option1 TEXT,
    option2 TEXT,
    option3 TEXT,
    option4 TEXT,
    correct_answer INTEGER,
    explanation TEXT,
    FOREIGN KEY(quiz_id) REFERENCES quizzes(quiz_id) ON DELETE CASCADE
);

# Attempts Table
CREATE TABLE attempts (
    attempt_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    quiz_id INTEGER,
    score INTEGER,
    percentage REAL,
    time_taken INTEGER,
    date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY(quiz_id) REFERENCES quizzes(quiz_id) ON DELETE CASCADE
);

# 📚 API Documentation


# Unique Features

#Achievement System
-  **Perfect Score** - Score 100% on a quiz
-  **Quiz Master** - Take 10+ quizzes
-  **Knowledge Seeker** - Take 3+ quizzes
-  **First Quiz** - Complete first quiz

# Timer System
- Overall quiz timer
- Per-question 30-second limit
- Real-time countdown display

# Export Functionality
- CSV export of quiz results
- Timestamped filenames
- Detailed attempt data

# Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

# License

This project is licensed under the MIT License - see the LICENSE file for details.

# Author

**Your Name**
- GitHub: [@nehamahule][(https://github.com/NehaMahule/QuizApplication])

