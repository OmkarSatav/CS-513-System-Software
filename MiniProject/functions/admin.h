#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
// Function Prototypes =================================

bool admin_operation_handler(int connFD);
bool add_account(int connFD);
int add_customer(int connFD, bool isPrimary, int newAccountNumber);
bool delete_account(int connFD);
bool modify_customer_info(int connFD);
int add_employee(int connFD);
bool modify_employee_role(int connFD);
bool find_employee_by_id(int employeeId, struct Employee *employee);
bool update_employee_role(int employeeId, int newRole);


bool modify_employee_info(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    struct Employee employee;
    int employeeID;
    off_t offset;

    // Ask for Employee ID
    writeBytes = write(connFD, "Enter the Employee ID to modify: ", 34);    
    if (writeBytes == -1) {
        perror("Error while writing message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error while reading employee ID from client!");
        return false;
    }

    employeeID = atoi(readBuffer);

    int employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDONLY);
    if (employeeFileDescriptor == -1) {
        perror("Employee File doesn't exist");
        return false;
    }

    offset = lseek(employeeFileDescriptor, employeeID * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error while seeking to required employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};

    // Lock the record to be read
    int lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1) {
        perror("Couldn't obtain lock on employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    readBytes = read(employeeFileDescriptor, &employee, sizeof(struct Employee));
    if (readBytes == -1) {
        perror("Error while reading employee record from the file!");
        close(employeeFileDescriptor);
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);

    close(employeeFileDescriptor);

    // Send modification options
    writeBytes = write(connFD, "Modify Employee Menu:\n1. Name\n2. Age\n3. Gender\nChoose an option: ", 78);
    if (writeBytes == -1) {
        perror("Error while writing modify menu to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error while getting modification choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    bzero(readBuffer, sizeof(readBuffer));

    switch (choice) {
    case 1:
        writeBytes = write(connFD, "Enter new name: ", 16);
        if (writeBytes == -1) {
            perror("Error while writing new name prompt to client!");
            return false;
        }
        readBytes = read(connFD, employee.name, sizeof(employee.name));
        if (readBytes == -1) {
            perror("Error while getting response for employee's new name from client!");
            return false;
        }
        break;
    case 2:
        writeBytes = write(connFD, "Enter new age: ", 15);
        if (writeBytes == -1) {
            perror("Error while writing new age prompt to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error while getting response for employee's new age from client!");
            return false;
        }
        employee.age = atoi(readBuffer);
        break;
    case 3:
        writeBytes = write(connFD, "Enter new gender (M/F/O): ", 26);
        if (writeBytes == -1) {
            perror("Error while writing new gender prompt to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error while getting response for employee's new gender from client!");
            return false;
        }
        employee.gender = readBuffer[0];
        break;
    default:
        writeBytes = write(connFD, "Invalid choice!\n", 16);
        if (writeBytes == -1) {
            perror("Error while sending invalid choice message to client!");
            return false;
        }
        return false;
    }

    // Open the employee file to write the updated record
    employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
    if (employeeFileDescriptor == -1) {
        perror("Error while opening employee file");
        return false;
    }
    
    offset = lseek(employeeFileDescriptor, employeeID * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error while seeking to required employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1) {
        perror("Error while obtaining write lock on employee record!");
        return false;
    }

    writeBytes = write(employeeFileDescriptor, &employee, sizeof(struct Employee));
    if (writeBytes == -1) {
        perror("Error while writing updated employee info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);

    close(employeeFileDescriptor);

    writeBytes = write(connFD, "Employee information modified successfully!", 40);
    if (writeBytes == -1) {
        perror("Error while sending success message to client!");
        return false;
    }
    return true;
}





// =====================================================

// Function Definition =================================

// =====================================================

bool admin_operation_handler(int connFD)
{

    if (login_handler(true, connFD, NULL))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ADMIN_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, ADMIN_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ADMIN_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }

            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                
                get_customer_details(connFD, -1);
                break;
            case 2:
                get_account_details(connFD, NULL);
                break;
            case 3: 
                get_transaction_details(connFD, -1);
                break;
            case 4:
                add_account(connFD);
                break;
            case 5:
                delete_account(connFD);
                break;
            case 6:
                modify_customer_info(connFD);
                break;
            case 7:
                add_employee(connFD);
                break;
            case 8:
                modify_employee_info(connFD);
            case 9:
                modify_employee_role(connFD);
            default:
                writeBytes = write(connFD, ADMIN_LOGOUT, strlen(ADMIN_LOGOUT));
                return false;
            }
        }
    }
    else
    {
        // ADMIN LOGIN FAILED
        return false;
    }
    return true;
}



bool add_account(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Account newAccount, prevAccount;

    // Open the file in read/write mode and create it if it doesn't exist
    int accountFileDescriptor = open("./records/account.bank", O_CREAT| O_RDWR , S_IRWXU);
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

        close(accountFileDescriptor);

        newAccount.accountNumber = prevAccount.accountNumber + 1;
    }

    writeBytes = write(connFD, ADMIN_ADD_ACCOUNT_TYPE, strlen(ADMIN_ADD_ACCOUNT_TYPE));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_ACCOUNT_TYPE message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading account type response from client!");
        return false;
    }

    newAccount.isRegularAccount = atoi(readBuffer) == 1 ? true : false;


    newAccount.owners[0] = add_customer(connFD, true, newAccount.accountNumber);

    if (newAccount.isRegularAccount)
        newAccount.owners[1] = -1;
    else
        newAccount.owners[1] = add_customer(connFD, false, newAccount.accountNumber);

    newAccount.active = true;
    newAccount.balance = 0;

    memset(newAccount.transactions, -1, MAX_TRANSACTIONS * sizeof(int));

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
    readBytes = read(connFD, readBuffer, sizeof(read)); // Dummy read
    return true;
}





