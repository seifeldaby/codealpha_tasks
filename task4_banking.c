/*
 * ============================================================
 *  CodeAlpha Internship — Task 4: Bank Account Management
 *  Author : Seif Eldaby
 *  Date   : 2026
 *  Features: Create Account, Deposit, Withdraw,
 *            Balance Enquiry, Transfer, Mini Statement
 *  Storage : Binary file  (accounts.dat)
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ============================================================
 *  Constants & Types
 * ============================================================ */
#define ACCOUNTS_FILE   "accounts.dat"
#define NAME_LEN        60
#define MAX_ACCOUNTS    200
#define HISTORY_LEN     10   /* last N transactions per account */

typedef struct {
    char    desc[64];
    double  amount;
    double  balanceAfter;
    char    timestamp[20];
} Transaction;

typedef struct {
    int         accNo;
    char        holderName[NAME_LEN];
    char        accountType[20];   /* "Savings" / "Current" */
    double      balance;
    int         active;
    Transaction history[HISTORY_LEN];
    int         histCount;
} Account;

/* ============================================================
 *  Prototypes
 * ============================================================ */
void    printBanner(void);
void    printMainMenu(void);
void    createAccount(void);
void    deposit(void);
void    withdraw(void);
void    balanceEnquiry(void);
void    transferFunds(void);
void    miniStatement(void);
void    listAllAccounts(void);
int     findAccount(Account arr[], int count, int accNo);
int     accNoExists(int accNo);
int     loadAll(Account arr[], int *count);
void    saveAll(Account arr[], int count);
void    addTransaction(Account *acc, const char *desc,
                       double amount, double balAfter);
void    getCurrentTime(char *buf, int size);
void    clearBuffer(void);
void    printAccountCard(const Account *a);

/* ============================================================
 *  main
 * ============================================================ */
int main(void)
{
    int  choice;
    char again;

    printBanner();

    do {
        printMainMenu();
        printf("  Choice: ");
        scanf("%d", &choice);
        clearBuffer();
        printf("\n");

        switch (choice) {
            case 1: createAccount();   break;
            case 2: deposit();         break;
            case 3: withdraw();        break;
            case 4: balanceEnquiry();  break;
            case 5: transferFunds();   break;
            case 6: miniStatement();   break;
            case 7: listAllAccounts(); break;
            case 8:
                printf("  Thank you for using CodeAlpha Bank. Goodbye!\n\n");
                return 0;
            default:
                printf("  [!] Invalid choice. Please select 1-8.\n");
        }

        printf("\n  Back to main menu? (y/n): ");
        scanf(" %c", &again);
        clearBuffer();
        printf("\n");

    } while (again == 'y' || again == 'Y');

    return 0;
}

/* ============================================================
 *  Core Operations
 * ============================================================ */

/* ── CREATE ACCOUNT ──────────────────────────────────────── */
void createAccount(void)
{
    Account arr[MAX_ACCOUNTS];
    int     count = 0;
    Account a;
    memset(&a, 0, sizeof(Account));

    loadAll(arr, &count);

    printf("  ── Open New Account ─────────────────\n");

    /* Auto-generate account number */
    a.accNo = 1001;
    for (int i = 0; i < count; i++)
        if (arr[i].accNo >= a.accNo) a.accNo = arr[i].accNo + 1;

    printf("  Assigned Account No: %d\n", a.accNo);

    printf("  Holder Name        : ");
    fgets(a.holderName, NAME_LEN, stdin);
    a.holderName[strcspn(a.holderName, "\n")] = '\0';

    printf("  Account Type\n");
    printf("    1. Savings\n    2. Current\n  Option: ");
    int t; scanf("%d", &t); clearBuffer();
    strcpy(a.accountType, (t == 2) ? "Current" : "Savings");

    printf("  Initial Deposit ($) : ");
    scanf("%lf", &a.balance);
    clearBuffer();

    if (a.balance < 0) {
        printf("  [!] Initial deposit cannot be negative.\n");
        return;
    }

    a.active    = 1;
    a.histCount = 0;
    addTransaction(&a, "Account Opened", a.balance, a.balance);

    if (count < MAX_ACCOUNTS) {
        arr[count++] = a;
        saveAll(arr, count);
        printf("\n  [✓] Account created successfully!\n");
        printAccountCard(&a);
    } else {
        printf("  [!] Bank database is full.\n");
    }
}

