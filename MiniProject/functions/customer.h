#ifndef CUSTOMER_FUNCTIONS
#define CUSTOMER_FUNCTIONS

#include <sys/ipc.h>
#include <sys/sem.h>

#include "./server-constants.h"
#include "../recordtypes/loan.h"
#include "../recordtypes/feedback.h"
#include "../recordtypes/transaction.h"

struct Customer loggedInCustomer;



// Function Prototypes =================================

bool customer_operation_handler(int connFD);
bool deposit(int connFD);
bool withdraw(int connFD);
bool get_balance(int connFD);
bool transfer_funds(int connFD);
bool update_account_in_file(struct Account *account);
bool change_password(int connFD);
bool write_feedback(int connFD);                                                         // major change, full file locking, to avoid inconsistency.
// int write_transaction_to_file(int accountNumber, long int oldBalance, long int newBalance, int operation);
int write_transaction_to_file(int accountNumber, long beforeBalance, long afterBalance, int operation, int receiverAccountNumber); 
bool apply_loan(int connFD);

// =====================================================

// Function Definition =================================




bool customer_operation_handler(int connFD) {
    ssize_t writeBytes, readBytes;
    char readBuffer[1000], writeBuffer[1000];

    // Loop until a successful login or exit
    while (1) {
        // Prompt for login ID
        // if (write(connFD, "🔑 Enter your login ID: ", strlen("🔑 Enter your login ID: ")) == -1) {             // I have commented, repeatitive.
        //     perror("Error while writing prompt for login ID to client");
        //     return false;
        // }

        // bzero(readBuffer, sizeof(readBuffer));
        // readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
        // if (readBytes <= 0) {
        //     perror("Error while reading login ID from client");
        //     return false;
        // }
        // readBuffer[readBytes] = '\0'; // Null-terminate the string

        // Validate the login ID
        if (login_handler(false, connFD, &loggedInCustomer)) {
            if (write(connFD, CUSTOMER_LOGIN_SUCCESS, strlen(CUSTOMER_LOGIN_SUCCESS)) == -1) {
                perror("Error while writing login success message to client");
                return false;
            }
            bzero(readBuffer, sizeof(readBuffer));
            read(connFD, readBuffer, sizeof(readBuffer)); 
            break; // Exit the loop for successful login
        } else {
            if (write(connFD, CUSTOMER_LOGIN_ID_DOESNT_EXIST, strlen(CUSTOMER_LOGIN_ID_DOESNT_EXIST)) == -1) {
                perror("Error while sending login failure message to client");
            }
            bzero(readBuffer, sizeof(readBuffer));
            read(connFD, readBuffer, sizeof(readBuffer)); 
        }
    }

    // Customer menu loop
    while (1) {
        // Display customer menu
        snprintf(writeBuffer, sizeof(writeBuffer), "\n%s", CUSTOMER_MENU);
        if (write(connFD, writeBuffer, strlen(writeBuffer)) == -1) {
            perror("Error while writing CUSTOMER_MENU to client!");
            return false;
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
        if (readBytes <= 0) {
            perror("Error while reading client's choice for CUSTOMER_MENU");
            return false;
        }
        readBuffer[readBytes] = '\0'; // Null-terminate the string

        int choice = atoi(readBuffer);
        switch (choice) {
            case 1:
                get_customer_details(connFD, loggedInCustomer.id);
                break;
            case 2:
                deposit(connFD);
                break;
            case 3:
                withdraw(connFD);
                break;
            case 4:
                get_balance(connFD);
                break;
            case 5:
                get_transaction_details(connFD, loggedInCustomer.account);
                break;
            case 6:
                change_password(connFD);
                break;
            case 7:
                transfer_funds(connFD);
                break;
            case 8:
                apply_loan(connFD);
                break;
            case 9:
                write_feedback(connFD);
                break;
            case 10: // Logout
                logout_handler(connFD, loggedInCustomer.id);
                return true; // Return to the initial prompt
            case 11: // Exit
                logout_handler(connFD, loggedInCustomer.id);
                write(connFD, "Exiting the application. Goodbye!$", strlen("Exiting the application. Goodbye!$"));
                close(connFD);
                return false;
            default:
                write(connFD, "Invalid choice! Please try again.\n", 36);
        }
    }

    return true;
}





bool deposit(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;
    long int depositAmount = 0;

    // Retrieve account details
    if (!get_account_details(connFD, &account)) {
        return false;
    }

    // Check if the account is active
    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Prompt for the deposit amount
    if (write(connFD, DEPOSIT_AMOUNT, strlen(DEPOSIT_AMOUNT)) == -1) {
        perror("Error writing DEPOSIT_AMOUNT prompt to client!");
        return false;
    }

    // Read the deposit amount from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading deposit amount from client!");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    depositAmount = atol(readBuffer);
    if (depositAmount <= 0) {
        write(connFD, DEPOSIT_AMOUNT_INVALID, strlen(DEPOSIT_AMOUNT_INVALID));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Log the transaction before updating the account balance
    int newTransactionID = write_transaction_to_file(
        account.accountNumber, account.balance, account.balance + depositAmount, 0, account.accountNumber);
    if (newTransactionID < 0) {
        return false; // Handle transaction logging failure
    }

    // Update the account balance
    account.balance += depositAmount;

    // Open the account file for updating
    int accountFileDescriptor = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating!");
        return false;
    }

    // Seek to the account record
    off_t offset = lseek(accountFileDescriptor, account.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file!");
        close(accountFileDescriptor);
        return false;
    }

    // Lock the account record
    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on account record!");
        close(accountFileDescriptor);
        return false;
    }

    // Write the updated account balance to the file
    if (write(accountFileDescriptor, &account, sizeof(struct Account)) == -1) {
        perror("Error writing updated account balance to file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        return false;
    }

    // Unlock the account record and close the file
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);

    // Notify the client of success
    write(connFD, DEPOSIT_AMOUNT_SUCCESS, strlen(DEPOSIT_AMOUNT_SUCCESS));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    // Display the updated balance
    get_balance(connFD);

    return true;
}




