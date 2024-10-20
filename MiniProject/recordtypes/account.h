#ifndef ACCOUNT_RECORD
#define ACCOUNT_RECORD

#define MAX_TRANSACTIONS 10

struct Account
{
    int accountNumber;     // 0, 1, 2, ....
    int owner;         // Customer IDs
    bool active;           // 1 -> Active, 0 -> Deactivated (Deleted)
    long int balance;      // Amount of money in the account
};

#endif