#ifndef COMMON_FUNCTIONS
#define COMMON_FUNCTIONS

#include <stdio.h>   
#include <unistd.h>    
#include <string.h>    
#include <stdbool.h>   
#include <sys/types.h>
#include <sys/stat.h>  
#include <fcntl.h>     
#include <semaphore.h> 
#include <stdlib.h>    
#include <errno.h>     

#include "../recordtypes/account.h"
#include "../recordtypes/customer.h"
#include "../recordtypes/transaction.h"
#include "../recordtypes/employee.h"  
#include "./admin-credentials.h"
#include "./server-constants.h"


bool login_user(int connFD, struct Employee *ptrToEmployee, bool isManager);


bool login_handler(bool isAdmin, int connFD, struct Customer *ptrToCustomer);
bool get_account_details(int connFD, struct Account *customerAccount);
bool get_customer_details(int connFD, int customerID);
bool get_transaction_details(int connFD, int accountNumber);
bool add_account(int connFD);
int add_customer(int connFD, int newAccountNumber);

// =====================================================

// Function Definition =================================


// Login handler function for both Manager and Employee
bool login_user(int connFD, struct Employee *ptrToEmployee, bool isManager) {
    ssize_t readBytes, writeBytes; // Number of bytes written to / read from the socket
    char readBuffer[1000], writeBuffer[1000]; // Buffers for reading & writing to the client
    struct Employee employee; // Employee structure to hold the fetched employee details

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    // Get login message for respective user type
    if (isManager) {
        strcpy(writeBuffer, "Welcome, Manager! Please enter your login ID:"); // Manager welcome message
    } else {
        strcpy(writeBuffer, "Welcome, Employee! Please enter your login ID:"); // Employee welcome message
    }

    // Request for LOGIN ID message
    strcat(writeBuffer, "\nLogin ID: ");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1) {
        perror("Error writing WELCOME & LOGIN_ID message to the client!");
        return false;
    }

    // Read the login ID from the client
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading login ID from client!");
        return false;
    }

    bool userFound = false;

    // Validate the user based on the entered login ID
    int employeeFileFD = open("./records/employee.bank", O_RDONLY); // Assuming employee.bank contains all employee data
    if (employeeFileFD == -1) {
        perror("Error opening employee file in read mode!");
        return false;
    }

    int enteredID = atoi(readBuffer); // Convert the entered login ID to an integer
    off_t offset = lseek(employeeFileFD, enteredID * sizeof(struct Employee), SEEK_SET);
    if (offset >= 0) {
        struct flock lock = {F_RDLCK, SEEK_SET, enteredID * sizeof(struct Employee), sizeof(struct Employee), getpid()};

        // Apply read lock on the employee record
        int lockingStatus = fcntl(employeeFileFD, F_SETLKW, &lock);
        if (lockingStatus == -1) {
            perror("Error obtaining read lock on employee record!");
            close(employeeFileFD);
            return false;
        }

        // Read employee record from the file
        readBytes = read(employeeFileFD, &employee, sizeof(struct Employee));
        if (readBytes == -1) {
            perror("Error reading employee record from file!");
            close(employeeFileFD);
            return false;
        }

        // Release the lock on the employee record
        lock.l_type = F_UNLCK;
        fcntl(employeeFileFD, F_SETLK, &lock);

        // Compare the entered login ID with the employee's ID
        if (employee.id == enteredID) {
            userFound = true;
            *ptrToEmployee = employee; // Store employee info if found
        }

        close(employeeFileFD);
    } else {
        writeBytes = write(connFD, "The provided login ID does not exist.\n", 38);
    }

    if (userFound) {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, "Please enter your password: ", 28);
        if (writeBytes == -1) {
            perror("Error writing PASSWORD message to client!");
            return false;
        }

        // Read password from the client at runtime
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes <= 0) {
            perror("Error reading password from the client!");
            return false;
        }

        // Verify the password based on whether the user is a Manager or Employee
        if (strcmp(readBuffer, employee.password) == 0) {
            return true;
        }

        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, "Invalid password. Please try again.\n", 36);
    } else {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, "Invalid login. Please check your ID.\n", 37);
    }

    return false;
}


