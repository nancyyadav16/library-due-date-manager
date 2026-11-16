
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BOOKS 200
#define MAX_TRANSACTIONS 500
#define FINE_PER_DAY 5.0
#define LOAN_DAYS 14

typedef struct {
    int id;
    char title[100];
    char author[100];
    int total;
    int available;
} Book;

typedef struct {
    int transactionId;
    int bookId;
    char studentName[100];
    time_t issueDate;
    time_t dueDate;
    int returned;
} Transaction;

Book books[MAX_BOOKS];
Transaction transactions[MAX_TRANSACTIONS];

int bookCount = 0;
int transactionCount = 0;

// Read input safely
void readLine(char str[], int size) {
    if (fgets(str, size, stdin) != NULL) {
        str[strcspn(str, "\n")] = '\0';
    }
}

// Read integer safely
int readInt(const char *message) {
    char input[50];
    int value;
    char extra;

    while (1) {
        printf("%s", message);
        readLine(input, sizeof(input));

        if (sscanf(input, " %d %c", &value, &extra) == 1) {
            return value;
        }

        printf("Invalid input! Enter a valid number.\n");
    }
}

// Save books
void saveBooks() {
    FILE *fp = fopen("books.dat", "wb");

    if (fp == NULL) {
        printf("Error saving books!\n");
        return;
    }

    fwrite(&bookCount, sizeof(int), 1, fp);
    fwrite(books, sizeof(Book), bookCount, fp);

    fclose(fp);
}

// Load books
void loadBooks() {
    FILE *fp = fopen("books.dat", "rb");

    if (fp == NULL) return;

    if (fread(&bookCount, sizeof(int), 1, fp) != 1 ||
        bookCount < 0 || bookCount > MAX_BOOKS) {
        bookCount = 0;
        fclose(fp);
        return;
    }

    fread(books, sizeof(Book), bookCount, fp);
    fclose(fp);
}

// Save transactions
void saveTransactions() {
    FILE *fp = fopen("transactions.dat", "wb");

    if (fp == NULL) {
        printf("Error saving transactions!\n");
        return;
    }

    fwrite(&transactionCount, sizeof(int), 1, fp);
    fwrite(transactions, sizeof(Transaction),
           transactionCount, fp);

    fclose(fp);
}

// Load transactions
void loadTransactions() {
    FILE *fp = fopen("transactions.dat", "rb");

    if (fp == NULL) return;

    if (fread(&transactionCount, sizeof(int), 1, fp) != 1 ||
        transactionCount < 0 ||
        transactionCount > MAX_TRANSACTIONS) {
        transactionCount = 0;
        fclose(fp);
        return;
    }

    fread(transactions, sizeof(Transaction),
          transactionCount, fp);

    fclose(fp);
}

// Find book using ID
int findBook(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == id)
            return i;
    }
    return -1;
}

// Add days to a date
time_t addDays(time_t date, int days) {
    struct tm *temp = localtime(&date);
    if (temp == NULL) return date;

    struct tm result = *temp;
    result.tm_mday += days;
    result.tm_hour = 12;

    return mktime(&result);
}

// Calculate days between two calendar dates
int daysBetween(time_t start, time_t end) {
    struct tm *a = localtime(&start);
    if (a == NULL) return 0;
    struct tm first = *a;

    struct tm *b = localtime(&end);
    if (b == NULL) return 0;
    struct tm second = *b;

    first.tm_hour = 12;
    first.tm_min = 0;
    first.tm_sec = 0;

    second.tm_hour = 12;
    second.tm_min = 0;
    second.tm_sec = 0;

    time_t t1 = mktime(&first);
    time_t t2 = mktime(&second);

    return (int)(difftime(t2, t1) / 86400.0);
}

// Display date
void showDate(time_t date) {
    char buffer[30];
    struct tm *t = localtime(&date);

    if (t != NULL) {
        strftime(buffer, sizeof(buffer), "%d-%m-%Y", t);
        printf("%s", buffer);
    }
}

// Add a book
void addBook() {
    if (bookCount >= MAX_BOOKS) {
        printf("Book storage is full!\n");
        return;
    }

    Book b;

    b.id = readInt("Enter Book ID: ");

    if (b.id <= 0 || findBook(b.id) != -1) {
        printf("Invalid or duplicate Book ID!\n");
        return;
    }

    printf("Enter Book Title: ");
    readLine(b.title, sizeof(b.title));

    printf("Enter Author Name: ");
    readLine(b.author, sizeof(b.author));

    if (strlen(b.title) == 0 || strlen(b.author) == 0) {
        printf("Title and author cannot be empty!\n");
        return;
    }

    b.total = readInt("Enter Total Copies: ");

    if (b.total <= 0) {
        printf("Quantity must be positive!\n");
        return;
    }

    b.available = b.total;

    books[bookCount++] = b;
    saveBooks();

    printf("\nBook added successfully!\n");
}

