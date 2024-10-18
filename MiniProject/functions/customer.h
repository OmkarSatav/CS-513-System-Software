#ifndef CUSTOMER_FUNCTIONS
#define CUSTOMER_FUNCTIONS

// Semaphores are necessary joint account due the design choice I've made
#include <sys/ipc.h>
#include <sys/sem.h>

struct Customer loggedInCustomer;
int semIdentifier;

// Function Prototypes =================================

bool customer_operation_handler(int connFD);
bool deposit(int connFD);
bool withdraw(int connFD);
bool get_balance(int connFD);
bool transfer_funds(int connFD);
bool update_account_in_file(struct Account *account);
bool change_password(int connFD);
bool lock_critical_section(struct sembuf *semOp);
bool unlock_critical_section(struct sembuf *sem_op);
void write_transaction_to_array(int *transactionArray, int ID);
int write_transaction_to_file(int accountNumber, long int oldBalance, long int newBalance, int operation);


// =====================================================

// Function Definition =================================

bool customer_operation_handler(int connFD)
{
    if (login_handler(false, connFD, &loggedInCustomer))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client

        // Get a semaphore for the user
        key_t semKey = ftok(CUSTOMER_FILE, loggedInCustomer.account); // Generate a key based on the account number hence, different customers will have different semaphores

        union semun
        {
            int val; // Value of the semaphore
        } semSet;

        int semctlStatus;
        semIdentifier = semget(semKey, 1, 0); // Get the semaphore if it exists
        if (semIdentifier == -1)
        {
            semIdentifier = semget(semKey, 1, IPC_CREAT | 0700); // Create a new semaphore
            if (semIdentifier == -1)
            {
                perror("Error while creating semaphore!");
                _exit(1);
            }

            semSet.val = 1; // Set a binary semaphore
            semctlStatus = semctl(semIdentifier, 0, SETVAL, semSet);
            if (semctlStatus == -1)
            {
                perror("Error while initializing a binary sempahore!");
                _exit(1);
            }
        }

        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, CUSTOMER_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, CUSTOMER_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing CUSTOMER_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for CUSTOMER_MENU");
                return false;
            }
            
            // printf("READ BUFFER : %s\n", readBuffer);
            int choice = atoi(readBuffer);
            // printf("CHOICE : %d\n", choice);
            switch (choice)
            {
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
            default:
                writeBytes = write(connFD, CUSTOMER_LOGOUT, strlen(CUSTOMER_LOGOUT));
                return false;
            }
        }
    }
    else
    {
        // CUSTOMER LOGIN FAILED
        return false;
    }
    return true;
}

bool deposit(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;
    long int depositAmount = 0;

    // Lock the critical section
    struct sembuf semOp;
    lock_critical_section(&semOp);

    if (!get_account_details(connFD, &account)) {
        unlock_critical_section(&semOp);
        return false;
    }

    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        unlock_critical_section(&semOp);
        return false;
    }

    // Ask for the deposit amount
    writeBytes = write(connFD, DEPOSIT_AMOUNT, strlen(DEPOSIT_AMOUNT));
    if (writeBytes == -1) {
        perror("Error writing DEPOSIT_AMOUNT prompt to client!");
        unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading deposit amount from client!");
        unlock_critical_section(&semOp);
        return false;
    }

    depositAmount = atol(readBuffer);
    if (depositAmount <= 0) {
        writeBytes = write(connFD, DEPOSIT_AMOUNT_INVALID, strlen(DEPOSIT_AMOUNT_INVALID));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        unlock_critical_section(&semOp);
        return false;
    }

    // Log the transaction before updating the account balance
    int newTransactionID = write_transaction_to_file(account.accountNumber, account.balance, account.balance + depositAmount, 0);
    if (newTransactionID < 0) {
        unlock_critical_section(&semOp);
        return false; // Handle transaction logging failure
    }

    // Update account balance
    account.balance += depositAmount;

    // Update the account file with the new balance
    int accountFileDescriptor = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating!");
        unlock_critical_section(&semOp);
        return false;
    }

    off_t offset = lseek(accountFileDescriptor, account.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file!");
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on account record!");
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    writeBytes = write(accountFileDescriptor, &account, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated account balance to file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    // Unlock the account record
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);

    write(connFD, DEPOSIT_AMOUNT_SUCCESS, strlen(DEPOSIT_AMOUNT_SUCCESS));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    get_balance(connFD);
    unlock_critical_section(&semOp);
    return true;
}

