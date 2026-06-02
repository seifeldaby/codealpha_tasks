/*
 * ============================================================
 *  CodeAlpha Internship — Task 1: CGPA Calculator
 *  Author : Seif Eldaby
 * ============================================================
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

// ── helpers ─────────────────────────────────────────────────
double gradeToPoints(const string& grade) {
    if (grade == "A+" || grade == "A")  return 4.0;
    if (grade == "A-")                  return 3.7;
    if (grade == "B+")                  return 3.3;
    if (grade == "B")                   return 3.0;
    if (grade == "B-")                  return 2.7;
    if (grade == "C+")                  return 2.3;
    if (grade == "C")                   return 2.0;
    if (grade == "C-")                  return 1.7;
    if (grade == "D+")                  return 1.3;
    if (grade == "D")                   return 1.0;
    if (grade == "F")                   return 0.0;
    return -1; // invalid
}

void printHeader() {
    cout << "\n";
    cout << "╔══════════════════════════════════════╗\n";
    cout << "║       CGPA CALCULATOR — CodeAlpha    ║\n";
    cout << "╚══════════════════════════════════════╝\n\n";
}

void printSummaryTable(const vector<string>& names,
                       const vector<string>& grades,
                       const vector<int>&    credits) {
    cout << "\n┌──────────────────────┬────────┬─────────┬────────────┐\n";
    cout << "│ Course               │ Grade  │ Credits │   Points   │\n";
    cout << "├──────────────────────┼────────┼─────────┼────────────┤\n";
    for (size_t i = 0; i < names.size(); ++i) {
        double pts = gradeToPoints(grades[i]) * credits[i];
        cout << "│ " << left  << setw(20) << names[i]
             << " │ " << setw(6) << grades[i]
             << " │ " << right << setw(7) << credits[i]
             << " │ " << setw(10) << fixed << setprecision(2) << pts
             << " │\n";
    }
    cout << "└──────────────────────┴────────┴─────────┴────────────┘\n";
}

// ── semester entry ───────────────────────────────────────────
double enterSemester(int semNum) {
    cout << "\n── Semester " << semNum << " ──\n";
    int n;
    cout << "  Number of courses: ";
    cin  >> n;
    cin.ignore();

    vector<string> names(n), grades(n);
    vector<int>    credits(n);

    for (int i = 0; i < n; ++i) {
        cout << "\n  Course " << (i + 1) << " name   : ";
        getline(cin, names[i]);

        string grade;
        double pts = -1;
        while (pts < 0) {
            cout << "  Grade (A+/A/A-/B+/B/B-/C+/C/C-/D+/D/F): ";
            cin  >> grade;
            // uppercase
            for (auto& c : grade) c = toupper(c);
            pts = gradeToPoints(grade);
            if (pts < 0) cout << "  ⚠  Invalid grade, try again.\n";
        }
        grades[i] = grade;

        cout << "  Credit hours     : ";
        cin  >> credits[i];
        cin.ignore();
    }

    // calculate semester GPA
    double totalPoints = 0;
    int    totalCredits = 0;
    for (int i = 0; i < n; ++i) {
        totalPoints  += gradeToPoints(grades[i]) * credits[i];
        totalCredits += credits[i];
    }
    double gpa = (totalCredits > 0) ? totalPoints / totalCredits : 0.0;

    printSummaryTable(names, grades, credits);
    cout << "\n  Total Credits : " << totalCredits << "\n";
    cout << "  Semester GPA  : " << fixed << setprecision(2) << gpa << "\n";

    return gpa;  // we'll accumulate for CGPA (weighted approach below)
    // Note: caller also needs total credits; refactored via struct in extended version.
    // For simplicity here we return GPA per semester and average them equally.
}

// ── main ─────────────────────────────────────────────────────
int main() {
    printHeader();

    int semesters;
    cout << "How many semesters do you want to enter? ";
    cin  >> semesters;

    double totalWeightedPoints = 0;
    int    grandTotalCredits   = 0;

    // We'll collect per-semester data to compute true CGPA
    for (int s = 1; s <= semesters; ++s) {
        cout << "\n── Semester " << s << " ──\n";
        int n;
        cout << "  Number of courses: ";
        cin  >> n;
        cin.ignore();

        double semPoints  = 0;
        int    semCredits = 0;
        vector<string> names(n), grades(n);
        vector<int>    credits(n);

        for (int i = 0; i < n; ++i) {
            cout << "\n  Course " << (i + 1) << " name   : ";
            getline(cin, names[i]);

            string grade;
            double pts = -1;
            while (pts < 0) {
                cout << "  Grade (A+/A/A-/B+/B/B-/C+/C/C-/D+/D/F): ";
                cin  >> grade;
                for (auto& c : grade) c = toupper(c);
                pts = gradeToPoints(grade);
                if (pts < 0) cout << "  ⚠  Invalid grade, try again.\n";
            }
            grades[i]  = grade;

            cout << "  Credit hours     : ";
            cin  >> credits[i];
            cin.ignore();

            semPoints  += gradeToPoints(grade) * credits[i];
            semCredits += credits[i];
        }

        double semGPA = (semCredits > 0) ? semPoints / semCredits : 0.0;

        printSummaryTable(names, grades, credits);
        cout << "\n  Total Credits  : " << semCredits   << "\n";
        cout << "  Semester GPA   : " << fixed << setprecision(2) << semGPA << "\n";

        totalWeightedPoints += semPoints;
        grandTotalCredits   += semCredits;
    }

    double cgpa = (grandTotalCredits > 0)
                  ? totalWeightedPoints / grandTotalCredits : 0.0;

    cout << "\n╔══════════════════════════════════════╗\n";
    cout << "║  Grand Total Credits : " << setw(4) << grandTotalCredits
         << "           ║\n";
    cout << "║  Overall CGPA        : "
         << fixed << setprecision(2) << cgpa
         << " / 4.00         ║\n";
    cout << "╚══════════════════════════════════════╝\n\n";

    return 0;
}