// Manager login handler
bool manager_login_handler(int connFD, struct Employee *ptrToEmployee) {
    return login_user(connFD, ptrToEmployee, true); // true indicates Manager
}

// Employee login handler
bool employee_login_handler(int connFD, struct Employee *ptrToEmployee) {
    return login_user(connFD, ptrToEmployee, false); // false indicates Employee
}





bool login_handler(bool isAdmin, int connFD, struct Customer *ptrToCustomerID)
{
    ssize_t readBytes, writeBytes;            // Number of bytes written to / read from the socket
    char readBuffer[1000], writeBuffer[1000]; // Buffer for reading from / writing to the client
    char tempBuffer[1000];
    struct Customer customer;

    int ID;

    bzero(readBuffer, sizeof(readBuffer));
    bzero(writeBuffer, sizeof(writeBuffer));

    // Get login message for respective user type
    if (isAdmin)
        strcpy(writeBuffer, ADMIN_LOGIN_WELCOME);
    else
        strcpy(writeBuffer, CUSTOMER_LOGIN_WELCOME);

    // Append the request for LOGIN ID message
    strcat(writeBuffer, "\n");
    strcat(writeBuffer, LOGIN_ID);

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing WELCOME & LOGIN_ID message to the client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading login ID from client!");
        return false;
    }

    bool userFound = false;

    if (isAdmin)
    {
        if (strcmp(readBuffer, ADMIN_LOGIN_ID) == 0)
            userFound = true;
    }
    else
    {
        bzero(tempBuffer, sizeof(tempBuffer));
        strcpy(tempBuffer, readBuffer);
        strtok(tempBuffer, "-");
        ID = atoi(strtok(NULL, "-"));

        int customerFileFD = open(CUSTOMER_FILE, O_RDONLY);
        if (customerFileFD == -1)
        {
            perror("Error opening customer file in read mode!");
            return false;
        }

        off_t offset = lseek(customerFileFD, ID * sizeof(struct Customer), SEEK_SET);
        if (offset >= 0)
        {
            struct flock lock = {F_RDLCK, SEEK_SET, ID * sizeof(struct Customer), sizeof(struct Customer), getpid()};

            int lockingStatus = fcntl(customerFileFD, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining read lock on customer record!");
                return false;
            }

            readBytes = read(customerFileFD, &customer, sizeof(struct Customer));
            if (readBytes == -1)
            {
                perror("Error reading customer record from file!");
            }

            lock.l_type = F_UNLCK;
            fcntl(customerFileFD, F_SETLK, &lock);

            if (strcmp(customer.login, readBuffer) == 0)
                userFound = true;

            close(customerFileFD);
        }
        else
        {
            writeBytes = write(connFD, CUSTOMER_LOGIN_ID_DOESNT_EXIT, strlen(CUSTOMER_LOGIN_ID_DOESNT_EXIT));
        }
    }

    if (userFound)
    {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, PASSWORD, strlen(PASSWORD));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD message to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == 1)
        {
            perror("Error reading password from the client!");
            return false;
        }

        char password[1000];
        strcpy(password, readBuffer); 

        if (isAdmin)
        {
            if (strcmp(password, ADMIN_PASSWORD) == 0)
                return true;
        }
        else
        {
            if (strcmp(password, customer.password) == 0)
            {
                *ptrToCustomerID = customer;
                return true;
            }
        }

        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, INVALID_PASSWORD, strlen(INVALID_PASSWORD));
    }
    else
    {
        bzero(writeBuffer, sizeof(writeBuffer));
        writeBytes = write(connFD, INVALID_LOGIN, strlen(INVALID_LOGIN));
    }

    return false;
}








