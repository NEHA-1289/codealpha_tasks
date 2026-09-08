#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
using namespace std;

class Transaction {
public:
    string type;
    double amount;
    string details;
    string date;

    Transaction(string t, double a, string d) {
        type = t;
        amount = a;
        details = d;

        time_t now = time(0);
        date = ctime(&now);
        date.pop_back();
    }

    void display() {
        cout << left << setw(15) << type
             << setw(12) << fixed << setprecision(2) << amount
             << setw(25) << details
             << date << endl;
    }
};

class Customer {
public:
    int customerId;
    string name;
    string phone;

    Customer(int id, string n, string p) {
        customerId = id;
        name = n;
        phone = p;
    }

    void display() {
        cout << "\nCustomer ID: " << customerId << endl;
        cout << "Name: " << name << endl;
        cout << "Phone: " << phone << endl;
    }
};

class Account {
public:
    int accountNumber;
    int customerId;
    double balance;
    vector<Transaction> transactions;

    Account(int accNo, int custId) {
        accountNumber = accNo;
        customerId = custId;
        balance = 0;
    }

    void deposit(double amount) {
        if (amount <= 0) {
            cout << "Invalid deposit amount.\n";
            return;
        }

        balance += amount;

        transactions.push_back(
            Transaction("Deposit", amount, "Money deposited")
        );

        cout << "Deposit successful.\n";
        cout << "Current balance: Rs. " << balance << endl;
    }

    void withdraw(double amount) {
        if (amount <= 0) {
            cout << "Invalid withdrawal amount.\n";
            return;
        }

        if (amount > balance) {
            cout << "Insufficient balance.\n";
            return;
        }

        balance -= amount;

        transactions.push_back(
            Transaction("Withdrawal", amount, "Money withdrawn")
        );

        cout << "Withdrawal successful.\n";
        cout << "Current balance: Rs. " << balance << endl;
    }

    bool transfer(Account &receiver, double amount) {
        if (amount <= 0) {
            cout << "Invalid transfer amount.\n";
            return false;
        }

        if (amount > balance) {
            cout << "Insufficient balance.\n";
            return false;
        }

        balance -= amount;
        receiver.balance += amount;

        transactions.push_back(
            Transaction("Transfer", amount,
                        "Transferred to Account " +
                        to_string(receiver.accountNumber))
        );

        receiver.transactions.push_back(
            Transaction("Received", amount,
                        "Received from Account " +
                        to_string(accountNumber))
        );

        cout << "Transfer successful.\n";
        cout << "Current balance: Rs. " << balance << endl;

        return true;
    }

    void displayAccount() {
        cout << "\n========== ACCOUNT INFORMATION ==========\n";
        cout << "Account Number: " << accountNumber << endl;
        cout << "Customer ID: " << customerId << endl;
        cout << "Balance: Rs. " << fixed << setprecision(2)
             << balance << endl;
    }

    void showTransactions() {
        cout << "\n================ TRANSACTION HISTORY ================\n";

        if (transactions.empty()) {
            cout << "No transactions found.\n";
            return;
        }

        cout << left << setw(15) << "Type"
             << setw(12) << "Amount"
             << setw(25) << "Details"
             << "Date" << endl;

        cout << string(80, '-') << endl;

        for (int i = transactions.size() - 1; i >= 0; i--) {
            transactions[i].display();
        }
    }
};

vector<Customer> customers;
vector<Account> accounts;

void saveCustomers() {
    ofstream file("customers.txt");

    for (Customer c : customers) {
        file << c.customerId << " "
             << c.name << " "
             << c.phone << endl;
    }

    file.close();
}

void saveAccounts() {
    ofstream file("accounts.txt");

    for (Account a : accounts) {
        file << a.accountNumber << " "
             << a.customerId << " "
             << a.balance << endl;
    }

    file.close();
}

void createCustomer() {
    int id;
    string name, phone;

    cout << "\n========== CREATE CUSTOMER ==========\n";

    cout << "Enter Customer ID: ";
    cin >> id;

    cout << "Enter Name: ";
    cin >> name;

    cout << "Enter Phone Number: ";
    cin >> phone;

    customers.push_back(Customer(id, name, phone));

    saveCustomers();

    cout << "Customer created successfully.\n";
}