bool withdraw(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;

    long int withdrawAmount = 0;

    // Lock the critical section
    struct sembuf semOp;
    lock_critical_section(&semOp);

    // Get account details
    if (!get_account_details(connFD, &account)) {
        unlock_critical_section(&semOp);
        perror("Failed to get account details");
        return false;
    }

    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        unlock_critical_section(&semOp);
        return false;
    }

    // Ask for the withdraw amount
    writeBytes = write(connFD, WITHDRAW_AMOUNT, strlen(WITHDRAW_AMOUNT));
    if (writeBytes == -1) {
        perror("Error writing WITHDRAW_AMOUNT message to client!");
        unlock_critical_section(&semOp);
        return false;
    }

    // Read the withdraw amount from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading withdraw amount from client!");
        unlock_critical_section(&semOp);
        return false;
    }

    withdrawAmount = atol(readBuffer);
    printf("Withdraw Amount Requested: %ld\n", withdrawAmount); // Debug log

    // Validate withdraw amount
    if (withdrawAmount > 0 && account.balance >= withdrawAmount) {
        // Log the transaction before updating the account balance
        int newTransactionID = write_transaction_to_file(account.accountNumber, account.balance, account.balance - withdrawAmount, 0);
        if (newTransactionID < 0) {
            unlock_critical_section(&semOp);
            return false; // Handle transaction logging failure
        }

        // Remove this line if not needed
        // write_transaction_to_array(account.transactions, newTransactionID); // Not needed for file logging

        // Update account balance
        account.balance -= withdrawAmount;

        // Update the account file with the new balance
        int accountFileDescriptor = open(ACCOUNT_FILE, O_RDWR);
        if (accountFileDescriptor == -1) {
            perror("Error opening account file for updating!");
            unlock_critical_section(&semOp);
            return false;
        }

        // Seek to the position of the account
        off_t offset = lseek(accountFileDescriptor, account.accountNumber * sizeof(struct Account), SEEK_SET);
        if (offset == -1) {
            perror("Error seeking in account file!");
            close(accountFileDescriptor);
            unlock_critical_section(&semOp);
            return false;
        }

        // Lock the account record for writing
        struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
        if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
            perror("Error obtaining write lock on account record!");
            close(accountFileDescriptor);
            unlock_critical_section(&semOp);
            return false;
        }

        // Write the updated account balance to the file
        writeBytes = write(accountFileDescriptor, &account, sizeof(struct Account));
        if (writeBytes == -1) {
            perror("Error writing updated account balance to file!");
            lock.l_type = F_UNLCK; // Unlock on error
            fcntl(accountFileDescriptor, F_SETLK, &lock);
            close(accountFileDescriptor);
            unlock_critical_section(&semOp);
            return false;
        }

        // Unlock the account record
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);

        // Notify client of success
        write(connFD, WITHDRAW_AMOUNT_SUCCESS, strlen(WITHDRAW_AMOUNT_SUCCESS));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

        // Get the updated balance for the client
        get_balance(connFD);
        unlock_critical_section(&semOp);
        return true;
    } else {
        // Invalid withdrawal amount
        writeBytes = write(connFD, WITHDRAW_AMOUNT_INVALID, strlen(WITHDRAW_AMOUNT_INVALID));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        unlock_critical_section(&semOp);
        return false;
    }
}

bool get_balance(int connFD)
{
    char buffer[1000];
    struct Account account;
    account.accountNumber = loggedInCustomer.account;
    if (get_account_details(connFD, &account))
    {
        bzero(buffer, sizeof(buffer));
        if (account.active)
        {
            sprintf(buffer, "You have ₹ %ld imaginary money in our bank!^", account.balance);
            write(connFD, buffer, strlen(buffer));
        }
        else
            write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, buffer, sizeof(buffer)); // Dummy read
    }
    else
    {
        // ERROR while getting balance
        return false;
    }
}