int add_customer(int connFD, bool isPrimary, int newAccountNumber)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Customer newCustomer, previousCustomer;

    // Open customer file to retrieve last customer ID or create a new customer
    int customerFileDescriptor = open(CUSTOMER_FILE, O_RDONLY );
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
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};
        int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Customer record!");
            return false;
        }

        readBytes = read(customerFileDescriptor, &previousCustomer, sizeof(struct Customer));
        if (readBytes == -1)
        {
            perror("Error while reading Customer record from file!");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(customerFileDescriptor, F_SETLK, &lock);

        close(customerFileDescriptor);

        newCustomer.id = previousCustomer.id + 1;
    }

    // Send prompt for customer name
    if (isPrimary)
        sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_PRIMARY, ADMIN_ADD_CUSTOMER_NAME);
    else
        sprintf(writeBuffer, "%s%s", ADMIN_ADD_CUSTOMER_SECONDARY, ADMIN_ADD_CUSTOMER_NAME);

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_CUSTOMER_NAME message to client!");
        return false;
    }

    // Read customer name from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer name response from client!");
        return false;
    }

    strcpy(newCustomer.name, readBuffer);

    // Send prompt for customer gender
    writeBytes = write(connFD, ADMIN_ADD_CUSTOMER_GENDER, strlen(ADMIN_ADD_CUSTOMER_GENDER));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_ADD_CUSTOMER_GENDER message to client!");
        return false;
    }

    // Read customer gender from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer gender response from client!");
        return false;
    }

    // Validate gender input
    if (readBuffer[0] == 'M' || readBuffer[0] == 'F' || readBuffer[0] == 'O')
        newCustomer.gender = readBuffer[0];
    else
    {
        writeBytes = write(connFD, ADMIN_ADD_CUSTOMER_WRONG_GENDER, strlen(ADMIN_ADD_CUSTOMER_WRONG_GENDER));
        return false;
    }

    // Send prompt for customer age
    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, "What is the age of the customer? ");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing age prompt to client!");
        return false;
    }

    // Read age from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading customer age response from client!");
        return false;
    }

    // Validate age input
    char *endptr;
    long customerAge = strtol(readBuffer, &endptr, 10);
    if (endptr == readBuffer || *endptr != '\0' || customerAge <= 0 || customerAge > 150)
    {
        printf("Invalid age received: %ld\n", customerAge);
        return false;
    }
    newCustomer.age = (int)customerAge;

    // Send prompt for customer password
    bzero(writeBuffer, sizeof(writeBuffer));
    strcpy(writeBuffer, "Enter the password: ");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing password prompt to client!");
        return false;
    }

    // Read password from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading password response from client!");
        return false;
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
        return false;
    }

    writeBytes = write(customerFileDescriptor, &newCustomer, sizeof(newCustomer));
    if (writeBytes == -1)
    {
        perror("Error while writing Customer record to file!");
        close(customerFileDescriptor);
        return false;
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
        return false;
    }

    // Dummy read to acknowledge completion
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));  // Dummy read

    return newCustomer.id;
}