// Display all books
void displayBooks() {
    if (bookCount == 0) {
        printf("\nNo books available.\n");
        return;
    }

    printf("\n========== BOOK LIST ==========\n");

    for (int i = 0; i < bookCount; i++) {
        printf("\nBook ID: %d\n", books[i].id);
        printf("Title: %s\n", books[i].title);
        printf("Author: %s\n", books[i].author);
        printf("Total Copies: %d\n", books[i].total);
        printf("Available Copies: %d\n", books[i].available);
    }
}

// Search book
void searchBook() {
    int id = readInt("Enter Book ID to search: ");
    int index = findBook(id);

    if (index == -1) {
        printf("Book not found!\n");
        return;
    }

    printf("\nBook Found!\n");
    printf("Title: %s\n", books[index].title);
    printf("Author: %s\n", books[index].author);
    printf("Available Copies: %d\n", books[index].available);
}

// Issue a book
void issueBook() {
    if (transactionCount >= MAX_TRANSACTIONS) {
        printf("Transaction storage is full!\n");
        return;
    }

    int id = readInt("Enter Book ID: ");
    int index = findBook(id);

    if (index == -1) {
        printf("Book does not exist!\n");
        return;
    }

    if (books[index].available <= 0) {
        printf("Sorry! Book is not available.\n");
        return;
    }

    Transaction t;

    t.transactionId = transactionCount + 1;
    t.bookId = id;

    printf("Enter Student Name: ");
    readLine(t.studentName, sizeof(t.studentName));

    if (strlen(t.studentName) == 0) {
        printf("Student name cannot be empty!\n");
        return;
    }

    t.issueDate = time(NULL);
    t.dueDate = addDays(t.issueDate, LOAN_DAYS);
    t.returned = 0;

    transactions[transactionCount++] = t;
    books[index].available--;

    saveBooks();
    saveTransactions();

    printf("\nBook issued successfully!\n");
    printf("Transaction ID: %d\n", t.transactionId);
    printf("Issue Date: ");
    showDate(t.issueDate);
    printf("\nDue Date: ");
    showDate(t.dueDate);
    printf("\n");
}

// Return a book
void returnBook() {
    int id = readInt("Enter Transaction ID: ");

    for (int i = 0; i < transactionCount; i++) {
        if (transactions[i].transactionId == id &&
            transactions[i].returned == 0) {

            time_t today = time(NULL);
            int overdueDays =
                daysBetween(transactions[i].dueDate, today);

            if (overdueDays < 0)
                overdueDays = 0;

            double fine = overdueDays * FINE_PER_DAY;

            transactions[i].returned = 1;

            int index = findBook(transactions[i].bookId);

            if (index != -1)
                books[index].available++;

            saveBooks();
            saveTransactions();

            printf("\nBook returned successfully!\n");
            printf("Overdue Days: %d\n", overdueDays);
            printf("Fine Amount: Rs. %.2f\n", fine);

            return;
        }
    }

    printf("Invalid transaction or book already returned!\n");
}

// Display due dates
void checkDueDates() {
    time_t today = time(NULL);
    int found = 0;

    printf("\n========== DUE DATE REPORT ==========\n");

    for (int i = 0; i < transactionCount; i++) {
        if (!transactions[i].returned) {
            int remaining =
                daysBetween(today, transactions[i].dueDate);

            printf("\nTransaction ID: %d\n",
                   transactions[i].transactionId);

            printf("Student: %s\n",
                   transactions[i].studentName);

            printf("Book ID: %d\n", transactions[i].bookId);

            printf("Due Date: ");
            showDate(transactions[i].dueDate);

            if (remaining < 0) {
                printf("\nOVERDUE by %d days\n", -remaining);
                printf("Current Fine: Rs. %.2f\n",
                       (-remaining) * FINE_PER_DAY);
            } else if (remaining == 0) {
                printf("\nDue today!\n");
            } else {
                printf("\nDays remaining: %d\n", remaining);
            }

            found = 1;
        }
    }

    if (!found)
        printf("\nNo active book issues.\n");
}

// Main menu
int main() {
    loadBooks();
    loadTransactions();

    int choice;

    printf("\n==================================\n");
    printf("     LIBRARY DUE-DATE MANAGER\n");
    printf("==================================\n");

    do {
        printf("\n1. Add Book\n");
        printf("2. Display All Books\n");
        printf("3. Search Book\n");
        printf("4. Issue Book\n");
        printf("5. Return Book\n");
        printf("6. Check Due Dates\n");
        printf("7. Exit\n");

        choice = readInt("\nEnter your choice: ");

        switch (choice) {
            case 1:
                addBook();
                break;

            case 2:
                displayBooks();
                break;

            case 3:
                searchBook();
                break;

            case 4:
                issueBook();
                break;

            case 5:
                returnBook();
                break;

            case 6:
                checkDueDates();
                break;

            case 7:
                saveBooks();
                saveTransactions();
                printf("\nThank you for using Library Manager!\n");
                break;

            default:
                printf("\nInvalid choice! Try again.\n");
        }

    } while (choice != 7);

    return 0;
}
