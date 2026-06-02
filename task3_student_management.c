/*
 * ============================================================
 *  CodeAlpha Internship — Task 3: Student Management System
 *  Author : Seif Eldaby
 *  Date   : 2026
 *  Features: Add, Delete, Update, Search, Display
 *  Storage : Binary file  (students.dat)
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 *  Constants & Types
 * ============================================================ */
#define FILE_NAME   "students.dat"
#define NAME_LEN    60
#define DEPT_LEN    40
#define MAX_STUDENTS 500

typedef struct {
    int    id;
    char   name[NAME_LEN];
    char   department[DEPT_LEN];
    int    year;
    float  gpa;
    int    active;   /* soft-delete flag: 1 = active, 0 = deleted */
} Student;

/* ============================================================
 *  Prototypes
 * ============================================================ */
void    printBanner(void);
void    printMenu(void);
void    addStudent(void);
void    deleteStudent(void);
void    updateStudent(void);
void    searchStudent(void);
void    displayAll(void);
int     idExists(int id);
int     loadAll(Student arr[], int *count);
void    saveAll(Student arr[], int count);
void    printStudentRow(const Student *s);
void    printTableHeader(void);
void    printTableFooter(void);
void    clearBuffer(void);

/* ============================================================
 *  main
 * ============================================================ */
int main(void)
{
    int choice;
    char again;

    printBanner();

    do {
        printMenu();
        printf("  Choice: ");
        scanf("%d", &choice);
        clearBuffer();
        printf("\n");

        switch (choice) {
            case 1: addStudent();    break;
            case 2: deleteStudent(); break;
            case 3: updateStudent(); break;
            case 4: searchStudent(); break;
            case 5: displayAll();    break;
            case 6:
                printf("  Goodbye! Data saved to '%s'.\n\n", FILE_NAME);
                return 0;
            default:
                printf("  [!] Invalid choice.\n");
        }

        printf("\n  Back to main menu? (y/n): ");
        scanf(" %c", &again);
        clearBuffer();
        printf("\n");

    } while (again == 'y' || again == 'Y');

    return 0;
}

/* ============================================================
 *  CRUD Operations
 * ============================================================ */

/* ── ADD ─────────────────────────────────────────────────── */
void addStudent(void)
{
    Student arr[MAX_STUDENTS];
    int     count = 0;
    Student s;

    loadAll(arr, &count);

    printf("  ── Add New Student ──────────────────\n");
    printf("  Student ID   : ");
    scanf("%d", &s.id);
    clearBuffer();

    if (idExists(s.id)) {
        printf("  [!] ID %d already exists!\n", s.id);
        return;
    }

    printf("  Full Name    : ");
    fgets(s.name, NAME_LEN, stdin);
    s.name[strcspn(s.name, "\n")] = '\0';

    printf("  Department   : ");
    fgets(s.department, DEPT_LEN, stdin);
    s.department[strcspn(s.department, "\n")] = '\0';

    printf("  Year (1-5)   : ");
    scanf("%d", &s.year);

    printf("  GPA (0-4)    : ");
    scanf("%f", &s.gpa);
    clearBuffer();

    s.active = 1;

    if (count < MAX_STUDENTS) {
        arr[count++] = s;
        saveAll(arr, count);
        printf("\n  [✓] Student '%s' (ID: %d) added successfully!\n", s.name, s.id);
    } else {
        printf("  [!] Database full.\n");
    }
}

/* ── DELETE ──────────────────────────────────────────────── */
void deleteStudent(void)
{
    Student arr[MAX_STUDENTS];
    int     count = 0;
    int     id, found = 0;

    loadAll(arr, &count);

    printf("  ── Delete Student ───────────────────\n");
    printf("  Enter Student ID to delete: ");
    scanf("%d", &id);
    clearBuffer();

    for (int i = 0; i < count; i++) {
        if (arr[i].id == id && arr[i].active) {
            arr[i].active = 0;   /* soft delete */
            found = 1;
            printf("  [✓] Student '%s' (ID: %d) deleted.\n", arr[i].name, id);
            break;
        }
    }

    if (!found)
        printf("  [!] Student with ID %d not found.\n", id);
    else
        saveAll(arr, count);
}

/* ── UPDATE ──────────────────────────────────────────────── */
void updateStudent(void)
{
    Student arr[MAX_STUDENTS];
    int     count = 0;
    int     id, found = 0;

    loadAll(arr, &count);

    printf("  ── Update Student ───────────────────\n");
    printf("  Enter Student ID to update: ");
    scanf("%d", &id);
    clearBuffer();

    for (int i = 0; i < count; i++) {
        if (arr[i].id == id && arr[i].active) {
            found = 1;
            printf("  Current record:\n");
            printTableHeader();
            printStudentRow(&arr[i]);
            printTableFooter();

            printf("\n  Enter new details (press Enter to keep current):\n");

            char buf[NAME_LEN];

            printf("  Full Name    [%s]: ", arr[i].name);
            fgets(buf, NAME_LEN, stdin);
            buf[strcspn(buf, "\n")] = '\0';
            if (strlen(buf) > 0) strcpy(arr[i].name, buf);

            printf("  Department   [%s]: ", arr[i].department);
            fgets(buf, DEPT_LEN, stdin);
            buf[strcspn(buf, "\n")] = '\0';
            if (strlen(buf) > 0) strcpy(arr[i].department, buf);

            printf("  Year (1-5)   [%d]: ", arr[i].year);
            fgets(buf, 10, stdin);
            if (buf[0] != '\n' && strlen(buf) > 1)
                arr[i].year = atoi(buf);

            printf("  GPA (0-4)    [%.2f]: ", arr[i].gpa);
            fgets(buf, 10, stdin);
            if (buf[0] != '\n' && strlen(buf) > 1)
                arr[i].gpa = (float)atof(buf);

            saveAll(arr, count);
            printf("\n  [✓] Student record updated.\n");
            break;
        }
    }

    if (!found)
        printf("  [!] Student with ID %d not found.\n", id);
}

