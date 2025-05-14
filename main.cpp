#include<iostream>
#include<fstream>
#include<string>
using namespace std;

// ======================== ENCRYPTION SYSTEM ========================

// Function to encrypt and store password
void storePassword(const string& username, const string& password) {
    char key = 'K'; // XOR key
    string encrypted;
    for (char c : password) {
        encrypted += c ^ key;
    }
    ofstream file("passwords.txt", ios::app);
    file << username << " " << encrypted << "\n";
    file.close();
}

// Function to verify password during login
bool verifyPassword(const string& username, const string& inputPassword) {
    ifstream file("passwords.txt");
    if (!file) {
        cerr << "Error opening passwords.txt\n";
        return false;
    }

    string storedUser, storedEncrypted;
    char key = 'K';

    while (file >> storedUser >> storedEncrypted) {
        if (storedUser == username) {
            // Decrypt the stored encrypted password
            string decrypted;
            for (char c : storedEncrypted) {
                decrypted += c ^ key;
            }

            // First check if password matches
            if (decrypted == inputPassword) {
                // Now check if the account is locked
                ifstream lockFile("locked.txt");
                if (lockFile) {
                    string lockedUser;
                    while (lockFile >> lockedUser) {
                        if (lockedUser == username) {
                            cout << "Account is locked. Contact administrator.\n";
                            return false; // even if password matches
                        }
                    }
                }
                return true; // password matches and not locked
            } else {
                return false; // password wrong
            }
        }
    }

    return false; // username not found
}
// ======================== CLASS DEFINITIONS ========================

class User {
protected:
    string username;
    string password;
public:
    User(string u = "", string p = "") : username(u), password(p) {}
    string getUsername() { return username; }
    string getPassword() { return password; }
    void setUsername(string u) { username = u; }
    void setPassword(string p) { password = p; }
};

class Customer : public User {
public:
    Customer(string u = "", string p = "") : User(u, p) {}

    void applyLoan();
    void depositRequest();
    void withdrawRequest();
    void trackLoan();
    void showBalance();
};

class Employee : public User {
protected:
    string role;
public:
    Employee(string u = "", string p = "", string r = "") : User(u, p), role(r) {}
    string getRole() { return role; }
};

class Manager : public Employee {
public:
    Manager(string u = "", string p = "", string r = "Manager") : Employee(u, p, r) {}

    void approveRequests();
    void approveLoans();
    void approveRegistrationRequests();
};
// ======================== CUSTOMER FUNCTIONS ========================

void Customer::applyLoan() {
    double amount;
    cout << "\nEnter Loan Amount to Apply: ";
    cin >> amount;
    ofstream loanFile("loans.txt", ios::app);
    loanFile << username << " " << amount << " Pending" << endl;
    loanFile.close();
    cout << "Loan Application Submitted Successfully! Pending Approval.\n";
}

void Customer::depositRequest() {
    double amount;
    cout << "\nEnter Amount to Deposit: ";
    cin >> amount;
    ofstream reqFile("requests.txt", ios::app);
    reqFile << username << " Deposit " << amount << " Pending" << endl;
    reqFile.close();
    cout << "Deposit Request Submitted. Awaiting Manager Approval.\n";
}

void Customer::withdrawRequest() {
    double amount;
    cout << "\nEnter Amount to Withdraw: ";
    cin >> amount;
    ofstream reqFile("requests.txt", ios::app);
    reqFile << username << " Withdraw " << amount << " Pending" << endl;
    reqFile.close();
    cout << "Withdraw Request Submitted. Awaiting Manager Approval.\n";
}