bool withdraw(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;
    long int withdrawAmount = 0;

    // Retrieve account details
    if (!get_account_details(connFD, &account)) {
        perror("Failed to get account details");
        return false;
    }

    // Check if the account is active
    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Prompt for the withdrawal amount
    if (write(connFD, WITHDRAW_AMOUNT, strlen(WITHDRAW_AMOUNT)) == -1) {
        perror("Error writing WITHDRAW_AMOUNT message to client!");
        return false;
    }

    // Read the withdrawal amount from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading withdraw amount from client!");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    withdrawAmount = atol(readBuffer);
    printf("Withdraw Amount Requested: %ld\n", withdrawAmount); // Debug log

    // Validate withdrawal amount
    if (withdrawAmount <= 0 || account.balance < withdrawAmount) {
        write(connFD, WITHDRAW_AMOUNT_INVALID, strlen(WITHDRAW_AMOUNT_INVALID));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Log the transaction before updating the account balance
    int newTransactionID = write_transaction_to_file(
        account.accountNumber, account.balance, account.balance - withdrawAmount, 1, account.accountNumber);
    if (newTransactionID < 0) {
        return false; // Handle transaction logging failure
    }

    // Update the account balance
    account.balance -= withdrawAmount;

    // Open the account file for updating
    int accountFileDescriptor = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating!");
        return false;
    }

    // Seek to the account record
    off_t offset = lseek(accountFileDescriptor, account.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file!");
        close(accountFileDescriptor);
        return false;
    }

    // Lock the account record for writing
    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on account record!");
        close(accountFileDescriptor);
        return false;
    }

    // Write the updated account balance to the file
    if (write(accountFileDescriptor, &account, sizeof(struct Account)) == -1) {
        perror("Error writing updated account balance to file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        return false;
    }

    // Unlock the account record and close the file
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);

    // Notify the client of success
    write(connFD, WITHDRAW_AMOUNT_SUCCESS, strlen(WITHDRAW_AMOUNT_SUCCESS));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    // Display the updated balance
    get_balance(connFD);

    return true;
}



bool get_balance(int connFD) {
    char buffer[1000];
    struct Account account;
    account.accountNumber = loggedInCustomer.account;

    // Retrieve account details
    if (!get_account_details(connFD, &account)) {
        perror("Error retrieving account details");
        return false;
    }

    bzero(buffer, sizeof(buffer));

    // Check if the account is active
    if (account.active) {
        snprintf(buffer, sizeof(buffer), "You have ₹ %ld imaginary money in our bank!\n^", account.balance);
        if (write(connFD, buffer, strlen(buffer)) == -1) {
            perror("Error sending balance information to client");
            return false;
        }
    } else {
        if (write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED)) == -1) {
            perror("Error sending account deactivated message to client");
            return false;
        }
    }

    bzero(buffer, sizeof(buffer));
    read(connFD, buffer, sizeof(buffer)             // Dummy read for acknowledgment

    return true;
}