/* ── SEARCH ──────────────────────────────────────────────── */
void searchStudent(void)
{
    Student arr[MAX_STUDENTS];
    int     count = 0;
    int     found = 0;

    loadAll(arr, &count);

    printf("  ── Search Student ───────────────────\n");
    printf("  1. Search by ID\n");
    printf("  2. Search by Name\n");
    printf("  Option: ");
    int opt;
    scanf("%d", &opt);
    clearBuffer();

    printTableHeader();

    if (opt == 1) {
        int id;
        printf("  Enter ID: ");
        scanf("%d", &id);
        clearBuffer();
        for (int i = 0; i < count; i++)
            if (arr[i].id == id && arr[i].active) {
                printStudentRow(&arr[i]);
                found = 1;
            }
    } else if (opt == 2) {
        char keyword[NAME_LEN];
        printf("  Enter name (or part): ");
        fgets(keyword, NAME_LEN, stdin);
        keyword[strcspn(keyword, "\n")] = '\0';

        for (int i = 0; i < count; i++)
            if (arr[i].active && strstr(arr[i].name, keyword)) {
                printStudentRow(&arr[i]);
                found = 1;
            }
    } else {
        printf("  [!] Invalid option.\n");
    }

    printTableFooter();

    if (!found)
        printf("  [!] No matching records found.\n");
}

/* ── DISPLAY ALL ─────────────────────────────────────────── */
void displayAll(void)
{
    Student arr[MAX_STUDENTS];
    int     count = 0, active = 0;

    loadAll(arr, &count);

    printf("  ── All Students ─────────────────────\n");
    printTableHeader();
    for (int i = 0; i < count; i++)
        if (arr[i].active) {
            printStudentRow(&arr[i]);
            active++;
        }
    printTableFooter();

    if (active == 0)
        printf("  No records found.\n");
    else
        printf("  Total active records: %d\n", active);
}

/* ============================================================
 *  File I/O
 * ============================================================ */

int loadAll(Student arr[], int *count)
{
    FILE *fp = fopen(FILE_NAME, "rb");
    *count = 0;
    if (!fp) return 0;

    while (fread(&arr[*count], sizeof(Student), 1, fp) == 1)
        (*count)++;

    fclose(fp);
    return 1;
}

void saveAll(Student arr[], int count)
{
    FILE *fp = fopen(FILE_NAME, "wb");
    if (!fp) { printf("  [!] Cannot open file for writing!\n"); return; }
    fwrite(arr, sizeof(Student), count, fp);
    fclose(fp);
}

int idExists(int id)
{
    Student arr[MAX_STUDENTS];
    int count = 0;
    loadAll(arr, &count);
    for (int i = 0; i < count; i++)
        if (arr[i].id == id && arr[i].active) return 1;
    return 0;
}

/* ============================================================
 *  Display helpers
 * ============================================================ */

void printBanner(void)
{
    printf("\n");
    printf("  ╔══════════════════════════════════════════════╗\n");
    printf("  ║   CodeAlpha — Student Management System     ║\n");
    printf("  ║   Task 3  |  C Programming                  ║\n");
    printf("  ╚══════════════════════════════════════════════╝\n\n");
}

void printMenu(void)
{
    printf("  ┌──────────────────────────────────────┐\n");
    printf("  │          Main Menu                   │\n");
    printf("  ├──────────────────────────────────────┤\n");
    printf("  │  1. Add    Student                   │\n");
    printf("  │  2. Delete Student                   │\n");
    printf("  │  3. Update Student                   │\n");
    printf("  │  4. Search Student                   │\n");
    printf("  │  5. Display All Students             │\n");
    printf("  │  6. Exit                             │\n");
    printf("  └──────────────────────────────────────┘\n");
}

void printTableHeader(void)
{
    printf("\n");
    printf("  %-6s  %-25s  %-18s  %-4s  %-5s\n",
           "ID", "Name", "Department", "Year", "GPA");
    printf("  %-6s  %-25s  %-18s  %-4s  %-5s\n",
           "──────", "─────────────────────────",
           "──────────────────", "────", "─────");
}

void printStudentRow(const Student *s)
{
    printf("  %-6d  %-25s  %-18s  %-4d  %.2f\n",
           s->id, s->name, s->department, s->year, s->gpa);
}

void printTableFooter(void)
{
    printf("\n");
}

void clearBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