bool transfer_funds(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;
    struct Account senderAccount, receiverAccount;
    senderAccount.accountNumber = loggedInCustomer.account;
    long transferAmount = 0;

    // Lock the critical section
    struct sembuf semOp;
    lock_critical_section(&semOp);

    // Get sender's account details
    if (!get_account_details(connFD, &senderAccount)) {
        unlock_critical_section(&semOp);
        return false;
    }

    if (!senderAccount.active) {
        write(connFD, "Account deactivated", 20);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        unlock_critical_section(&semOp);
        return false;
    }

    // Ask for receiver's account ID
    writeBytes = write(connFD, "Enter the account ID of the recipient: ", 40);
    if (writeBytes == -1) {
        perror("Error writing recipient account prompt!");
        unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading recipient account ID!");
        unlock_critical_section(&semOp);
        return false;
    }

    int receiverAccountNumber = atoi(readBuffer);

    // Get recipient's account details
    receiverAccount.accountNumber = receiverAccountNumber;
    if (!get_account_details(receiverAccountNumber, &receiverAccount)) {
        write(connFD, "Recipient account not found!", 29);
        unlock_critical_section(&semOp);
        return false;
    }

    if (!receiverAccount.active) {
        write(connFD, "Recipient account is deactivated!", 33);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        unlock_critical_section(&semOp);
        return false;
    }

    // Ask for the transfer amount
    writeBytes = write(connFD, "Enter the amount to transfer: ", 30);
    if (writeBytes == -1) {
        perror("Error writing transfer amount prompt!");
        unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading transfer amount!");
        unlock_critical_section(&semOp);
        return false;
    }

    transferAmount = atol(readBuffer);

    if (transferAmount <= 0 || senderAccount.balance < transferAmount) {
        writeBytes = write(connFD, "Insufficient funds or invalid amount!", 37);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        unlock_critical_section(&semOp);
        return false;
    }

    // Log the transaction for sender (Withdraw operation)
    int newSenderTransactionID = write_transaction_to_file(senderAccount.accountNumber, senderAccount.balance, senderAccount.balance - transferAmount, 0);
    if (newSenderTransactionID < 0) {
        unlock_critical_section(&semOp);
        return false; // Handle transaction logging failure
    }

    // Deduct from sender's account
    senderAccount.balance -= transferAmount;

    // Update sender's account in file
    int accountFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating sender!");
        unlock_critical_section(&semOp);
        return false;
    }

    off_t offset = lseek(accountFileDescriptor, senderAccount.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file for sender!");
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on sender account record!");
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    writeBytes = write(accountFileDescriptor, &senderAccount, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated balance into sender account file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    // Unlock the sender account record
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);  // Close after updating sender

    // Reopen the file for the receiver's account
    accountFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating receiver!");
        unlock_critical_section(&semOp);
        return false;
    }

    // Log the transaction for receiver (Deposit operation)
    int newReceiverTransactionID = write_transaction_to_file(receiverAccount.accountNumber, receiverAccount.balance, receiverAccount.balance + transferAmount, 1);
    if (newReceiverTransactionID < 0) {
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false; // Handle transaction logging failure
    }

    // Add to receiver's account
    receiverAccount.balance += transferAmount;

    // Update receiver's account in file
    offset = lseek(accountFileDescriptor, receiverAccount.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file for receiver!");
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    lock.l_type = F_WRLCK;
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on receiver account record!");
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    writeBytes = write(accountFileDescriptor, &receiverAccount, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated balance into receiver account file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        unlock_critical_section(&semOp);
        return false;
    }

    // Unlock the receiver account record
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);

    // Notify sender and receiver
    writeBytes = write(connFD, "Transfer successful!^", 21);
    if (writeBytes == -1) {
        perror("Error sending transfer success message!");
        unlock_critical_section(&semOp);
        return false;
    }

    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    unlock_critical_section(&semOp);
    return true;
}



int write_transaction_to_file(int accountNumber, long beforeBalance, long afterBalance, int operation) {
    char transactionBuffer[1024];
    char operationType[10];
    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    // Determine the operation type (0 = Withdraw, 1 = Deposit)
    if (operation == 0) {
        strcpy(operationType, "Withdraw");
    } else if (operation == 1) {
        strcpy(operationType, "Deposit");
    }

    long difference = afterBalance - beforeBalance;

    // Format the transaction log with account number
    snprintf(transactionBuffer, sizeof(transactionBuffer), 
             "Account ID: %d\n"
             "Details of transaction:\n"
             "\tDate: %02d:%02d %02d/%02d/%d\n"
             "\tOperation: %s\n"
             "\tBalance -\n"
             "\t\tBefore: %ld\n"
             "\t\tAfter: %ld\n"
             "\t\tDifference: %ld\n\n",
             accountNumber, local->tm_hour, local->tm_min, local->tm_mday, local->tm_mon + 1, local->tm_year + 1900,
             operationType, beforeBalance, afterBalance, difference);

    // Write to transaction file
    int transactionFileDescriptor = open(TRANSACTION_FILE, O_CREAT | O_APPEND | O_WRONLY, 0777);
    if (transactionFileDescriptor == -1) {
        perror("Error opening transaction file");
        return -1;
    }

    if (write(transactionFileDescriptor, transactionBuffer, strlen(transactionBuffer)) == -1) {
        perror("Error writing transaction to file");
        close(transactionFileDescriptor);
        return -1;
    }

    close(transactionFileDescriptor);
    return 1;
}




