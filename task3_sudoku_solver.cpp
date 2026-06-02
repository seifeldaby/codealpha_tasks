/*
 * ============================================================
 *  CodeAlpha Internship — Task 3: Sudoku Solver
 *  Author : Seif Eldaby
 *
 *  Algorithm : Backtracking with constraint propagation hint
 *  Features  :
 *    • Pretty-print grid with box borders
 *    • User can enter a custom puzzle or use a built-in demo
 *    • Detects unsolvable puzzles
 * ============================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

// ── grid type ─────────────────────────────────────────────────
const int N = 9;
typedef int Grid[N][N];

// ── pretty printer ────────────────────────────────────────────
void printGrid(const Grid& g) {
    cout << "\n  ┌───────┬───────┬───────┐\n";
    for (int r = 0; r < N; ++r) {
        if (r == 3 || r == 6)
            cout << "  ├───────┼───────┼───────┤\n";
        cout << "  │";
        for (int c = 0; c < N; ++c) {
            if (c == 3 || c == 6) cout << " │";
            cout << " " << (g[r][c] == 0 ? '.' : (char)('0' + g[r][c]));
        }
        cout << " │\n";
    }
    cout << "  └───────┴───────┴───────┘\n";
}

// ── constraint checks ─────────────────────────────────────────
bool isSafe(const Grid& g, int row, int col, int num) {
    // row
    for (int c = 0; c < N; ++c)
        if (g[row][c] == num) return false;
    // column
    for (int r = 0; r < N; ++r)
        if (g[r][col] == num) return false;
    // 3×3 box
    int boxRow = (row / 3) * 3;
    int boxCol = (col / 3) * 3;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (g[boxRow + r][boxCol + c] == num) return false;
    return true;
}

// ── find next empty cell (returns false if none) ──────────────
bool findEmpty(const Grid& g, int& row, int& col) {
    for (row = 0; row < N; ++row)
        for (col = 0; col < N; ++col)
            if (g[row][col] == 0) return true;
    return false;
}

// ── backtracking solver ───────────────────────────────────────
bool solve(Grid& g) {
    int row, col;
    if (!findEmpty(g, row, col)) return true;   // solved!

    for (int num = 1; num <= 9; ++num) {
        if (isSafe(g, row, col, num)) {
            g[row][col] = num;
            if (solve(g)) return true;
            g[row][col] = 0;   // backtrack
        }
    }
    return false;   // trigger backtrack in caller
}

// ── copy grid ─────────────────────────────────────────────────
void copyGrid(const Grid& src, Grid& dst) {
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            dst[r][c] = src[r][c];
}

// ── input helpers ─────────────────────────────────────────────
void loadDemo(Grid& g) {
    // Classic hard puzzle (0 = empty)
    int demo[N][N] = {
        {5,3,0, 0,7,0, 0,0,0},
        {6,0,0, 1,9,5, 0,0,0},
        {0,9,8, 0,0,0, 0,6,0},

        {8,0,0, 0,6,0, 0,0,3},
        {4,0,0, 8,0,3, 0,0,1},
        {7,0,0, 0,2,0, 0,0,6},

        {0,6,0, 0,0,0, 2,8,0},
        {0,0,0, 4,1,9, 0,0,5},
        {0,0,0, 0,8,0, 0,7,9}
    };
    for (int r = 0; r < N; ++r)
        for (int c = 0; c < N; ++c)
            g[r][c] = demo[r][c];
}

bool readUserGrid(Grid& g) {
    cout << "\n  Enter the puzzle row by row.\n";
    cout << "  Use 0 for empty cells. Digits only, 9 per row (spaces optional).\n\n";

    for (int r = 0; r < N; ++r) {
        cout << "  Row " << (r + 1) << ": ";
        string line;
        // skip blank lines
        while (line.empty()) {
            getline(cin, line);
        }
        // strip spaces
        string digits;
        for (char ch : line)
            if (isdigit(ch)) digits += ch;

        if (digits.size() != 9) {
            cout << "  ✗ Expected 9 digits, got " << digits.size() << ". Try again.\n";
            --r;
            continue;
        }
        for (int c = 0; c < N; ++c)
            g[r][c] = digits[c] - '0';
    }
    return true;
}

// ── main ─────────────────────────────────────────────────────
int main() {
    cout << "\n╔══════════════════════════════════════╗\n";
    cout << "║      SUDOKU SOLVER — CodeAlpha       ║\n";
    cout << "╚══════════════════════════════════════╝\n";

    cout << "\n  [1] Solve built-in demo puzzle\n";
    cout << "  [2] Enter your own puzzle\n";
    cout << "\n  Choice: ";

    int choice;
    cin >> choice;
    cin.ignore();

    Grid puzzle, original;

    if (choice == 2) {
        readUserGrid(puzzle);
    } else {
        loadDemo(puzzle);
    }

    copyGrid(puzzle, original);

    cout << "\n  Puzzle to solve:";
    printGrid(original);

    cout << "\n  Solving...\n";

    if (solve(puzzle)) {
        cout << "  ✓ Solution found!\n";
        printGrid(puzzle);
    } else {
        cout << "  ✗ No solution exists for this puzzle.\n";
    }

    cout << "\n";
    return 0;
}