bool delete_account(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Account account;

    writeBytes = write(connFD, ADMIN_DEL_ACCOUNT_NO, strlen(ADMIN_DEL_ACCOUNT_NO));
    if (writeBytes == -1)
    {
        perror("Error writing ADMIN_DEL_ACCOUNT_NO to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading account number response from the client!");
        return false;
    }

    int accountNumber = atoi(readBuffer);

    int accountFileDescriptor = open(ACCOUNT_FILE, O_RDONLY);
    if (accountFileDescriptor == -1)
    {
        // Account record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ACCOUNT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
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
    if (errno == EINVAL)
    {
        // Customer record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ACCOUNT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
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
        perror("Error obtaining read lock on Account record!");
        return false;
    }

    readBytes = read(accountFileDescriptor, &account, sizeof(struct Account));
    if (readBytes == -1)
    {
        perror("Error while reading Account record from file!");
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);

    close(accountFileDescriptor);

    bzero(writeBuffer, sizeof(writeBuffer));
    if (account.balance == 0)
    {
        // No money, hence can close account
        account.active = false;
        accountFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
        if (accountFileDescriptor == -1)
        {
            perror("Error opening Account file in write mode!");
            return false;
        }

        offset = lseek(accountFileDescriptor, accountNumber * sizeof(struct Account), SEEK_SET);
        if (offset == -1)
        {
            perror("Error seeking to the Account!");
            return false;
        }

        lock.l_type = F_WRLCK;
        lock.l_start = offset;

        int lockingStatus = fcntl(accountFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining write lock on the Account file!");
            return false;
        }

        writeBytes = write(accountFileDescriptor, &account, sizeof(struct Account));
        if (writeBytes == -1)
        {
            perror("Error deleting account record!");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);

        strcpy(writeBuffer, ADMIN_DEL_ACCOUNT_SUCCESS);
    }
    else
        // Account has some money ask customer to withdraw it
        strcpy(writeBuffer, ADMIN_DEL_ACCOUNT_FAILURE);
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error while writing final DEL message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}






bool modify_customer_info(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Customer customer;

    int customerID;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_ID, strlen(ADMIN_MOD_CUSTOMER_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_CUSTOMER_ID message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading customer ID from client!");
        return false;
    }

    customerID = atoi(readBuffer);

    int customerFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
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
    
    offset = lseek(customerFileDescriptor, customerID * sizeof(struct Customer), SEEK_SET);
    if (errno == EINVAL)
    {
        // Customer record doesn't exist
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
    else if (offset == -1)
    {
        perror("Error while seeking to required customer record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on customer record!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &customer, sizeof(struct Customer));
    if (readBytes == -1)
    {
        perror("Error while reading customer record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    close(customerFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_MENU, strlen(ADMIN_MOD_CUSTOMER_MENU));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_CUSTOMER_MENU message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting customer modification menu choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_NAME, strlen(ADMIN_MOD_CUSTOMER_NEW_NAME));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_NAME message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new name from client!");
            return false;
        }
        strcpy(customer.name, readBuffer);
        break;
    case 2:
        writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_AGE, strlen(ADMIN_MOD_CUSTOMER_NEW_AGE));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_AGE message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new age from client!");
            return false;
        }
        int updatedAge = atoi(readBuffer);
        if (updatedAge == 0)
        {
            // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
                return false;
            }
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
            return false;
        }
        customer.age = updatedAge;
        break;
    case 3:
        writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_GENDER, strlen(ADMIN_MOD_CUSTOMER_NEW_GENDER));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new gender from client!");
            return false;
        }
        customer.gender = readBuffer[0];
        break;
    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, INVALID_MENU_CHOICE);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    customerFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }
    offset = lseek(customerFileDescriptor, customerID * sizeof(struct Customer), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required customer record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on customer record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &customer, sizeof(struct Customer));
    if (writeBytes == -1)
    {
        perror("Error while writing update customer info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_SUCCESS, strlen(ADMIN_MOD_CUSTOMER_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_CUSTOMER_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}






// Function to add employee details
int add_employee(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    struct Employee newEmployee;

    // Open employee file to retrieve last employee ID or create a new employee
    int employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDONLY);
    if (employeeFileDescriptor == -1 && errno == ENOENT) {
        // Employee file was never created
        newEmployee.id = 0;
    } else if (employeeFileDescriptor == -1) {
        perror("Error while opening employee file");
        return -1;
    } else {
        // Seek to the last employee record and read it to determine the next employee ID
        int offset = lseek(employeeFileDescriptor, -sizeof(struct Employee), SEEK_END);
        if (offset == -1) {
            perror("Error seeking to last employee record!");
            return -1;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};
        int lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1) {
            perror("Error obtaining read lock on employee record!");
            return -1;
        }

        readBytes = read(employeeFileDescriptor, &newEmployee, sizeof(struct Employee));
        if (readBytes == -1) {
            perror("Error while reading employee record from file!");
            return -1;
        }

        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);

        close(employeeFileDescriptor);

        newEmployee.id = newEmployee.id + 1;
    }

    // Send prompt for employee name
    writeBytes = write(connFD, "Enter the employee name: ", 25);
    if (writeBytes == -1) {
        perror("Error writing employee name prompt to client!");
        return -1;
    }

    // Read employee name from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee name from client!");
        return -1;
    }
    strcpy(newEmployee.name, readBuffer);

    // Send prompt for employee gender
    writeBytes = write(connFD, "Enter the employee gender (M/F/O): ", 36);
    if (writeBytes == -1) {
        perror("Error writing employee gender prompt to client!");
        return -1;
    }

    // Read employee gender from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee gender from client!");
        return -1;
    }
    newEmployee.gender = readBuffer[0];

    // Send prompt for employee age
    writeBytes = write(connFD, "Enter the employee age: ", 25);
    if (writeBytes == -1) {
        perror("Error writing employee age prompt to client!");
        return -1;
    }

    // Read employee age from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee age from client!");
        return -1;
    }
    newEmployee.age = atoi(readBuffer);

    // Send prompt for employee password
    writeBytes = write(connFD, "Enter the password: ", 20);
    if (writeBytes == -1) {
        perror("Error writing password prompt to client!");
        return -1;
    }

    // Read password from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading password from client!");
        return -1;
    }
    strcpy(newEmployee.password, readBuffer);

    // Set employee type
    writeBytes = write(connFD, "Enter employee type (1 for Manager, 2 for Bank Employee): ", 58);
    if (writeBytes == -1) {
        perror("Error writing employee type prompt to client!");
        return -1;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee type from client!");
        return -1;
    }
    newEmployee.employeeType = atoi(readBuffer);

    // Write employee record to file
    employeeFileDescriptor = open(EMPLOYEE_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (employeeFileDescriptor == -1) {
        perror("Error while creating / opening employee file!");
        return -1;
    }

    writeBytes = write(employeeFileDescriptor, &newEmployee, sizeof(newEmployee));
    if (writeBytes == -1) {
        perror("Error while writing employee record to file!");
        close(employeeFileDescriptor);
        return -1;
    }

    close(employeeFileDescriptor); // Close the file after writing

    // Send success message
    writeBytes = write(connFD, "Employee added successfully!\n^", 30);
    if (writeBytes == -1) {
        perror("Error sending success message to client!");
        return -1;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));

    // Print employee details to the client
    int writtenBytes = snprintf(writeBuffer, sizeof(writeBuffer),
                "\nEmployee Details:\n"
                "ID: %d\n"
                "Name: %s\n"
                "Gender: %c\n"
                "Age: %d\n"
                "Login: %s-%d\n"
                "Password: %s\n"
                "Employee Type: %d (1: Manager, 2: Bank Employee)\n",
                newEmployee.id, newEmployee.name, newEmployee.gender, newEmployee.age,
                newEmployee.name, newEmployee.id, newEmployee.password, newEmployee.employeeType);

    // Check if snprintf truncated the output
    if (writtenBytes >= sizeof(writeBuffer)) {
        fprintf(stderr, "Error: Buffer overflow, data was truncated!\n");
        return -1;
    }

    // // Append the termination character '^'
    strcat(writeBuffer, "^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1) {
        perror("Error sending employee details to client!");
        return -1;
    }

    // Dummy read from the client after sending the termination character
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading dummy confirmation from client!");
        return -1;
    }

    return newEmployee.id;
}