void Customer::trackLoan() {
    ifstream loanFile("loans.txt");
    string uname, status;
    double amt;
    bool found = false;
    while (loanFile >> uname >> amt >> status) {
        if (uname == username) {
            cout << "\nLoan Amount: " << amt << " Status: " << status << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "\nNo loan applications found for this user.\n";
    }
    loanFile.close();
}

void Customer::showBalance() {
    ifstream transFile("transactions.txt");
    string uname, type;
    double amt;
    double balance = 0;
    while (transFile >> uname >> type >> amt) {
        if (uname == username) {
            if (type == "Deposit") balance += amt;
            else if (type == "Withdraw") balance -= amt;
        }
    }
    cout << "\nCurrent Balance: " << balance << endl;
    transFile.close();
}

// ======================== MANAGER FUNCTIONS ========================

void Manager::approveRequests() {
    ifstream reqFile("requests.txt");
    ofstream tempFile("temp.txt");
    ofstream transFile("transactions.txt", ios::app);

    string uname, type, status;
    double amt;
    int choice;

    while (reqFile >> uname >> type >> amt >> status) {
        if (status == "Pending") {
            cout << "\nRequest: " << uname << " - " << type << " - " << amt << endl;
            cout << "Approve (1) or Reject (0)? ";
            cin >> choice;
            if (choice == 1) {
                tempFile << uname << " " << type << " " << amt << " Approved" << endl;
                transFile << uname << " " << type << " " << amt << endl;
            } else {
                tempFile << uname << " " << type << " " << amt << " Rejected" << endl;
            }
        } else {
            tempFile << uname << " " << type << " " << amt << " " << status << endl;
        }
    }

    reqFile.close();
    tempFile.close();
    transFile.close();

    remove("requests.txt");
    rename("temp.txt", "requests.txt");

    cout << "\nAll pending deposit/withdraw requests processed.\n";
}
void Manager::approveRegistrationRequests() {
    ifstream requestsFile("customerRegistrationRequests.txt");
    ofstream tempFile("temp.txt");
    string username, password, dob;
    char approval;
    const char key = 'K'; // for encryption

    if (requestsFile.is_open() && tempFile.is_open()) {
        while (requestsFile >> username >> password >> dob) {
            cout << "Approve registration for " << username << "? (Y/N): ";
            cin >> approval;

            if (approval == 'Y' || approval == 'y') {
                // Add to users.txt
                ofstream usersFile("users.txt", ios::app);
                if (usersFile.is_open()) {
                    usersFile << username << " Customer" << endl;
                    usersFile.close();
                }

                // Encrypt password and add to passwords.txt
                string encryptedPassword;
                for (char c : password) {
                    encryptedPassword += c ^ key;
                }
                ofstream passwordsFile("passwords.txt", ios::app);
                if (passwordsFile.is_open()) {
                    passwordsFile << username << " " << encryptedPassword << endl;
                    passwordsFile.close();
                }

                cout << "Registration approved for " << username << endl;
            } else {
                // Keep in temp file if not approved yet
                tempFile << username << " " << password << " " << dob << endl;
            }
        }
        requestsFile.close();
        tempFile.close();

        remove("customerRegistrationRequests.txt");
        rename("temp.txt", "customerRegistrationRequests.txt");
    } else {
        cout << "Error opening files!" << endl;
    }
}
void Manager::approveLoans() {
    ifstream loanFile("loans.txt");
    ofstream tempFile("tempLoan.txt");

    string uname, status;
    double amt;
    int choice;

    while (loanFile >> uname >> amt >> status) {
        if (status == "Pending") {
            cout << "\nLoan Request: " << uname << " - " << amt << endl;
            cout << "Approve (1) or Reject (0)? ";
            cin >> choice;
            if (choice == 1) {
                tempFile << uname << " " << amt << " Approved" << endl;
            } else {
                tempFile << uname << " " << amt << " Rejected" << endl;
            }
        } else {
            tempFile << uname << " " << amt << " " << status << endl;
        }
    }

    loanFile.close();
    tempFile.close();

    remove("loans.txt");
    rename("tempLoan.txt", "loans.txt");

    cout << "\nAll pending loan applications processed.\n";
}
// ======================== EMPLOYEE MENUS ========================

void ceoMenu() {
    int choice;
    do {
        cout << "\n========= CEO Menu =========";
        cout << "\n1. View Employees";
        cout << "\n2. View Customers";
        cout << "\n3. Logout";
        cout << "\nEnter Choice: ";
        cin >> choice;

        ifstream empFile, custFile;
        string uname, pass, role;
        switch (choice) {
            case 1:
                empFile.open("employees.txt");
                cout << "\nEmployees:\n";
                while (empFile >> uname >> pass >> role) {
                    cout << "Username: " << uname << " | Role: " << role << endl;
                }
                empFile.close();
                break;
            case 2:
                custFile.open("users.txt");
                cout << "\nCustomers:\n";
                while (custFile >> uname >> role) {
                    cout << "Username: " << uname << endl;
                }
                custFile.close();
                break;
            case 3:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\nInvalid choice.\n";
        }
    } while (choice != 3);
}

void cfoMenu() {
    int choice;
    do {
        cout << "\n========= CFO Menu =========";
        cout << "\n1. View Financial Summary";
        cout << "\n2. View Transaction History";
        cout << "\n3. Logout";
        cout << "\nEnter Choice: ";
        cin >> choice;

        ifstream transFile;
        string uname, type;
        double amt;
        double totalDeposits = 0, totalWithdraws = 0;

        switch (choice) {
            case 1:
                transFile.open("transactions.txt");
                while (transFile >> uname >> type >> amt) {
                    if (type == "Deposit") totalDeposits += amt;
                    else if (type == "Withdraw") totalWithdraws += amt;
                }
                transFile.close();
                cout << "\nTotal Deposited: " << totalDeposits;
                cout << "\nTotal Withdrawn: " << totalWithdraws << endl;
                break;
            case 2:
                transFile.open("transactions.txt");
                cout << "\nTransaction History:\n";
                while (transFile >> uname >> type >> amt) {
                    cout << uname << " | " << type << " | " << amt << endl;
                }
                transFile.close();
                break;
            case 3:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\nInvalid choice.\n";
        }
    } while (choice != 3);
}

void croMenu() {
    int choice;
    do {
        cout << "\n========= CRO Menu =========";
        cout << "\n1. View Suspicious Transactions";
        cout << "\n2. Generate Risk Report";
        cout << "\n3. Logout";
        cout << "\nEnter Choice: ";
        cin >> choice;

        ifstream transFile;
        string uname, type;
        double amt;

        switch (choice) {
            case 1:
                transFile.open("transactions.txt");
                cout << "\nSuspicious Transactions (Amount > 100000):\n";
                while (transFile >> uname >> type >> amt) {
                    if (amt > 100000) {
                        cout << uname << " | " << type << " | " << amt << endl;
                    }
                }
                transFile.close();
                break;
            case 2:
                cout << "\nRisk Report Generated Successfully.\n";
                break;
            case 3:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\nInvalid choice.\n";
        }
    } while (choice != 3);
}

void ciskMenu() {
    int choice;
    do {
        cout << "\n========= CISK Menu =========";
        cout << "\n1. Lock Account";
        cout << "\n2. Unlock Account";
        cout << "\n3. View Security Logs";
        cout << "\n4. Logout";
        cout << "\nEnter Choice: ";
        cin >> choice;

        string uname;
        ofstream lockFile;
        ifstream lockFileIn;
        ofstream tempFile;
        string lockedUser;

        switch (choice) {
            case 1:
                cout << "\nEnter Username to Lock: ";
                cin >> uname;
                lockFile.open("locked.txt", ios::app);
                lockFile << uname << endl;
                lockFile.close();
                cout << "\nAccount Locked Successfully.\n";
                break;
            case 2:
                cout << "\nEnter Username to Unlock: ";
                cin >> uname;
                lockFileIn.open("locked.txt");
                tempFile.open("tempLock.txt");
                while (lockFileIn >> lockedUser) {
                    if (lockedUser != uname) {
                        tempFile << lockedUser << endl;
                    }
                }
                lockFileIn.close();
                tempFile.close();
                remove("locked.txt");
                rename("tempLock.txt", "locked.txt");
                cout << "\nAccount Unlocked Successfully.\n";
                break;
            case 3:
                cout << "\nSecurity logs displayed successfully.\n";
                break;
            case 4:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\nInvalid choice.\n";
        }
    } while (choice != 4);
}
// ======================== LOGIN SYSTEM ========================

bool customerLogin(string& username) {
    ifstream users("users.txt");
    string uname, pass;
    string inputUser, inputPass;
    cout << "\nEnter Username: ";
    cin >> inputUser;
    cout << "Enter Password: ";
    cin >> inputPass;

    if (!verifyPassword(inputUser, inputPass)) {
        cout << "\nInvalid Username or Password.\n";
        return false;
    }

    while (users >> uname >> pass ) {
        if (uname == inputUser) {
            username = uname;
            users.close();
            return true;
        }
    }
    users.close();
    return false;
}

bool employeeLogin(string& username, string& role) {
    ifstream employees("employees.txt");
    string uname, pass, r;
    string inputUser, inputPass;
    cout << "\nEnter Username: ";
    cin >> inputUser;
    cout << "Enter Password: ";
    cin >> inputPass;

    if (!verifyPassword(inputUser, inputPass)) {
        cout << "\nInvalid Username or Password.\n";
        return false;
    }

    while (employees >> uname >> pass >> r) {
        if (uname == inputUser) {
            username = uname;
            role = r;
            employees.close();
            return true;
        }
    }
    employees.close();
    return false;
}

// ======================== CUSTOMER MENU ========================

void customerMenu(Customer c) {
    int choice;
    do {
        cout << "\n========= Customer Menu =========";
        cout << "\n1. Deposit Request";
        cout << "\n2. Withdraw Request";
        cout << "\n3. Show Balance";
        cout << "\n4. Apply for Loan";
        cout << "\n5. Track Loan Status";
        cout << "\n6. Logout";
        cout << "\nEnter Choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                c.depositRequest();
                break;
            case 2:
                c.withdrawRequest();
                break;
            case 3:
                c.showBalance();
                break;
            case 4:
                c.applyLoan();
                break;
            case 5:
                c.trackLoan();
                break;
            case 6:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\nInvalid choice. Try again.\n";
        }
    } while (choice != 6);
}

