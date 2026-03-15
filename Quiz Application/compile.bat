@echo off
echo Compiling Quiz Application...
echo.

echo Step 1: Compiling SQLite3...
gcc -c sqlite3.c -o sqlite3.o
if %errorlevel% neq 0 (
    echo Failed to compile SQLite3
    pause
    exit /b %errorlevel%
)

echo Step 2: Compiling main application...
g++ -std=c++11 main.cpp Database.cpp ErrorHandler.cpp Authentication.cpp Admin.cpp Analytics.cpp User.cpp QuizEngine.cpp sqlite3.o -o quizapp.exe
if %errorlevel% neq 0 (
    echo Failed to compile main application
    pause
    exit /b %errorlevel%
)

echo.
echo Compilation successful! Run quizapp.exe to start.
pause