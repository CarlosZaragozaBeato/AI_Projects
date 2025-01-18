#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <map>
#include <algorithm>
#include <numeric>

using namespace std;

// Forward declarations
class Account;
class Transaction;

// AI Components
class TransactionAnalyzer {
private:
    vector<double> avgDailySpending;
    map<string, double> categorySpending;
    double spendingThreshold;
    
    double calculateStandardDeviation(const vector<double>& values) {
        if (values.empty()) return 0.0;
        
        double sum = 0.0;
        double mean = 0.0;
        
        for (double value : values) {
            sum += value;
        }
        mean = sum / values.size();
        
        double squaredDiffSum = 0.0;
        for (double value : values) {
            squaredDiffSum += pow(value - mean, 2);
        }
        
        return sqrt(squaredDiffSum / values.size());
    }

public:
    TransactionAnalyzer() : spendingThreshold(0.0) {}

    bool isFraudulent(const Transaction& transaction);

    void updateSpendingPattern(double amount) {
        avgDailySpending.push_back(abs(amount));
        if (avgDailySpending.size() > 30) { // Keep last 30 days
            avgDailySpending.erase(avgDailySpending.begin());
        }
    }

    vector<string> getRecommendations(const Account& account);
};

// Transaction Class
class Transaction {
private:
    string date;
    string type;
    double amount;
    double balance;
    string category;

public:
    Transaction(string t, double amt, double bal, string cat = "General") 
        : type(t), amount(amt), balance(bal), category(cat) {
        time_t now = time(0);
        date = ctime(&now);
    }

    string getDate() const { return date; }
    string getType() const { return type; }
    double getAmount() const { return amount; }
    double getBalance() const { return balance; }
    string getCategory() const { return category; }
};

// Account Class
class Account {
private:
    string accountNumber;
    string holderName;
    double balance;
    string pin;
    vector<Transaction> transactions;
    TransactionAnalyzer analyzer;

public:
    Account(string accNum, string name, string p, double initialBalance = 0.0) 
        : accountNumber(accNum), holderName(name), pin(p), balance(initialBalance) {
        if (initialBalance > 0) {
            addTransaction("Initial Deposit", initialBalance, "Setup");
        }
    }

    bool verifyPin(const string& enteredPin) const {
        return pin == enteredPin;
    }

    void addTransaction(string type, double amount, string category = "General") {
        Transaction trans(type, amount, balance, category);
        
        // AI Fraud Detection
        if (analyzer.isFraudulent(trans)) {
            cout << "\nWARNING: Unusual transaction detected! Please verify this transaction." << endl;
            cout << "Amount: $" << amount << " Type: " << type << endl;
            cout << "Do you want to proceed? (y/n): ";
            char choice;
            cin >> choice;
            if (choice != 'y') {
                throw runtime_error("Transaction cancelled due to fraud detection");
            }
        }

        transactions.push_back(trans);
        analyzer.updateSpendingPattern(amount);
    }

    void deposit(double amount, string category = "Deposit") {
        if (amount > 0) {
            balance += amount;
            addTransaction("Deposit", amount, category);
        } else {
            throw runtime_error("Invalid deposit amount");
        }
    }

    bool withdraw(double amount, string category = "Withdrawal") {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            addTransaction("Withdrawal", -amount, category);
            return true;
        }
        return false;
    }

    void displayTransactionHistory() const {
        cout << "\nTransaction History for Account " << accountNumber << endl;
        cout << setfill('-') << setw(100) << "-" << setfill(' ') << endl;
        cout << setw(30) << left << "Date" 
             << setw(15) << "Type"
             << setw(15) << "Amount"
             << setw(15) << "Balance"
             << setw(15) << "Category" << endl;
        cout << setfill('-') << setw(100) << "-" << setfill(' ') << endl;

        for (const auto& trans : transactions) {
            cout << setw(30) << left << trans.getDate()
                 << setw(15) << trans.getType()
                 << setw(15) << fixed << setprecision(2) << trans.getAmount()
                 << setw(15) << trans.getBalance()
                 << setw(15) << trans.getCategory() << endl;
        }
    }

    void getAIRecommendations() {
        vector<string> recommendations = analyzer.getRecommendations(*this);
        if (!recommendations.empty()) {
            cout << "\nAI Banking Recommendations:" << endl;
            cout << setfill('-') << setw(50) << "-" << setfill(' ') << endl;
            for (const auto& rec : recommendations) {
                cout << "- " << rec << endl;
            }
        }
    }

    string getAccountNumber() const { return accountNumber; }
    string getHolderName() const { return holderName; }
    double getBalance() const { return balance; }
    const vector<Transaction>& getTransactions() const { return transactions; }

    void saveToFile() const {
        ofstream file("account_" + accountNumber + ".txt");
        if (file.is_open()) {
            file << accountNumber << endl;
            file << holderName << endl;
            file << pin << endl;
            file << balance << endl;
            
            for (const auto& trans : transactions) {
                file << trans.getDate() << ","
                     << trans.getType() << ","
                     << trans.getAmount() << ","
                     << trans.getBalance() << ","
                     << trans.getCategory() << endl;
            }
            file.close();
        }
    }
};