void managerMenu(Manager m) {
    int choice;
    do {
        cout << "\n========= Bank Manager Menu =========";
        cout << "\n1. Approve Deposit/Withdraw Requests";
        cout << "\n2. Approve Loan Applications";
        cout << "\n3. Approve Registration Requests";   // <-- Added
        cout << "\n4. Logout";
        cout << "\nEnter Choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                m.approveRequests();
                break;
            case 2:
                m.approveLoans();
                break;
            case 3:
                m.approveRegistrationRequests();   // <-- Call the new function
                break;
            case 4:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\nInvalid choice. Try again.\n";
        }
    } while (choice != 4);
}

// ======================= CUSTOMER REGISTRATION =======================
void customerRegistrationRequest() {
    string username, password, dob;

    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;
    cout << "Enter Date of Birth (DD/MM/YYYY): ";
    cin >> dob;

    ofstream requestsFile("customerRegistrationRequests.txt", ios::app);
    if (requestsFile.is_open()) {
        requestsFile << username << " " << password << " " << dob << endl;
        cout << "Registration request submitted successfully!" << endl;
        requestsFile.close();
    } else {
        cout << "Unable to open requests file!" << endl;
    }
}
void trackRegistrationRequest() {
    string username;
    cout << "Enter your username to track registration: ";
    cin >> username;

    bool foundInRequests = false, foundInUsers = false;
    string fileUsername, filePasswordOrRole, dobOrRole;

    // First check in requests.txt
    ifstream requestsFile("customerRegistrationRequests.txt");
    if (requestsFile.is_open()) {
        while (requestsFile >> fileUsername >> filePasswordOrRole >> dobOrRole) {
            if (fileUsername == username) {
                foundInRequests = true;
                break;
            }
        }
        requestsFile.close();
    }

    // If not found, check in users.txt
    if (!foundInRequests) {
        ifstream usersFile("users.txt");
        if (usersFile.is_open()) {
            while (usersFile >> fileUsername >> filePasswordOrRole) {
                if (fileUsername == username) {
                    foundInUsers = true;
                    break;
                }
            }
            usersFile.close();
        }
    }

    if (foundInRequests) {
        cout << "Your registration request is still pending approval." << endl;
    } else if (foundInUsers) {
        cout << "Your account has been approved!" << endl;
    } else {
        cout << "No registration request found for this username." << endl;
    }
}