bool transfer_funds(int connFD) {
    char readBuffer[1000];
    char writeBuffer[1000];
    ssize_t readBytes, writeBytes;
    struct Account senderAccount, receiverAccount;
    senderAccount.accountNumber = loggedInCustomer.account;
    long transferAmount = 0;

    // Retrieve sender's account details
    if (!get_account_details(connFD, &senderAccount)) {
        perror("Failed to retrieve sender's account details");
        return false;
    }

    // Check if sender's account is active
    if (!senderAccount.active) {
        sprintf(writeBuffer, "Account deactivated^");  
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer));           // Dummy read
        return false;
    }

    // Prompt for receiver's account ID
    sprintf(writeBuffer, "Enter the account ID of the recipient: ");
    write(connFD, writeBuffer, strlen(writeBuffer));

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading recipient account ID!");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    int receiverAccountNumber = atoi(readBuffer);

    // Check if the receiver is the same as the sender
    if (receiverAccountNumber == senderAccount.accountNumber) {
        sprintf(writeBuffer, "Cannot transfer funds to the same account!^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    // Retrieve receiver's account details
    receiverAccount.accountNumber = receiverAccountNumber;
    if (!get_account_details(receiverAccountNumber, &receiverAccount)) {
        sprintf(writeBuffer, "Recipient account not found!^"); 
        write(connFD, writeBuffer, strlen(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer));               // Dummy read
        return false;
    }

    // Check if receiver's account is active
    if (!receiverAccount.active) {
        sprintf(writeBuffer, "Recipient account is deactivated!\n^"); 
        write(connFD, writeBuffer, strlen(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer));               // Dummy read
        return false;
    }

    // Prompt for transfer amount
    sprintf(writeBuffer, "Enter the amount to transfer: ");
    write(connFD, writeBuffer, strlen(writeBuffer));

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading transfer amount!");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    transferAmount = atol(readBuffer);

    // Validate transfer amount
    if (transferAmount <= 0 || senderAccount.balance < transferAmount) {
        sprintf(writeBuffer, "Insufficient funds or invalid amount!^");
        write(connFD, writeBuffer, strlen(writeBuffer));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer));                       / Dummy read
        return false;
    }

    // Log the transaction for sender
    int newSenderTransactionID = write_transaction_to_file(
        senderAccount.accountNumber, senderAccount.balance, senderAccount.balance - transferAmount, 2, receiverAccount.accountNumber);
    if (newSenderTransactionID < 0) {
        return false; // Handle transaction logging failure
    }

    // Deduct from sender's account
    senderAccount.balance -= transferAmount;

    // Update sender's account in file
    if (!update_account_in_file(&senderAccount)) {
        return false;
    }

    // Log the transaction for receiver
    int newReceiverTransactionID = write_transaction_to_file(
        receiverAccount.accountNumber, receiverAccount.balance, receiverAccount.balance + transferAmount, 2, senderAccount.accountNumber);
    if (newReceiverTransactionID < 0) {
        return false; // Handle transaction logging failure
    }

    // Add to receiver's account
    receiverAccount.balance += transferAmount;

    // Update receiver's account in file
    if (!update_account_in_file(&receiverAccount)) {
        return false;
    }

    // Notify sender of success
    sprintf(writeBuffer, "Transfer successful!^"); 
    write(connFD, writeBuffer, strlen(writeBuffer));
    bzero(readBuffer, sizeof(readBuffer));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}




