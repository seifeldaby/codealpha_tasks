/*
 * ============================================================
 *  CodeAlpha Internship — Task 2: Login & Registration System
 *  Author : Seif Eldaby
 *
 *  Security notes:
 *    • Passwords are stored as a simple SHA-256-style hash using
 *      std::hash (portable, no external deps needed for demo).
 *    • Each user record is one line in "users.db":
 *        username:hashed_password:salt
 *    • Input is validated (length, allowed characters).
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <functional>   // std::hash
#include <limits>
#include <ctime>
using namespace std;

// ── file & constants ─────────────────────────────────────────
const string DB_FILE = "users.db";

// ── simple hash (demo-grade; use bcrypt in production) ───────
string hashPassword(const string& password, const string& salt) {
    string combined = salt + password + "CodeAlpha2026";
    size_t h = hash<string>{}(combined);
    ostringstream oss;
    oss << hex << h;
    return oss.str();
}

string generateSalt() {
    // pseudo-random salt from timestamp + random bits
    srand((unsigned)time(nullptr));
    ostringstream oss;
    oss << hex << (time(nullptr) ^ rand());
    return oss.str();
}

// ── DB helpers ───────────────────────────────────────────────
struct UserRecord {
    string username;
    string hashedPwd;
    string salt;
};

bool userExists(const string& username) {
    ifstream db(DB_FILE);
    string line;
    while (getline(db, line)) {
        istringstream ss(line);
        string u;
        getline(ss, u, ':');
        if (u == username) return true;
    }
    return false;
}

bool saveUser(const UserRecord& rec) {
    ofstream db(DB_FILE, ios::app);
    if (!db) return false;
    db << rec.username << ":" << rec.hashedPwd << ":" << rec.salt << "\n";
    return true;
}

bool verifyUser(const string& username, const string& password) {
    ifstream db(DB_FILE);
    string line;
    while (getline(db, line)) {
        istringstream ss(line);
        string u, h, s;
        getline(ss, u, ':');
        getline(ss, h, ':');
        getline(ss, s, ':');
        if (u == username) {
            return h == hashPassword(password, s);
        }
    }
    return false;
}

// ── input validation ─────────────────────────────────────────
bool isValidUsername(const string& u) {
    if (u.size() < 3 || u.size() > 20) return false;
    for (char c : u)
        if (!isalnum(c) && c != '_') return false;
    return true;
}

bool isValidPassword(const string& p) {
    if (p.size() < 6 || p.size() > 30) return false;
    bool hasUpper = false, hasDigit = false;
    for (char c : p) {
        if (isupper(c)) hasUpper = true;
        if (isdigit(c)) hasDigit = true;
    }
    return hasUpper && hasDigit;
}

// ── masked password input ─────────────────────────────────────
string readPassword(const string& prompt) {
    cout << prompt;
    string pwd;
    // Simple version: just hide with asterisks using character-by-character
    // (works on most terminals; for full cross-platform use platform APIs)
#ifdef _WIN32
    char c;
    while ((c = _getch()) != '\r') {
        if (c == '\b' && !pwd.empty()) {
            pwd.pop_back();
            cout << "\b \b";
        } else if (c != '\b') {
            pwd += c;
            cout << '*';
        }
    }
    cout << '\n';
#else
    // On Linux/Mac, echo can be disabled via termios — fallback to getline
    getline(cin, pwd);
#endif
    return pwd;
}

// ── UI helpers ────────────────────────────────────────────────
void printHeader() {
    cout << "\n╔══════════════════════════════════════╗\n";
    cout << "║  LOGIN & REGISTRATION — CodeAlpha    ║\n";
    cout << "╚══════════════════════════════════════╝\n\n";
}

void printMenu() {
    cout << "  [1] Register\n";
    cout << "  [2] Login\n";
    cout << "  [3] Exit\n";
    cout << "\n  Choice: ";
}

// ── actions ──────────────────────────────────────────────────
void doRegister() {
    cout << "\n── New Registration ──\n";
    string username, password, confirm;

    cout << "  Username (3-20 chars, letters/digits/_): ";
    cin  >> username;
    cin.ignore();

    if (!isValidUsername(username)) {
        cout << "  ✗ Invalid username format.\n";
        return;
    }
    if (userExists(username)) {
        cout << "  ✗ Username already taken.\n";
        return;
    }

    cout << "  Password rules: 6-30 chars, at least 1 uppercase & 1 digit.\n";
    password = readPassword("  Password: ");
    if (!isValidPassword(password)) {
        cout << "  ✗ Password does not meet the requirements.\n";
        return;
    }

    confirm = readPassword("  Confirm Password: ");
    if (password != confirm) {
        cout << "  ✗ Passwords do not match.\n";
        return;
    }

    string salt = generateSalt();
    UserRecord rec{username, hashPassword(password, salt), salt};
    if (saveUser(rec))
        cout << "  ✓ Registration successful! You can now log in.\n";
    else
        cout << "  ✗ Failed to save user. Check file permissions.\n";
}

void doLogin() {
    cout << "\n── Login ──\n";
    string username, password;

    cout << "  Username: ";
    cin  >> username;
    cin.ignore();

    password = readPassword("  Password: ");

    if (verifyUser(username, password)) {
        cout << "\n  ✓ Welcome back, " << username << "! Login successful.\n";
        cout << "  [Session active — press Enter to logout]\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  Logged out.\n";
    } else {
        cout << "  ✗ Invalid username or password.\n";
    }
}

// ── main ─────────────────────────────────────────────────────
int main() {
    printHeader();

    int choice = 0;
    do {
        printMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
        }
        cin.ignore();

        switch (choice) {
            case 1: doRegister(); break;
            case 2: doLogin();    break;
            case 3: cout << "\n  Goodbye!\n\n"; break;
            default: cout << "  ⚠  Invalid option.\n"; break;
        }
        cout << "\n";
    } while (choice != 3);

    return 0;
}
