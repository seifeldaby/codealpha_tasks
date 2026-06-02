/*
 * ============================================================
 *  CodeAlpha Internship — Task 4: Banking System
 *  Author : Seif Eldaby
 *
 *  Classes  : Customer, Account, Transaction, Bank
 *  Features :
 *    • Create customers & accounts
 *    • Deposit, withdraw, transfer
 *    • Transaction history (last 10)
 *    • Persistent storage in "bank.db"
 *    • PIN-protected account access
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <limits>
using namespace std;

// ── utils ─────────────────────────────────────────────────────
string currentTimestamp() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

string generateAccountNumber() {
    srand((unsigned)time(nullptr) ^ rand());
    return "ACC" + to_string(100000 + rand() % 900000);
}

// ── Transaction ───────────────────────────────────────────────
struct Transaction {
    string type;      // DEPOSIT / WITHDRAW / TRANSFER_IN / TRANSFER_OUT
    double amount;
    double balanceAfter;
    string timestamp;
    string note;

    string serialize() const {
        ostringstream ss;
        ss << type << "|" << fixed << setprecision(2) << amount << "|"
           << balanceAfter << "|" << timestamp << "|" << note;
        return ss.str();
    }

    static Transaction deserialize(const string& line) {
        Transaction t;
        istringstream ss(line);
        string tok;
        getline(ss, t.type,         '|');
        getline(ss, tok,            '|'); t.amount       = stod(tok);
        getline(ss, tok,            '|'); t.balanceAfter = stod(tok);
        getline(ss, t.timestamp,    '|');
        getline(ss, t.note);
        return t;
    }
};

// ── Account ───────────────────────────────────────────────────
class Account {
public:
    string accNumber;
    string ownerID;       // links to Customer::id
    double balance;
    string pin;           // 4-digit PIN (stored as plain for demo)
    vector<Transaction> history;

    Account() : balance(0) {}
    Account(const string& owner, const string& pinCode, double initial = 0)
        : accNumber(generateAccountNumber()), ownerID(owner),
          balance(initial), pin(pinCode) {
        if (initial > 0) {
            Transaction t{"DEPOSIT", initial, initial, currentTimestamp(), "Initial deposit"};
            history.push_back(t);
        }
    }

    bool deposit(double amount, const string& note = "") {
        if (amount <= 0) return false;
        balance += amount;
        history.push_back({"DEPOSIT", amount, balance, currentTimestamp(),
                           note.empty() ? "Deposit" : note});
        if (history.size() > 50) history.erase(history.begin());
        return true;
    }

    bool withdraw(double amount, const string& note = "") {
        if (amount <= 0 || amount > balance) return false;
        balance -= amount;
        history.push_back({"WITHDRAW", amount, balance, currentTimestamp(),
                           note.empty() ? "Withdrawal" : note});
        if (history.size() > 50) history.erase(history.begin());
        return true;
    }

    void printHistory(int last = 10) const {
        cout << "\n  ┌──────────────────────────────────────────────────────┐\n";
        cout << "  │         Transaction History (last " << last << ")              │\n";
        cout << "  ├──────────────┬───────────┬───────────┬───────────────┤\n";
        cout << "  │ Type         │   Amount  │  Balance  │ Date/Time     │\n";
        cout << "  ├──────────────┼───────────┼───────────┼───────────────┤\n";

        int start = max(0, (int)history.size() - last);
        for (int i = start; i < (int)history.size(); ++i) {
            const auto& t = history[i];
            cout << "  │ " << left << setw(12) << t.type
                 << " │ " << right << setw(9) << fixed << setprecision(2) << t.amount
                 << " │ " << setw(9) << t.balanceAfter
                 << " │ " << t.timestamp.substr(0, 16)
                 << " │\n";
        }
        cout << "  └──────────────┴───────────┴───────────┴───────────────┘\n";
    }

    // serialization
    string serialize() const {
        ostringstream ss;
        ss << accNumber << "," << ownerID << ","
           << fixed << setprecision(2) << balance << "," << pin << "\n";
        // transactions on next lines prefixed with TAB
        for (const auto& t : history)
            ss << "\t" << t.serialize() << "\n";
        ss << "---\n";
        return ss.str();
    }
};

// ── Customer ──────────────────────────────────────────────────
class Customer {
public:
    string id;
    string name;
    string phone;
    vector<string> accountNumbers;

    Customer() {}
    Customer(const string& n, const string& p)
        : name(n), phone(p) {
        srand((unsigned)time(nullptr) ^ rand());
        id = "CUS" + to_string(10000 + rand() % 90000);
    }

    string serialize() const {
        ostringstream ss;
        ss << id << "|" << name << "|" << phone << "|";
        for (size_t i = 0; i < accountNumbers.size(); ++i) {
            if (i) ss << ";";
            ss << accountNumbers[i];
        }
        return ss.str();
    }

    static Customer deserialize(const string& line) {
        Customer c;
        istringstream ss(line);
        string tok;
        getline(ss, c.id,    '|');
        getline(ss, c.name,  '|');
        getline(ss, c.phone, '|');
        string accs;
        getline(ss, accs);
        istringstream as(accs);
        string acc;
        while (getline(as, acc, ';'))
            if (!acc.empty()) c.accountNumbers.push_back(acc);
        return c;
    }
};

// ── Bank ──────────────────────────────────────────────────────
class Bank {
    vector<Customer> customers;
    vector<Account>  accounts;
    const string DB = "bank.db";

    Customer* findCustomer(const string& id) {
        for (auto& c : customers) if (c.id == id) return &c;
        return nullptr;
    }

    Account* findAccount(const string& accNum) {
        for (auto& a : accounts) if (a.accNumber == accNum) return &a;
        return nullptr;
    }

public:
    Bank() { load(); }
    ~Bank() { save(); }

    // ── persistence (simple text format) ─────────────────────
    void save() const {
        ofstream f(DB);
        f << "[CUSTOMERS]\n";
        for (const auto& c : customers)
            f << c.serialize() << "\n";
        f << "[ACCOUNTS]\n";
        for (const auto& a : accounts)
            f << a.serialize();
    }

    void load() {
        ifstream f(DB);
        if (!f) return;
        string line, section;
        Account* curAcc = nullptr;
        while (getline(f, line)) {
            if (line == "[CUSTOMERS]") { section = "C"; continue; }
            if (line == "[ACCOUNTS]") { section = "A"; curAcc = nullptr; continue; }
            if (section == "C" && !line.empty())
                customers.push_back(Customer::deserialize(line));
            if (section == "A") {
                if (line == "---") { curAcc = nullptr; continue; }
                if (!line.empty() && line[0] == '\t') {
                    if (curAcc)
                        curAcc->history.push_back(Transaction::deserialize(line.substr(1)));
                } else {
                    accounts.emplace_back();
                    curAcc = &accounts.back();
                    istringstream ss(line);
                    string tok;
                    getline(ss, curAcc->accNumber, ',');
                    getline(ss, curAcc->ownerID,   ',');
                    getline(ss, tok,               ','); curAcc->balance = stod(tok);
                    getline(ss, curAcc->pin);
                }
            }
        }
    }

    // ── public actions ────────────────────────────────────────
    void createCustomer() {
        cout << "\n── New Customer ──\n";
        string name, phone;
        cout << "  Full name : "; cin.ignore(); getline(cin, name);
        cout << "  Phone     : "; getline(cin, phone);

        Customer c(name, phone);
        customers.push_back(c);
        cout << "  ✓ Customer created. ID: " << c.id << "\n";
        save();
    }

    void createAccount() {
        cout << "\n── Open Account ──\n";
        string cid;
        cout << "  Customer ID: "; cin >> cid;

        Customer* c = findCustomer(cid);
        if (!c) { cout << "  ✗ Customer not found.\n"; return; }

        string pin;
        cout << "  Set 4-digit PIN: "; cin >> pin;
        if (pin.size() != 4 || !all_of(pin.begin(), pin.end(), ::isdigit)) {
            cout << "  ✗ PIN must be exactly 4 digits.\n"; return;
        }

        double initial = 0;
        cout << "  Initial deposit (0 for none): "; cin >> initial;

        Account a(cid, pin, initial);
        c->accountNumbers.push_back(a.accNumber);
        accounts.push_back(a);
        cout << "  ✓ Account opened. Number: " << a.accNumber
             << "  |  Balance: " << fixed << setprecision(2) << a.balance << "\n";
        save();
    }

    void accountMenu(Account& acc) {
        int ch = 0;
        do {
            cout << "\n  Account: " << acc.accNumber
                 << "  |  Balance: EGP " << fixed << setprecision(2) << acc.balance << "\n";
            cout << "  [1] Deposit\n  [2] Withdraw\n  [3] Transfer\n"
                 << "  [4] Transaction History\n  [5] Logout\n\n  Choice: ";
            if (!(cin >> ch)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); ch=0; }

            if (ch == 1) {
                double amt; cout << "  Amount: "; cin >> amt;
                acc.deposit(amt) ? cout << "  ✓ Deposit successful.\n"
                                 : cout << "  ✗ Invalid amount.\n";
            } else if (ch == 2) {
                double amt; cout << "  Amount: "; cin >> amt;
                acc.withdraw(amt) ? cout << "  ✓ Withdrawal successful.\n"
                                  : cout << "  ✗ Insufficient funds or invalid amount.\n";
            } else if (ch == 3) {
                string dest; double amt;
                cout << "  Destination account: "; cin >> dest;
                cout << "  Amount             : "; cin >> amt;
                Account* d = findAccount(dest);
                if (!d) { cout << "  ✗ Destination not found.\n"; }
                else if (!acc.withdraw(amt, "Transfer to " + dest)) {
                    cout << "  ✗ Insufficient funds.\n";
                } else {
                    d->deposit(amt, "Transfer from " + acc.accNumber);
                    cout << "  ✓ Transfer of EGP " << fixed << setprecision(2) << amt << " done.\n";
                }
            } else if (ch == 4) {
                acc.printHistory();
            }
            save();
        } while (ch != 5);
    }

    void login() {
        cout << "\n── Account Login ──\n";
        string accNum, pin;
        cout << "  Account number : "; cin >> accNum;
        cout << "  PIN            : "; cin >> pin;

        Account* a = findAccount(accNum);
        if (!a || a->pin != pin) {
            cout << "  ✗ Invalid account or PIN.\n"; return;
        }
        cout << "  ✓ Access granted.\n";
        accountMenu(*a);
    }

    void listCustomers() {
        if (customers.empty()) { cout << "  No customers yet.\n"; return; }
        cout << "\n  ┌─────────┬─────────────────────┬─────────────┐\n";
        cout << "  │ ID      │ Name                │ Phone       │\n";
        cout << "  ├─────────┼─────────────────────┼─────────────┤\n";
        for (const auto& c : customers) {
            cout << "  │ " << left << setw(7) << c.id
                 << " │ " << setw(19) << c.name
                 << " │ " << setw(11) << c.phone << " │\n";
        }
        cout << "  └─────────┴─────────────────────┴─────────────┘\n";
    }

    void run() {
        cout << "\n╔══════════════════════════════════════╗\n";
        cout << "║      BANKING SYSTEM — CodeAlpha      ║\n";
        cout << "╚══════════════════════════════════════╝\n";

        int ch = 0;
        do {
            cout << "\n  [1] New Customer\n  [2] Open Account\n"
                 << "  [3] Account Login\n  [4] List Customers\n  [5] Exit\n\n  Choice: ";
            if (!(cin >> ch)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(),'\n'); ch=0; }

            switch (ch) {
                case 1: createCustomer(); break;
                case 2: createAccount();  break;
                case 3: login();           break;
                case 4: listCustomers();   break;
                case 5: cout << "\n  Goodbye!\n\n"; break;
                default: cout << "  ⚠  Invalid option.\n"; break;
            }
        } while (ch != 5);
    }
};

// ── main ─────────────────────────────────────────────────────
int main() {
    Bank bank;
    bank.run();
    return 0;
}