bool change_password(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], newPassword[1000];

    // Prompt for old password
    if (write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS)) == -1) {
        perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        return false;
    }

    // Read old password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading old password response from client");
        return false;
    }
    readBuffer[readBytes] = '\0';       // Null-terminate the input

    // Validate old password
    if (strcmp(readBuffer, loggedInCustomer.password) != 0) {
        write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        bzero(readBuffer, sizeof(readBuffer));
        read(connFD, readBuffer, sizeof(readBuffer));                       // Dummy read
        return false;
    }

    // Prompt for new password
    if (write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS)) == -1) {
        perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
        return false;
    }

    // Read new password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading new password response from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    strcpy(newPassword, readBuffer);

    // Prompt for re-entering new password
    if (write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE)) == -1) {
        perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
        return false;
    }

    // Read re-entered password
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading new password re-enter response from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Validate new passwords match
    if (strcmp(readBuffer, newPassword) != 0) {
        write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
        read(connFD, readBuffer, sizeof(readBuffer));                                   // Dummy read
        return false;
    }

    // Update password in memory
    strcpy(loggedInCustomer.password, newPassword);

    // Open customer file for updating
    int customerFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
    if (customerFileDescriptor == -1) {
        perror("Error opening customer file!");
        return false;
    }

    // Seek to the customer's record
    off_t offset = lseek(customerFileDescriptor, loggedInCustomer.id * sizeof(struct Customer), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking to the customer record!");
        close(customerFileDescriptor);
        return false;
    }

    // Lock the customer record for writing
    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};
    if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on customer record!");
        close(customerFileDescriptor);
        return false;
    }

    // Write the updated customer record
    if (write(customerFileDescriptor, &loggedInCustomer, sizeof(struct Customer)) == -1) {
        perror("Error storing updated customer password into customer record!");
        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);
        close(customerFileDescriptor);
        return false;
    }

    // Unlock the record and close the file
    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);
    close(customerFileDescriptor);

    // Notify the client of success
    if (write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS)) == -1) {
        perror("Error writing PASSWORD_CHANGE_SUCCESS message to client!");
        unlock_critical_section(&semOp);
        return false;
    }

    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}




bool update_account_in_file(struct Account *account) {
    int accountFileDescriptor;
    off_t offset;
    struct flock lock;

    accountFileDescriptor = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating!");
        return false;
    }

    // Calculate offset of the record in file
    offset = account->accountNumber * sizeof(struct Account);

    // Seek to the record
    if (lseek(accountFileDescriptor, offset, SEEK_SET) == -1) {
        perror("Error seeking in account file!");
        close(accountFileDescriptor);
        return false;
    }

    // Set up a write lock
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = offset;
    lock.l_len = sizeof(struct Account);
    lock.l_pid = getpid();

    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on account record!");
        close(accountFileDescriptor);
        return false;
    }

    // Write the updated account record
    if (write(accountFileDescriptor, account, sizeof(struct Account)) == -1) {
        perror("Error writing updated account record to file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);

    close(accountFileDescriptor);
    return true;
}




bool apply_loan(int connFD) {
    char readBuffer[500];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;

    // Retrieve account details
    if (!get_account_details(connFD, &account)) {
        return false;
    }

    // Check if the account is active
    if (!account.active) {
        write(connFD, "Account is deactivated.\n^", strlen("Account is deactivated.\n^"));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read for acknowledgment
        return false;
    }

    int loanAmount;

    // Prompt the client to enter the loan amount
    const char *promptMessage = "Please enter the loan amount you wish to apply for: ";
    if (write(connFD, promptMessage, strlen(promptMessage)) == -1) {
        perror("Error sending loan amount prompt to client");
        return false;
    }

    // Read the loan amount from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading loan amount from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Convert the loan amount to an integer
    loanAmount = atoi(readBuffer);
    if (loanAmount <= 0) {
        const char *errorMessage = "Invalid loan amount provided. Please enter a valid amount.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return false;
    }

    // Open the loans file for reading and writing, create if it does not exist
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (loanFileDescriptor == -1) {
        perror("Error opening loans file");
        write(connFD, "Internal error. Cannot access loan records.\n", 45);
        return false;
    }

    struct Loan newLoan, lastLoan;
    off_t fileSize = lseek(loanFileDescriptor, 0, SEEK_END);
    if (fileSize == 0) {
        // File is empty, set the first loan ID to 0
        newLoan.loanID = 0;
    } else {
        // Move to the last loan record
        if (lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_END) == -1) {
            perror("Error seeking to last loan record");
            close(loanFileDescriptor);
            return false;
        }

        // Read the last loan record to determine the new loan ID
        if (read(loanFileDescriptor, &lastLoan, sizeof(struct Loan)) != sizeof(struct Loan)) {
            perror("Error reading last loan record");
            close(loanFileDescriptor);
            return false;
        }

        // Set the new loan ID
        newLoan.loanID = lastLoan.loanID + 1;
    }

    // Set the new loan details
    newLoan.amount = loanAmount;
    newLoan.custID = loggedInCustomer.account;
    newLoan.status = 0; // Initial status: unassigned
    newLoan.empID = -1; // No employee assigned yet

    // Write lock to ensure exclusive access while adding the new loan
    off_t offset = lseek(loanFileDescriptor, 0, SEEK_END);
    if (offset == -1) {
        perror("Error seeking to end of loan file");
        close(loanFileDescriptor);
        return false;
    }

    struct flock writeLock;
    writeLock.l_type = F_WRLCK;
    writeLock.l_whence = SEEK_SET;
    writeLock.l_start = offset;
    writeLock.l_len = sizeof(struct Loan);
    writeLock.l_pid = getpid();
    if (fcntl(loanFileDescriptor, F_SETLKW, &writeLock) == -1) {
        perror("Error obtaining write lock on loan file");
        close(loanFileDescriptor);
        return false;
    }

    // Write the new loan record to the file
    if (write(loanFileDescriptor, &newLoan, sizeof(struct Loan)) == -1) {
        perror("Error writing new loan record to file");
        writeLock.l_type = F_UNLCK;
        fcntl(loanFileDescriptor, F_SETLK, &writeLock);
        close(loanFileDescriptor);
        return false;
    }

    // Release the write lock
    writeLock.l_type = F_UNLCK;
    fcntl(loanFileDescriptor, F_SETLK, &writeLock);

    // Close the file descriptor
    close(loanFileDescriptor);

    // Notify the client of success
    const char *successMessage = "Loan application has been successfully submitted.\n^";
    if (write(connFD, successMessage, strlen(successMessage)) == -1) {
        perror("Error sending success message to client");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));

    return true;
}