// Function to modify the role of an employee
bool modify_employee_role(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int employeeId, newRole;

    // First, ensure that the current user is an admin
    // For simplicity, we assume the admin check happens before calling this function
    
    // Get the employee ID from the admin
    writeBytes = write(connFD, "Enter the Employee ID to change role: ", 39);
    if (writeBytes == -1) {
        perror("Error writing GET_EMPLOYEE_ID message to client!");
        return false;
    }
    
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading Employee ID from client!");
        return false;
    }

    employeeId = atoi(readBuffer);

    struct Employee employee;
    if (!find_employee_by_id(employeeId, &employee)) {
        write(connFD, "Employee not found.\n", 20);
        return false;
    }

    // Show the current role
    snprintf(writeBuffer, sizeof(writeBuffer), "Current Role: %d (0: Admin, 1: Manager, 2: Bank Employee)\n", employee.employeeType);
    write(connFD, writeBuffer, strlen(writeBuffer));
    
    // Get the new role from the admin
    writeBytes = write(connFD, "Enter the new role (0 for Admin, 1 for Manager, 2 for Bank Employee): ", 66);
    if (writeBytes == -1) {
        perror("Error writing GET_NEW_ROLE message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading new role from client!");
        return false;
    }

    newRole = atoi(readBuffer);
    if (newRole < 0 || newRole > 2) {
        write(connFD, "Invalid role. Please enter 0, 1, or 2.\n", 39);
        return false;
    }

    // Update the employee role
    if (update_employee_role(employeeId, newRole)) {
        write(connFD, "Employee role updated successfully.\n", 36);
        return true;
    } else {
        write(connFD, "Error updating employee role.\n", 30);
        return false;
    }
}



// Function to find employee by ID from the file
bool find_employee_by_id(int employeeId, struct Employee *employee) {
    FILE *file = fopen(EMPLOYEE_FILE, "r");
    if (file == NULL) {
        perror("Error opening employee file.");
        return false;
    }

    while (fread(employee, sizeof(struct Employee), 1, file)) {
        if (employee->id == employeeId) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}




// Function to update the employee role in the file
bool update_employee_role(int employeeId, int newRole) {
    FILE *file = fopen(EMPLOYEE_FILE, "r+");
    if (file == NULL) {
        perror("Error opening employee file.");
        return false;
    }

    struct Employee employee;
    while (fread(&employee, sizeof(struct Employee), 1, file)) {
        if (employee.id == employeeId) {
            // Found the employee, update the role
            employee.employeeType = newRole;

            // Move the file pointer back to the beginning of this record
            fseek(file, -sizeof(struct Employee), SEEK_CUR);

            // Write the updated employee record back to the file
            fwrite(&employee, sizeof(struct Employee), 1, file);

            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}






#endif