bool get_account_details(int connFD, struct Account *customerAccount)
{
    ssize_t readBytes, writeBytes;            // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[1000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];

    int accountNumber;
    struct Account account;
    int accountFileDescriptor;

    if (customerAccount == NULL)
    {
        writeBytes = write(connFD, GET_ACCOUNT_NUMBER, strlen(GET_ACCOUNT_NUMBER));
        if (writeBytes == -1)
        {
            perror("Error writing GET_ACCOUNT_NUMBER message to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading account number response from client!");
            return false;
        }

        accountNumber = atoi(readBuffer);
    }
    else
        accountNumber = customerAccount->accountNumber;

    accountFileDescriptor = open(ACCOUNT_FILE, O_RDONLY);
    if (accountFileDescriptor == -1)
    {
        // Account record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ACCOUNT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        perror("Error opening account file in get_account_details!");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ACCOUNT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    int offset = lseek(accountFileDescriptor, accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1 && errno == EINVAL)
    {
        // Account record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ACCOUNT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        perror("Error seeking to account record in get_account_details!");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ACCOUNT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required account record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};

    int lockingStatus = fcntl(accountFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error obtaining read lock on account record!");
        return false;
    }

    readBytes = read(accountFileDescriptor, &account, sizeof(struct Account));
    if (readBytes == -1)
    {
        perror("Error reading account record from file!");
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);

    if (customerAccount != NULL)
    {
        *customerAccount = account;
        return true;
    }

    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "Account Details - \n\tAccount Number : %d\n\tAccount Status : %s", account.accountNumber, (account.active) ? "Active" : "Deactivated");
    
    if (account.active)
    {
        sprintf(tempBuffer, "\n\tAccount Balance: ₹ %ld", account.balance);
        strcat(writeBuffer, tempBuffer);
    }

    sprintf(tempBuffer, "\n\tOwner ID: %d", account.owner);
    strcat(writeBuffer, tempBuffer);

    strcat(writeBuffer, "\n^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}


bool get_customer_details(int connFD, int customerID)
{
    ssize_t readBytes, writeBytes;              // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // Buffer for reading from / writing to the socket
    struct Customer customer;
    int customerFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Customer), getpid()}; // Adjusted to lock the size of the Customer struct

    // Check if customerID is valid
    if (customerID < 0)
    {
        writeBytes = write(connFD, GET_CUSTOMER_ID, strlen(GET_CUSTOMER_ID));
        if (writeBytes == -1)
        {
            perror("Error while writing GET_CUSTOMER_ID message to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error getting customer ID from client!");
            return false;
        }

        customerID = atoi(readBuffer);
    }

    // Open customer file
    customerFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, CUSTOMER_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Seek to the correct customer record
    off_t offset = lseek(customerFileDescriptor, customerID * sizeof(struct Customer), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required customer record!");
        close(customerFileDescriptor); // Ensure file descriptor is closed
        return false;
    }

    // Lock the record
    lock.l_start = offset;
    if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1)
    {
        perror("Error while obtaining read lock on the Customer file!");
        close(customerFileDescriptor); // Ensure file descriptor is closed
        return false;
    }

    // Read the customer record
    readBytes = read(customerFileDescriptor, &customer, sizeof(struct Customer));
    if (readBytes == -1 || readBytes != sizeof(struct Customer))
    {
        perror("Error reading customer record from file!");
        lock.l_type = F_UNLCK; // Unlock before returning
        fcntl(customerFileDescriptor, F_SETLK, &lock);
        close(customerFileDescriptor); // Ensure file descriptor is closed
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);
    close(customerFileDescriptor); // Close file descriptor after use

    // Prepare and send the customer details
    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "Customer Details - \n\tID : %d\n\tName : %s\n\tGender : %c\n\tAge: %d\n\tAccount Number : %d\n\tLoginID : %s", 
            customer.id, customer.name, customer.gender, customer.age, customer.account, customer.login);

    strcat(writeBuffer, "\n\nYou'll now be redirected to the main menu...^");

    // Write the details to the client
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing customer info to client!");
        return false;
    }

    // Dummy read to wait for the client
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}