/* ── DEPOSIT ─────────────────────────────────────────────── */
void deposit(void)
{
    Account arr[MAX_ACCOUNTS];
    int     count = 0, idx;
    int     accNo;
    double  amount;

    loadAll(arr, &count);

    printf("  ── Deposit ──────────────────────────\n");
    printf("  Account No : ");
    scanf("%d", &accNo); clearBuffer();

    idx = findAccount(arr, count, accNo);
    if (idx < 0) { printf("  [!] Account not found.\n"); return; }

    printf("  Amount ($) : ");
    scanf("%lf", &amount); clearBuffer();

    if (amount <= 0) { printf("  [!] Amount must be positive.\n"); return; }

    arr[idx].balance += amount;
    addTransaction(&arr[idx], "Deposit", amount, arr[idx].balance);
    saveAll(arr, count);

    printf("\n  [✓] $%.2lf deposited to account %d.\n", amount, accNo);
    printf("  New Balance: $%.2lf\n", arr[idx].balance);
}

/* ── WITHDRAW ────────────────────────────────────────────── */
void withdraw(void)
{
    Account arr[MAX_ACCOUNTS];
    int     count = 0, idx;
    int     accNo;
    double  amount;

    loadAll(arr, &count);

    printf("  ── Withdraw ─────────────────────────\n");
    printf("  Account No : ");
    scanf("%d", &accNo); clearBuffer();

    idx = findAccount(arr, count, accNo);
    if (idx < 0) { printf("  [!] Account not found.\n"); return; }

    printf("  Amount ($) : ");
    scanf("%lf", &amount); clearBuffer();

    if (amount <= 0) { printf("  [!] Amount must be positive.\n"); return; }
    if (amount > arr[idx].balance) {
        printf("  [!] Insufficient funds! Balance: $%.2lf\n", arr[idx].balance);
        return;
    }

    arr[idx].balance -= amount;
    addTransaction(&arr[idx], "Withdrawal", -amount, arr[idx].balance);
    saveAll(arr, count);

    printf("\n  [✓] $%.2lf withdrawn from account %d.\n", amount, accNo);
    printf("  Remaining Balance: $%.2lf\n", arr[idx].balance);
}

/* ── BALANCE ENQUIRY ─────────────────────────────────────── */
void balanceEnquiry(void)
{
    Account arr[MAX_ACCOUNTS];
    int     count = 0, idx, accNo;

    loadAll(arr, &count);

    printf("  ── Balance Enquiry ──────────────────\n");
    printf("  Account No : ");
    scanf("%d", &accNo); clearBuffer();

    idx = findAccount(arr, count, accNo);
    if (idx < 0) { printf("  [!] Account not found.\n"); return; }

    printAccountCard(&arr[idx]);
}

/* ── TRANSFER ────────────────────────────────────────────── */
void transferFunds(void)
{
    Account arr[MAX_ACCOUNTS];
    int     count = 0, fromIdx, toIdx;
    int     fromAcc, toAcc;
    double  amount;

    loadAll(arr, &count);

    printf("  ── Transfer Funds ───────────────────\n");
    printf("  From Account No : ");
    scanf("%d", &fromAcc); clearBuffer();
    printf("  To   Account No : ");
    scanf("%d", &toAcc); clearBuffer();

    fromIdx = findAccount(arr, count, fromAcc);
    toIdx   = findAccount(arr, count, toAcc);

    if (fromIdx < 0) { printf("  [!] Source account not found.\n"); return; }
    if (toIdx   < 0) { printf("  [!] Destination account not found.\n"); return; }
    if (fromAcc == toAcc) { printf("  [!] Cannot transfer to same account.\n"); return; }

    printf("  Amount ($) : ");
    scanf("%lf", &amount); clearBuffer();

    if (amount <= 0) { printf("  [!] Amount must be positive.\n"); return; }
    if (amount > arr[fromIdx].balance) {
        printf("  [!] Insufficient funds! Balance: $%.2lf\n", arr[fromIdx].balance);
        return;
    }

    char desc[64];
    arr[fromIdx].balance -= amount;
    snprintf(desc, sizeof(desc), "Transfer to Acc#%d", toAcc);
    addTransaction(&arr[fromIdx], desc, -amount, arr[fromIdx].balance);

    arr[toIdx].balance += amount;
    snprintf(desc, sizeof(desc), "Transfer from Acc#%d", fromAcc);
    addTransaction(&arr[toIdx], desc, amount, arr[toIdx].balance);

    saveAll(arr, count);

    printf("\n  [✓] $%.2lf transferred from Acc#%d to Acc#%d.\n",
           amount, fromAcc, toAcc);
}