// Implementation of TransactionAnalyzer methods that depend on Account
bool TransactionAnalyzer::isFraudulent(const Transaction& transaction) {
    if (avgDailySpending.empty()) return false;
    
    double stdDev = calculateStandardDeviation(avgDailySpending);
    double mean = accumulate(avgDailySpending.begin(), avgDailySpending.end(), 0.0) / avgDailySpending.size();
    
    return abs(transaction.getAmount()) > (mean + 3 * stdDev);
}

vector<string> TransactionAnalyzer::getRecommendations(const Account& account) {
    vector<string> recommendations;
    double balance = account.getBalance();
    double avgSpending = 0.0;
    
    if (!avgDailySpending.empty()) {
        avgSpending = accumulate(avgDailySpending.begin(), avgDailySpending.end(), 0.0) / avgDailySpending.size();
    }

    if (balance < avgSpending * 3) {
        recommendations.push_back("Warning: Your balance is lower than 3x your average daily spending.");
    }

    if (balance > avgSpending * 30) {
        recommendations.push_back("Consider investing excess funds for better returns.");
    }

    if (avgDailySpending.size() >= 7) {
        double recentAvg = accumulate(avgDailySpending.end() - 7, avgDailySpending.end(), 0.0) / 7;
        if (recentAvg > avgSpending * 1.5) {
            recommendations.push_back("Your recent spending is 50% higher than your average. Consider budgeting.");
        }
    }

    return recommendations;
}

// Bank Class
class Bank {
private:
    vector<Account*> accounts;

public:
    ~Bank() {
        for (auto account : accounts) {
            delete account;
        }
    }

    void createAccount(string name, string pin) {
        string accNum = generateAccountNumber();
        accounts.push_back(new Account(accNum, name, pin));
        cout << "Account created successfully. Account number: " << accNum << endl;
    }

    Account* findAccount(const string& accNum) {
        for (auto account : accounts) {
            if (account->getAccountNumber() == accNum) {
                return account;
            }
        }
        return nullptr;
    }

    string generateAccountNumber() {
        return "ACC" + to_string(accounts.size() + 1001);
    }

    void saveAllAccounts() {
        for (const auto& account : accounts) {
            account->saveToFile();
        }
    }
};

// Menu function
void displayAccountMenu(Account* acc) {
    string choice;
    while (true) {
        cout << "\nAI-Enhanced Account Menu\n";
        cout << "1. Check Balance\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Transaction History\n";
        cout << "5. Get AI Recommendations\n";
        cout << "6. Return to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == "1") {
            cout << "Current balance: $" << fixed << setprecision(2) 
                 << acc->getBalance() << endl;
        }
        else if (choice == "2") {
            double amount;
            string category;
            cout << "Enter amount to deposit: $";
            cin >> amount;
            cout << "Enter category (or press enter for default): ";
            cin.ignore();
            getline(cin, category);
            if (category.empty()) category = "Deposit";
            
            try {
                acc->deposit(amount, category);
                cout << "Deposit successful\n";
            } catch (const runtime_error& e) {
                cout << "Error: " << e.what() << endl;
            }
        }
        else if (choice == "3") {
            double amount;
            string category;
            cout << "Enter amount to withdraw: $";
            cin >> amount;
            cout << "Enter category (or press enter for default): ";
            cin.ignore();
            getline(cin, category);
            if (category.empty()) category = "Withdrawal";
            
            if (acc->withdraw(amount, category)) {
                cout << "Withdrawal successful\n";
            } else {
                cout << "Insufficient funds or invalid amount\n";
            }
        }
        else if (choice == "4") {
            acc->displayTransactionHistory();
        }
        else if (choice == "5") {
            acc->getAIRecommendations();
        }
        else if (choice == "6") {
            break;
        }
    }
}

// Main function
int main() {
    Bank bank;
    string choice;

    while (true) {
        cout << "\nAI-Enhanced Bank Account System\n";
        cout << "1. Create New Account\n";
        cout << "2. Access Existing Account\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == "1") {
            string name, pin;
            cout << "Enter name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter PIN: ";
            cin >> pin;
            bank.createAccount(name, pin);
        }
        else if (choice == "2") {
            string accNum, pin;
            cout << "Enter account number: ";
            cin >> accNum;
            Account* acc = bank.findAccount(accNum);
            
            if (acc) {
                cout << "Enter PIN: ";
                cin >> pin;
                
                if (acc->verifyPin(pin)) {
                    displayAccountMenu(acc);
                } else {
                    cout << "Invalid PIN\n";
                }
            } else {
                cout << "Account not found\n";
            }
        }
        else if (choice == "3") {
            bank.saveAllAccounts();
            cout << "Thank you for using our AI-Enhanced banking system!\n";
            break;
        }
    }
    return 0;
}