bool write_feedback(int connFD) {
    char readBuffer[500];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;

    // Retrieve account details
    if (!get_account_details(connFD, &account)) {
        return false;
    }

    // Check if the account is active
    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    struct Feedback newFeedback, lastFeedback;
    newFeedback.account = loggedInCustomer.account; // Set account number
    newFeedback.state = 0; // Set initial state

    // Open the feedback file in append mode
    int feedbackFileDescriptor = open("records/feedback.bank", O_CREAT | O_RDWR | O_APPEND, S_IRWXU);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file!");
        return false;
    }

    // Determine the new feedback ID
    struct flock lock = {F_WRLCK, SEEK_SET, 0, 0, getpid()};
    if (fcntl(feedbackFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining read lock on feedback record!");
        close(feedbackFileDescriptor);
        return false;
    }

    off_t fileSize = lseek(feedbackFileDescriptor, 0, SEEK_END);
    if (fileSize == 0) {
        newFeedback.id = 0; // First feedback ID
    } else {
        // Move to the last feedback record
        if (lseek(feedbackFileDescriptor, -sizeof(struct Feedback), SEEK_END) == -1) {
            perror("Error seeking to last feedback record!");
            close(feedbackFileDescriptor);
            return false;
        }

        // Read lock the last feedback record
        // struct flock lock = {F_RDLCK, SEEK_CUR, 0, sizeof(struct Feedback), getpid()};
        // if (fcntl(feedbackFileDescriptor, F_SETLKW, &lock) == -1) {
        //     perror("Error obtaining read lock on feedback record!");
        //     close(feedbackFileDescriptor);
        //     return false;
        // }

        // Read the last feedback record
        if (read(feedbackFileDescriptor, &lastFeedback, sizeof(struct Feedback)) != sizeof(struct Feedback)) {
            perror("Error reading last feedback record!");
            lock.l_type = F_UNLCK;
            fcntl(feedbackFileDescriptor, F_SETLK, &lock);
            close(feedbackFileDescriptor);
            return false;
        }

        // // Release the read lock
        // lock.l_type = F_UNLCK;
        // fcntl(feedbackFileDescriptor, F_SETLK, &lock);

        // Set the new feedback ID
        newFeedback.id = lastFeedback.id + 1;
    }

    // Prompt the client for feedback
    if (write(connFD, CUSTOMER_FEEDBACK, strlen(CUSTOMER_FEEDBACK)) == -1) {
        perror("Error writing CUSTOMER_FEEDBACK prompt to client!");
        writeLock.l_type = F_UNLCK;
        fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
        close(feedbackFileDescriptor);
        return false;
    }

    // Read the feedback message from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error reading feedback from client!");
        writeLock.l_type = F_UNLCK;
        fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
        close(feedbackFileDescriptor);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input

    // Copy the feedback message, ensuring it is null-terminated
    strncpy(newFeedback.message, readBuffer, sizeof(newFeedback.message) - 1);
    newFeedback.message[sizeof(newFeedback.message) - 1] = '\0';

    // Write lock to ensure exclusive access while adding the new feedback
    off_t offset = lseek(feedbackFileDescriptor, 0, SEEK_END);
    if (offset == -1) {
        perror("Error seeking to end of loan file");
        writeLock.l_type = F_UNLCK;
        fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
        close(feedbackFileDescriptor);
        return false;
    }

    // struct flock writeLock;
    // writeLock.l_type = F_WRLCK;
    // writeLock.l_whence = SEEK_SET;
    // writeLock.l_start = offset;
    // writeLock.l_len = sizeof(struct Loan);
    // writeLock.l_pid = getpid();
    // if (fcntl(feedbackFileDescriptor, F_SETLKW, &writeLock) == -1) {
    //     perror("Error obtaining write lock on feedback file!");
    //     close(feedbackFileDescriptor);
    //     return false;
    // }

    // Write the new feedback record to the file
    if (write(feedbackFileDescriptor, &newFeedback, sizeof(struct Feedback)) == -1) {
        perror("Error writing new feedback record to file!");
        writeLock.l_type = F_UNLCK;
        fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
        close(feedbackFileDescriptor);
        return false;
    }

    // Release the write lock
    writeLock.l_type = F_UNLCK;
    fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);

    // Close the file descriptor
    close(feedbackFileDescriptor);

    return true;
}




