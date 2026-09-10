# Login and Registration System - C++

## Task
CodeAlpha Internship - Task 2: Login and Registration System

## Features
- User registration with username and password
- Username and password validation
- Duplicate username checking
- Password confirmation
- Credentials stored in `users.txt`
- Password is hashed before storage
- User login and identity verification
- Success and error messages
- Menu-driven console interface

## Files
- `main.cpp` - complete C++ source code
- `users.txt` - created automatically when the first user registers

## How to Run

### Using g++
```bash
g++ main.cpp -o LoginRegistrationSystem
./LoginRegistrationSystem
```

### Using Windows
```bash
g++ main.cpp -o LoginRegistrationSystem.exe
LoginRegistrationSystem.exe
```

The `users.txt` file will be created automatically in the same folder when a user registers.

## Note
`std::hash` is used here to demonstrate password hashing for a basic C++ internship project. Production applications should use a dedicated password hashing algorithm such as Argon2, bcrypt, or scrypt.
