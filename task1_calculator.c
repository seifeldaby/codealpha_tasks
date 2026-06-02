/*
 * ============================================================
 *  CodeAlpha Internship — Task 1: Basic Calculator
 *  Author : Seif Eldaby
 *  Date   : 2026
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* ---------- prototypes ---------- */
void printBanner(void);
void printMenu(void);
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);

/* ============================================================
 *  main
 * ============================================================ */
int main(void)
{
    int    choice;
    double num1, num2, result;
    char   again;

    printBanner();

    do {
        printMenu();

        printf("  Enter your choice (1-5): ");
        if (scanf("%d", &choice) != 1) {
            /* flush invalid input */
            while (getchar() != '\n');
            printf("\n  [!] Invalid input. Please enter a number.\n\n");
            continue;
        }

        if (choice == 5) {
            printf("\n  Goodbye! Thanks for using CodeAlpha Calculator.\n\n");
            break;
        }

        if (choice < 1 || choice > 4) {
            printf("\n  [!] Invalid choice. Please select 1-5.\n\n");
            continue;
        }

        printf("\n  Enter first  number : ");
        scanf("%lf", &num1);
        printf("  Enter second number : ");
        scanf("%lf", &num2);

        switch (choice) {
            case 1:
                result = add(num1, num2);
                printf("\n  Result: %.2lf + %.2lf = %.4lf\n", num1, num2, result);
                break;

            case 2:
                result = subtract(num1, num2);
                printf("\n  Result: %.2lf - %.2lf = %.4lf\n", num1, num2, result);
                break;

            case 3:
                result = multiply(num1, num2);
                printf("\n  Result: %.2lf * %.2lf = %.4lf\n", num1, num2, result);
                break;

            case 4:
                if (num2 == 0) {
                    printf("\n  [!] Error: Division by zero is undefined!\n");
                } else {
                    result = divide(num1, num2);
                    printf("\n  Result: %.2lf / %.2lf = %.4lf\n", num1, num2, result);
                }
                break;
        }

        printf("\n  Do another calculation? (y/n): ");
        scanf(" %c", &again);
        printf("\n");

    } while (again == 'y' || again == 'Y');

    return 0;
}

/* ============================================================
 *  Function implementations
 * ============================================================ */

void printBanner(void)
{
    printf("\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║    CodeAlpha — Basic Calculator      ║\n");
    printf("  ║    Task 1  |  C Programming          ║\n");
    printf("  ╚══════════════════════════════════════╝\n\n");
}

void printMenu(void)
{
    printf("  ┌─────────────────────────────┐\n");
    printf("  │   Select an Operation       │\n");
    printf("  ├─────────────────────────────┤\n");
    printf("  │  1. Addition        (+)     │\n");
    printf("  │  2. Subtraction     (-)     │\n");
    printf("  │  3. Multiplication  (*)     │\n");
    printf("  │  4. Division        (/)     │\n");
    printf("  │  5. Exit                    │\n");
    printf("  └─────────────────────────────┘\n");
}

double add(double a, double b)      { return a + b; }
double subtract(double a, double b) { return a - b; }
double multiply(double a, double b) { return a * b; }
double divide(double a, double b)   { return a / b; }