// ======================== MAIN FUNCTION ========================

int main() {
    int choice;
    string username, role;

    do {
        cout << "\n\n\t\t========= Welcome to TechBank =========";
        cout << "\n\t\t1. Customer Login";
        cout << "\n\t\t2. Employee Login";
        cout << "\n\t\t3. Customer Registration Request";
        cout << "\n\t\t4. Track Registration Request" ;
        cout << "\n\t\t5. Exit" ;

        cout << "\n\t\tEnter Choice: ";
        cin >> choice;

        if (choice == 1) {
            if (customerLogin(username)) {
                Customer c(username);
                customerMenu(c);
            }
            else {
                cout << "\nInvalid Customer Login.\n";
            }
        }
        else if (choice == 2) {
            if (employeeLogin(username, role)) {
                if (role == "CEO") ceoMenu();
                else if (role == "CFO") cfoMenu();
                else if (role == "CRO") croMenu();
                else if (role == "CISK") ciskMenu();
                else if (role == "Manager") {
                    Manager m(username);
                    managerMenu(m);
                }
            }
            else {
                cout << "\nInvalid Employee Login.\n";
            }
        }
        else if (choice == 3) {
            customerRegistrationRequest();  // <<< NEW
        }
        else if (choice == 4) {
            trackRegistrationRequest();     // <<< NEW
        }
        else if (choice == 5) {
            cout << "\nExiting TechBank System. Thank you!\n";
        }
        else {
            cout << "\nInvalid choice. Try again.\n";
        }
        
    } while (choice != 5);

    return 0;
}