int write_transaction_to_file(int accountNumber, long beforeBalance, long afterBalance, int operation, int receiverAccountNumber) {
    int transactionFileDescriptor;
    struct flock lock;
    struct Transaction lastTransaction;
    struct Transaction newTransaction;
    off_t fileSize;
    ssize_t bytesRead;

    // Open the file for read/write (create if it doesn't exist)
    transactionFileDescriptor = open(TRANSACTION_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (transactionFileDescriptor == -1) {
        perror("Error opening transaction file");
        return -1;
    }

    // Prepare write lock for entire file
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0; // 0 means "lock whole file"
    lock.l_pid = getpid();

    // Acquire the lock
    if (fcntl(transactionFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error acquiring write lock on transaction file");
        close(transactionFileDescriptor);
        return -1;
    }

    // Find current file size
    fileSize = lseek(transactionFileDescriptor, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Error seeking to end of transaction file");
        // release lock
        lock.l_type = F_UNLCK;
        fcntl(transactionFileDescriptor, F_SETLK, &lock);
        close(transactionFileDescriptor);
        return -1;
    }

    int newTransactionID = 1; // default if file is empty

    if (fileSize >= sizeof(struct Transaction)) {
        // Move to start of last record
        if (lseek(transactionFileDescriptor, -sizeof(struct Transaction), SEEK_END) == -1) {
            perror("Error seeking to last transaction record");
            lock.l_type = F_UNLCK;
            fcntl(transactionFileDescriptor, F_SETLK, &lock);
            close(transactionFileDescriptor);
            return -1;
        }

        // Read last transaction
        bytesRead = read(transactionFileDescriptor, &lastTransaction, sizeof(struct Transaction));
        if (bytesRead == -1) {
            perror("Error reading last transaction record");
            lock.l_type = F_UNLCK;
            fcntl(transactionFileDescriptor, F_SETLK, &lock);
            close(transactionFileDescriptor);
            return -1;
        } else if (bytesRead == sizeof(struct Transaction)) {
            newTransactionID = lastTransaction.transactionID + 1;
        }
    }

    // Prepare new transaction record
    newTransaction.transactionID = newTransactionID;
    newTransaction.accountNumber = accountNumber;
    newTransaction.operation = operation;
    newTransaction.oldBalance = beforeBalance;
    newTransaction.newBalance = afterBalance;
    newTransaction.receiverAccNumber = receiverAccountNumber;
    newTransaction.transactionTime = time(NULL);

    // Seek to EOF before writing
    if (lseek(transactionFileDescriptor, 0, SEEK_END) == -1) {
        perror("Error seeking to end of transaction file for writing");
        lock.l_type = F_UNLCK;
        fcntl(transactionFileDescriptor, F_SETLK, &lock);
        close(transactionFileDescriptor);
        return -1;
    }

    // Write new transaction
    ssize_t writeBytes = write(transactionFileDescriptor, &newTransaction, sizeof(struct Transaction));
    if (writeBytes == -1 || writeBytes != sizeof(struct Transaction)) {
        perror("Error writing new transaction to file");
        lock.l_type = F_UNLCK;
        fcntl(transactionFileDescriptor, F_SETLK, &lock);
        close(transactionFileDescriptor);
        return -1;
    }

    // Unlock the file
    lock.l_type = F_UNLCK;
    if (fcntl(transactionFileDescriptor, F_SETLK, &lock) == -1) {
        perror("Error releasing lock on transaction file");
    }

    close(transactionFileDescriptor);

    return newTransactionID;
}



#endif