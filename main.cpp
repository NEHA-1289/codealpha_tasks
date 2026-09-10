#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <cctype>

using namespace std;

const string FILE_NAME = "users.txt";

// Hash the password before storing it
string hashPassword(const string& password)
{
    hash<string> hasher;
    return to_string(hasher(password));
}

// Validate username
bool validUsername(const string& username)
{
    if (username.length() < 3)
        return false;

    for (char ch : username)
    {
        if (!isalnum(static_cast<unsigned char>(ch)) && ch != '_')
            return false;
    }

    return true;
}

// Validate password
bool validPassword(const string& password)
{
    return password.length() >= 6;
}

// Check whether a username already exists
bool usernameExists(const string& username)
{
    ifstream file(FILE_NAME);

    string storedUsername;
    string storedPassword;

    while (file >> storedUsername >> storedPassword)
    {
        if (storedUsername == username)
            return true;
    }

    return false;
}

// Register a new user
void registration()
{
    string username;
    string password;
    string confirmPassword;

    cout << "\n========== REGISTRATION ==========\n";

    cout << "Enter username: ";
    cin >> username;

    if (!validUsername(username))
    {
        cout << "\nError: Username must contain at least 3 characters.\n";
        cout << "Only letters, numbers and underscore (_) are allowed.\n";
        return;
    }

    if (usernameExists(username))
    {
        cout << "\nError: Username already exists!\n";
        cout << "Please choose another username.\n";
        return;
    }

    cout << "Enter password: ";
    cin >> password;

    if (!validPassword(password))
    {
        cout << "\nError: Password must contain at least 6 characters.\n";
        return;
    }

    cout << "Confirm password: ";
    cin >> confirmPassword;

    if (password != confirmPassword)
    {
        cout << "\nError: Passwords do not match.\n";
        return;
    }

    ofstream file(FILE_NAME, ios::app);

    if (!file)
    {
        cout << "\nError: Unable to open user file.\n";
        return;
    }

    file << username << " " << hashPassword(password) << '\n';
    file.close();

    cout << "\nRegistration successful!\n";
    cout << "You can now login using your credentials.\n";
}

// Login an existing user
void login()
{
    string username;
    string password;

    cout << "\n============== LOGIN ==============\n";

    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    ifstream file(FILE_NAME);

    if (!file)
    {
        cout << "\nError: No registered users found.\n";
        cout << "Please register first.\n";
        return;
    }

    string storedUsername;
    string storedPassword;
    string enteredHash = hashPassword(password);

    bool loginSuccessful = false;

    while (file >> storedUsername >> storedPassword)
    {
        if (storedUsername == username && storedPassword == enteredHash)
        {
            loginSuccessful = true;
            break;
        }
    }

    file.close();

    if (loginSuccessful)
    {
        cout << "\n==================================\n";
        cout << "       LOGIN SUCCESSFUL!\n";
        cout << "==================================\n";
        cout << "Welcome, " << username << "!\n";
    }
    else
    {
        cout << "\nLogin failed!\n";
        cout << "Invalid username or password.\n";
    }
}

int main()
{
    int choice;

    cout << "============================================\n";
    cout << "       LOGIN AND REGISTRATION SYSTEM\n";
    cout << "============================================\n";

    do
    {
        cout << "\n--------------- MENU ----------------\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "-------------------------------------\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
                registration();
                break;

            case 2:
                login();
                break;

            case 3:
                cout << "\nThank you for using the system!\n";
                cout << "Goodbye!\n";
                break;

            default:
                cout << "\nInvalid choice!\n";
                cout << "Please enter 1, 2 or 3.\n";
        }

    } while (choice != 3);

    return 0;
}