bool get_transaction_details(int connFD, int accountNumber) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000];
    char writeBuffer[10000] = {0}; // Initialize to empty
    struct Transaction transaction;
    int transactionFileDescriptor;

    if (accountNumber == -1) {
        writeBytes = write(connFD, "Enter your account number: ", 27);
        if (writeBytes == -1) {
            perror("Error writing GET_ACCOUNT_NUMBER message to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error reading account number response from client!");
            return false;
        }

        accountNumber = atoi(readBuffer);
    }

    // Open the transaction file in read mode
    transactionFileDescriptor = open(TRANSACTION_FILE, O_RDONLY);
    if (transactionFileDescriptor == -1) {
        perror("Error opening transaction file!");
        write(connFD, "No transactions found for your account.", 40);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bool transactionFound = false;
    while (read(transactionFileDescriptor, &transaction, sizeof(struct Transaction)) > 0) {
        if (transaction.accountNumber == accountNumber) {
            transactionFound = true;

            // Prepare transaction details to send back to the client
            snprintf(writeBuffer + strlen(writeBuffer), sizeof(writeBuffer) - strlen(writeBuffer),
                     "Transaction ID: %d\n"
                     "Account ID: %d\n"
                     "Date: %s" // Placeholder for actual date formatting
                     "Operation: %s\n"
                     "Sender Account ID: %d\n"
                     "Receiver Account ID: %d\n"
                     "Balance -\n"
                     "\tBefore: %ld\n"
                     "\tAfter: %ld\n"
                     "\tDifference: %ld\n\n",
                     transaction.transactionID,
                     transaction.accountNumber,
                     ctime(&transaction.transactionTime), // Convert time_t to string
                     transaction.operation ? "Deposit" : "Withdraw", // Adjust based on operation
                     transaction.accountNumber, // Sender Account (same as accountNumber)
                     transaction.receiverAccNumber,
                     transaction.oldBalance,
                     transaction.newBalance,
                     transaction.newBalance - transaction.oldBalance); // Calculate difference
        }
    }

    close(transactionFileDescriptor);

    if (!transactionFound) {
        write(connFD, "No transactions found for your account.", 40);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    } else {
        strncat(writeBuffer, "^", sizeof(writeBuffer) - strlen(writeBuffer) - 1);  // Append a termination character
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1) {
            perror("Error writing transaction details to client!");
            return false;
        }
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    }

    return true; // Indicate success
}