void createAccount() {
    int accountNumber, customerId;

    cout << "\n========== CREATE ACCOUNT ==========\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    cout << "Enter Customer ID: ";
    cin >> customerId;

    bool customerFound = false;

    for (Customer c : customers) {
        if (c.customerId == customerId) {
            customerFound = true;
            break;
        }
    }

    if (!customerFound) {
        cout << "Customer not found.\n";
        return;
    }

    accounts.push_back(Account(accountNumber, customerId));

    saveAccounts();

    cout << "Account created successfully.\n";
}

Account* findAccount(int accountNumber) {
    for (auto &account : accounts) {
        if (account.accountNumber == accountNumber) {
            return &account;
        }
    }

    return nullptr;
}

void depositMoney() {
    int accountNumber;
    double amount;

    cout << "\n========== DEPOSIT ==========\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    Account* account = findAccount(accountNumber);

    if (account == nullptr) {
        cout << "Account not found.\n";
        return;
    }

    cout << "Enter amount: ";
    cin >> amount;

    account->deposit(amount);

    saveAccounts();
}

void withdrawMoney() {
    int accountNumber;
    double amount;

    cout << "\n========== WITHDRAW ==========\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    Account* account = findAccount(accountNumber);

    if (account == nullptr) {
        cout << "Account not found.\n";
        return;
    }

    cout << "Enter amount: ";
    cin >> amount;

    account->withdraw(amount);

    saveAccounts();
}

void transferMoney() {
    int senderAccount, receiverAccount;
    double amount;

    cout << "\n========== FUND TRANSFER ==========\n";

    cout << "Enter Sender Account Number: ";
    cin >> senderAccount;

    cout << "Enter Receiver Account Number: ";
    cin >> receiverAccount;

    Account* sender = findAccount(senderAccount);
    Account* receiver = findAccount(receiverAccount);

    if (sender == nullptr || receiver == nullptr) {
        cout << "One or both accounts not found.\n";
        return;
    }

    if (sender == receiver) {
        cout << "Sender and receiver accounts cannot be the same.\n";
        return;
    }

    cout << "Enter amount: ";
    cin >> amount;

    sender->transfer(*receiver, amount);

    saveAccounts();
}

void showAccount() {
    int accountNumber;

    cout << "\n========== ACCOUNT DETAILS ==========\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    Account* account = findAccount(accountNumber);

    if (account == nullptr) {
        cout << "Account not found.\n";
        return;
    }

    account->displayAccount();
}

void showTransactionHistory() {
    int accountNumber;

    cout << "\n========== TRANSACTIONS ==========\n";

    cout << "Enter Account Number: ";
    cin >> accountNumber;

    Account* account = findAccount(accountNumber);

    if (account == nullptr) {
        cout << "Account not found.\n";
        return;
    }

    account->showTransactions();
}

void showCustomers() {
    cout << "\n========== CUSTOMERS ==========\n";

    if (customers.empty()) {
        cout << "No customers found.\n";
        return;
    }

    for (Customer c : customers) {
        c.display();
        cout << "-------------------------\n";
    }
}

int main() {
    int choice;

    while (true) {
        cout << "\n\n========================================\n";
        cout << "          BANKING SYSTEM\n";
        cout << "========================================\n";

        cout << "1. Create Customer\n";
        cout << "2. Create Account\n";
        cout << "3. Deposit Money\n";
        cout << "4. Withdraw Money\n";
        cout << "5. Transfer Funds\n";
        cout << "6. View Account Information\n";
        cout << "7. View Transaction History\n";
        cout << "8. View Customers\n";
        cout << "9. Exit\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                createCustomer();
                break;

            case 2:
                createAccount();
                break;

            case 3:
                depositMoney();
                break;

            case 4:
                withdrawMoney();
                break;

            case 5:
                transferMoney();
                break;

            case 6:
                showAccount();
                break;

            case 7:
                showTransactionHistory();
                break;

            case 8:
                showCustomers();
                break;

            case 9:
                cout << "Thank you for using the Banking System.\n";
                return 0;

            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}