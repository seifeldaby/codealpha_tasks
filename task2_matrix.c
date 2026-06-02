/*
 * ============================================================
 *  CodeAlpha Internship — Task 2: Matrix Operations
 *  Author : Seif Eldaby
 *  Date   : 2026
 *  Supports: Addition, Multiplication, Transpose
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX 10

/* ---------- type ---------- */
typedef struct {
    int data[MAX][MAX];
    int rows;
    int cols;
} Matrix;

/* ---------- prototypes ---------- */
void     printBanner(void);
void     printMenu(void);
Matrix   inputMatrix(const char *name);
void     displayMatrix(const char *label, const Matrix *m);
Matrix   addMatrices(const Matrix *a, const Matrix *b);
Matrix   multiplyMatrices(const Matrix *a, const Matrix *b);
Matrix   transposeMatrix(const Matrix *a);

/* ============================================================
 *  main
 * ============================================================ */
int main(void)
{
    int    choice;
    char   again;

    printBanner();

    do {
        printMenu();
        printf("  Enter your choice (1-4): ");
        scanf("%d", &choice);

        if (choice == 4) {
            printf("\n  Goodbye!\n\n");
            break;
        }

        Matrix A, B, result;

        switch (choice) {

            /* ── Addition ─────────────────────────────────── */
            case 1:
                printf("\n  === Matrix Addition ===\n");
                A = inputMatrix("A");
                B = inputMatrix("B");

                if (A.rows != B.rows || A.cols != B.cols) {
                    printf("\n  [!] Dimensions must match for addition!\n");
                    break;
                }
                result = addMatrices(&A, &B);
                displayMatrix("A", &A);
                displayMatrix("B", &B);
                displayMatrix("A + B", &result);
                break;

            /* ── Multiplication ───────────────────────────── */
            case 2:
                printf("\n  === Matrix Multiplication ===\n");
                A = inputMatrix("A");
                B = inputMatrix("B");

                if (A.cols != B.rows) {
                    printf("\n  [!] Columns of A must equal rows of B!\n");
                    break;
                }
                result = multiplyMatrices(&A, &B);
                displayMatrix("A", &A);
                displayMatrix("B", &B);
                displayMatrix("A x B", &result);
                break;

            /* ── Transpose ────────────────────────────────── */
            case 3:
                printf("\n  === Matrix Transpose ===\n");
                A = inputMatrix("A");
                result = transposeMatrix(&A);
                displayMatrix("Original A", &A);
                displayMatrix("Transpose of A", &result);
                break;

            default:
                printf("\n  [!] Invalid choice!\n");
        }

        printf("\n  Perform another operation? (y/n): ");
        scanf(" %c", &again);
        printf("\n");

    } while (again == 'y' || again == 'Y');

    return 0;
}

/* ============================================================
 *  I/O helpers
 * ============================================================ */

void printBanner(void)
{
    printf("\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║    CodeAlpha — Matrix Operations     ║\n");
    printf("  ║    Task 2  |  C Programming          ║\n");
    printf("  ╚══════════════════════════════════════╝\n\n");
}

void printMenu(void)
{
    printf("  ┌─────────────────────────────┐\n");
    printf("  │   Matrix Operations Menu    │\n");
    printf("  ├─────────────────────────────┤\n");
    printf("  │  1. Addition                │\n");
    printf("  │  2. Multiplication          │\n");
    printf("  │  3. Transpose               │\n");
    printf("  │  4. Exit                    │\n");
    printf("  └─────────────────────────────┘\n");
}

Matrix inputMatrix(const char *name)
{
    Matrix m;
    printf("\n  -- Enter Matrix %s --\n", name);
    printf("  Rows    (max %d): ", MAX);
    scanf("%d", &m.rows);
    printf("  Columns (max %d): ", MAX);
    scanf("%d", &m.cols);

    printf("  Enter elements row by row:\n");
    for (int i = 0; i < m.rows; i++)
        for (int j = 0; j < m.cols; j++) {
            printf("    %s[%d][%d] = ", name, i, j);
            scanf("%d", &m.data[i][j]);
        }
    return m;
}

void displayMatrix(const char *label, const Matrix *m)
{
    printf("\n  [ %s ]  (%d x %d)\n", label, m->rows, m->cols);
    printf("  ┌");
    for (int j = 0; j < m->cols; j++) printf("────────");
    printf("┐\n");

    for (int i = 0; i < m->rows; i++) {
        printf("  │");
        for (int j = 0; j < m->cols; j++)
            printf("  %5d ", m->data[i][j]);
        printf("│\n");
    }

    printf("  └");
    for (int j = 0; j < m->cols; j++) printf("────────");
    printf("┘\n");
}

/* ============================================================
 *  Matrix operations
 * ============================================================ */

Matrix addMatrices(const Matrix *a, const Matrix *b)
{
    Matrix r;
    r.rows = a->rows;
    r.cols = a->cols;
    for (int i = 0; i < r.rows; i++)
        for (int j = 0; j < r.cols; j++)
            r.data[i][j] = a->data[i][j] + b->data[i][j];
    return r;
}

Matrix multiplyMatrices(const Matrix *a, const Matrix *b)
{
    Matrix r;
    r.rows = a->rows;
    r.cols = b->cols;
    for (int i = 0; i < r.rows; i++)
        for (int j = 0; j < r.cols; j++) {
            r.data[i][j] = 0;
            for (int k = 0; k < a->cols; k++)
                r.data[i][j] += a->data[i][k] * b->data[k][j];
        }
    return r;
}

Matrix transposeMatrix(const Matrix *a)
{
    Matrix r;
    r.rows = a->cols;
    r.cols = a->rows;
    for (int i = 0; i < a->rows; i++)
        for (int j = 0; j < a->cols; j++)
            r.data[j][i] = a->data[i][j];
    return r;
}