/* ── MINI STATEMENT ──────────────────────────────────────── */
void miniStatement(void)
{
    Account arr[MAX_ACCOUNTS];
    int     count = 0, idx, accNo;

    loadAll(arr, &count);

    printf("  ── Mini Statement ───────────────────\n");
    printf("  Account No : ");
    scanf("%d", &accNo); clearBuffer();

    idx = findAccount(arr, count, accNo);
    if (idx < 0) { printf("  [!] Account not found.\n"); return; }

    Account *a = &arr[idx];
    printf("\n  Account : %d  |  Holder: %s\n", a->accNo, a->holderName);
    printf("  %-20s  %-12s  %-12s  %s\n",
           "Date/Time", "Description", "Amount ($)", "Balance ($)");
    printf("  %-20s  %-12s  %-12s  %s\n",
           "────────────────────", "────────────",
           "────────────", "──────────");

    for (int i = 0; i < a->histCount; i++) {
        Transaction *tx = &a->history[i];
        printf("  %-20s  %-12s  %+12.2lf  %.2lf\n",
               tx->timestamp, tx->desc, tx->amount, tx->balanceAfter);
    }

    printf("\n  Current Balance: $%.2lf\n", a->balance);
}

/* ── LIST ALL ────────────────────────────────────────────── */
void listAllAccounts(void)
{
    Account arr[MAX_ACCOUNTS];
    int     count = 0, active = 0;

    loadAll(arr, &count);

    printf("  ── All Accounts ─────────────────────\n\n");
    printf("  %-8s  %-25s  %-10s  %-12s\n",
           "Acc No", "Holder Name", "Type", "Balance ($)");
    printf("  %-8s  %-25s  %-10s  %-12s\n",
           "────────", "─────────────────────────",
           "──────────", "────────────");

    for (int i = 0; i < count; i++)
        if (arr[i].active) {
            printf("  %-8d  %-25s  %-10s  %.2lf\n",
                   arr[i].accNo, arr[i].holderName,
                   arr[i].accountType, arr[i].balance);
            active++;
        }

    printf("\n  Total accounts: %d\n", active);
}

/* ============================================================
 *  File I/O
 * ============================================================ */

int loadAll(Account arr[], int *count)
{
    FILE *fp = fopen(ACCOUNTS_FILE, "rb");
    *count = 0;
    if (!fp) return 0;
    while (fread(&arr[*count], sizeof(Account), 1, fp) == 1)
        (*count)++;
    fclose(fp);
    return 1;
}

void saveAll(Account arr[], int count)
{
    FILE *fp = fopen(ACCOUNTS_FILE, "wb");
    if (!fp) { printf("  [!] File write error!\n"); return; }
    fwrite(arr, sizeof(Account), count, fp);
    fclose(fp);
}

int findAccount(Account arr[], int count, int accNo)
{
    for (int i = 0; i < count; i++)
        if (arr[i].accNo == accNo && arr[i].active) return i;
    return -1;
}

/* ============================================================
 *  Helpers
 * ============================================================ */

void addTransaction(Account *acc, const char *desc,
                    double amount, double balAfter)
{
    /* Shift history if full */
    if (acc->histCount == HISTORY_LEN) {
        for (int i = 0; i < HISTORY_LEN - 1; i++)
            acc->history[i] = acc->history[i + 1];
        acc->histCount = HISTORY_LEN - 1;
    }
    Transaction *tx = &acc->history[acc->histCount++];
    strncpy(tx->desc, desc, 63);
    tx->desc[63]    = '\0';
    tx->amount      = amount;
    tx->balanceAfter = balAfter;
    getCurrentTime(tx->timestamp, sizeof(tx->timestamp));
}

void getCurrentTime(char *buf, int size)
{
    time_t     now = time(NULL);
    struct tm *t   = localtime(&now);
    strftime(buf, size, "%Y-%m-%d %H:%M", t);
}

void printAccountCard(const Account *a)
{
    printf("\n  ┌──────────────────────────────────────┐\n");
    printf("  │  Account No  : %-22d│\n", a->accNo);
    printf("  │  Holder      : %-22s│\n", a->holderName);
    printf("  │  Type        : %-22s│\n", a->accountType);
    printf("  │  Balance     : $%-21.2lf│\n", a->balance);
    printf("  └──────────────────────────────────────┘\n");
}

void printBanner(void)
{
    printf("\n");
    printf("  ╔══════════════════════════════════════════════╗\n");
    printf("  ║   CodeAlpha — Bank Account Management       ║\n");
    printf("  ║   Task 4  |  C Programming                  ║\n");
    printf("  ╚══════════════════════════════════════════════╝\n\n");
}

void printMainMenu(void)
{
    printf("  ┌──────────────────────────────────────┐\n");
    printf("  │          CodeAlpha Bank              │\n");
    printf("  ├──────────────────────────────────────┤\n");
    printf("  │  1. Create Account                   │\n");
    printf("  │  2. Deposit                          │\n");
    printf("  │  3. Withdraw                         │\n");
    printf("  │  4. Balance Enquiry                  │\n");
    printf("  │  5. Transfer Funds                   │\n");
    printf("  │  6. Mini Statement                   │\n");
    printf("  │  7. List All Accounts                │\n");
    printf("  │  8. Exit                             │\n");
    printf("  └──────────────────────────────────────┘\n");
}

void clearBuffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