bool add_account(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Account newAccount, prevAccount;

    // Open the file in read/write mode and create it if it doesn't exist
    int accountFileDescriptor = open("./records/account.bank", O_CREAT | O_RDWR, S_IRWXU);
    if (accountFileDescriptor == -1)
    {
        perror("Error while opening or creating account file");
        return false;
    }

    // Check if the file is empty
    off_t fileSize = lseek(accountFileDescriptor, 0, SEEK_END);
    if (fileSize == 0)  // File is empty, no previous accounts
    {
        newAccount.accountNumber = 0;
    }
    else
    {
        int offset = lseek(accountFileDescriptor, -sizeof(struct Account), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Account record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
        int lockingStatus = fcntl(accountFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Account record!");
            return false;
        }

        readBytes = read(accountFileDescriptor, &prevAccount, sizeof(struct Account));
        if (readBytes == -1)
        {
            perror("Error while reading Account record from file!");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);

        newAccount.accountNumber = prevAccount.accountNumber + 1;
    }

    
    // Only one owner for the account
    newAccount.active = true;
    newAccount.balance = 0;

    newAccount.owner = add_customer(connFD, newAccount.accountNumber);

    accountFileDescriptor = open(ACCOUNT_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (accountFileDescriptor == -1)
    {
        perror("Error while creating / opening account file!");
        return false;
    }

    writeBytes = write(accountFileDescriptor, &newAccount, sizeof(struct Account));
    if (writeBytes == -1)
    {
        perror("Error while writing Account record to file!");
        return false;
    }

    close(accountFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "%s%d", ADMIN_ADD_ACCOUNT_NUMBER, newAccount.accountNumber);
    strcat(writeBuffer, "\nRedirecting you to the main menu ...^");
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}





int add_customer(int connFD, int newAccountNumber)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Customer newCustomer, previousCustomer;

    // Open customer file to retrieve last customer ID or create a new customer
    int customerFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
    if (customerFileDescriptor == -1 && errno == ENOENT)
    {
        // Customer file was never created
        newCustomer.id = 0;
    }
    else if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return -1;
    }
    else
    {
        // Seek to the last customer record and read it to determine the next customer ID
        int offset = lseek(customerFileDescriptor, -sizeof(struct Customer), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Customer record!");
            return -1;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};
        int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Customer record!");
            return -1;
        }

        readBytes = read(customerFileDescriptor, &previousCustomer, sizeof(struct Customer));
        if (readBytes == -1)
        {
            perror("Error while reading Customer record from file!");
            return -1;
        }

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);

        close(customerFileDescriptor);

        newCustomer.id = previousCustomer.id + 1;
    }

    // Send prompt for customer name
    sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_PRIMARY, ADMIN_ADD_CUSTOMER_NAME);
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_CUSTOMER_NAME message to client!");
        return -1;
    }

    // Read customer name from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer name response from client!");
        return -1;
    }
    strcpy(newCustomer.name, readBuffer);

    // Send prompt for customer gender
    writeBytes = write(connFD, ADMIN_ADD_CUSTOMER_GENDER, strlen(ADMIN_ADD_CUSTOMER_GENDER));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_CUSTOMER_GENDER message to client!");
        return -1;
    }

    // Read customer gender from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer gender response from client!");
        return -1;
    }

    // Validate gender input
    if (readBuffer[0] == 'M' || readBuffer[0] == 'F' || readBuffer[0] == 'O')
        newCustomer.gender = readBuffer[0];
    else
    {
        writeBytes = write(connFD, ADMIN_ADD_CUSTOMER_WRONG_GENDER, strlen(ADMIN_ADD_CUSTOMER_WRONG_GENDER));
        return -1;
    }

    // Send prompt for customer age
    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, "What is the age of the customer? ");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing age prompt to client!");
        return -1;
    }

    // Read age from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer age response from client!");
        return -1;
    }

    // Validate age input
    char *endptr;
    long customerAge = strtol(readBuffer, &endptr, 10);
    if (endptr == readBuffer || *endptr != '\0' || customerAge <= 0 || customerAge > 150)
    {
        printf("Invalid age received: %ld\n", customerAge);
        return -1;
    }
    newCustomer.age = (int)customerAge;

    // Send prompt for customer password
    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, "Enter the password: ");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing password prompt to client!");
        return -1;
    }

    // Read password from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading password response from client!");
        return -1;
    }
    strcpy(newCustomer.password, readBuffer);

    // Set additional customer details
    newCustomer.account = newAccountNumber;

    // Generate login ID based on customer name and ID
    strcpy(newCustomer.login, newCustomer.name);
    strcat(newCustomer.login, "-");
    sprintf(writeBuffer, "%d", newCustomer.id);
    strcat(newCustomer.login, writeBuffer);

    // Write customer record to file
    customerFileDescriptor = open(CUSTOMER_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (customerFileDescriptor == -1)
    {
        perror("Error while creating / opening customer file!");
        return -1;
    }

    writeBytes = write(customerFileDescriptor, &newCustomer, sizeof(newCustomer));
    if (writeBytes == -1)
    {
        perror("Error while writing Customer record to file!");
        close(customerFileDescriptor);
        return -1;
    }

    close(customerFileDescriptor);  // Close the file after writing

    // Send generated login ID and password to the client
    bzero(writeBuffer, sizeof(writeBuffer));
    sprintf(writeBuffer, "%s%s-%d\n%s%s", ADMIN_ADD_CUSTOMER_AUTOGEN_LOGIN, newCustomer.name, newCustomer.id, ADMIN_ADD_CUSTOMER_AUTOGEN_PASSWORD, newCustomer.password);
    strcat(writeBuffer, "^");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error sending customer loginID and password to the client!");
        return -1;
    }

    // Dummy read to acknowledge completion
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));  // Dummy read

    return newCustomer.id;  // Return the customer ID
}


#endif