bool change_password(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000], hashedPassword[1000];

    char newPassword[1000];

    // Lock the critical section
    struct sembuf semOp = {0, -1, SEM_UNDO};
    int semopStatus = semop(semIdentifier, &semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return false;
    }

    writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS));
    if (writeBytes == -1)
    {
        perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading old password response from client");
        unlock_critical_section(&semOp);
        return false;
    }
    
    if (strcmp(readBuffer, loggedInCustomer.password) == 0)  // Direct comparison of old password
    {
        // Password matches with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
            unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password response from client");
            unlock_critical_section(&semOp);
            return false;
        }

        strcpy(newPassword, readBuffer);  // Store the new password directly

        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
            unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password reenter response from client");
            unlock_critical_section(&semOp);
            return false;
        }

        if (strcmp(readBuffer, newPassword) == 0)  // Direct comparison of new and reentered password
        {
            // New & reentered passwords match
            strcpy(loggedInCustomer.password, newPassword);  // Store the new password directly

            int customerFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
            if (customerFileDescriptor == -1)
            {
                perror("Error opening customer file!");
                unlock_critical_section(&semOp);
                return false;
            }

            off_t offset = lseek(customerFileDescriptor, loggedInCustomer.id * sizeof(struct Customer), SEEK_SET);
            if (offset == -1)
            {
                perror("Error seeking to the customer record!");
                unlock_critical_section(&semOp);
                return false;
            }

            struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};
            int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining write lock on customer record!");
                unlock_critical_section(&semOp);
                return false;
            }

            writeBytes = write(customerFileDescriptor, &loggedInCustomer, sizeof(struct Customer));
            if (writeBytes == -1)
            {
                perror("Error storing updated customer password into customer record!");
                unlock_critical_section(&semOp);
                return false;
            }

            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(customerFileDescriptor, F_SETLK, &lock);

            close(customerFileDescriptor);

            writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

            unlock_critical_section(&semOp);

            return true;
        }
        else
        {
            // New & reentered passwords don't match
            writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        }
    }
    else
    {
        // Password doesn't match with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    }


    unlock_critical_section(&semOp);

    return false;
}

bool lock_critical_section(struct sembuf *semOp)
{
    semOp->sem_flg = SEM_UNDO;
    semOp->sem_op = -1;
    semOp->sem_num = 0;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return false;
    }
    return true;
}

bool unlock_critical_section(struct sembuf *semOp)
{
    semOp->sem_op = 1;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while operating on semaphore!");
        _exit(1);
    }
    return true;
}

void write_transaction_to_array(int *transactionArray, int ID)
{
    // Check if there's any free space in the array to write the new transaction ID
    int iter = 0;
    while (transactionArray[iter] != -1)
        iter++;

    if (iter >= MAX_TRANSACTIONS)
    {
        // No space
        for (iter = 1; iter < MAX_TRANSACTIONS; iter++)
            // Shift elements one step back discarding the oldest transaction
            transactionArray[iter - 1] = transactionArray[iter];
        transactionArray[iter - 1] = ID;
    }
    else
    {
        // Space available
        transactionArray[iter] = ID;
    }
}

// int write_transaction_to_file(int accountNumber, long int oldBalance, long int newBalance, bool operation)
// {
//     struct Transaction newTransaction;
//     newTransaction.accountNumber = accountNumber;
//     newTransaction.oldBalance = oldBalance;
//     newTransaction.newBalance = newBalance;
//     newTransaction.operation = operation;
//     newTransaction.transactionTime = time(NULL);

//     ssize_t readBytes, writeBytes;

//     int transactionFileDescriptor = open(TRANSACTION_FILE, O_CREAT | O_APPEND | O_RDWR, S_IRWXU);

//     // Get most recent transaction number
//     off_t offset = lseek(transactionFileDescriptor, -sizeof(struct Transaction), SEEK_END);
//     if (offset >= 0)
//     {
//         // There exists at least one transaction record
//         struct Transaction prevTransaction;
//         readBytes = read(transactionFileDescriptor, &prevTransaction, sizeof(struct Transaction));

//         newTransaction.transactionID = prevTransaction.transactionID + 1;
//     }
//     else
//         // No transaction records exist
//         newTransaction.transactionID = 0;

//     writeBytes = write(transactionFileDescriptor, &newTransaction, sizeof(struct Transaction));

//     return newTransaction.transactionID;
// }

// =====================================================

#endif