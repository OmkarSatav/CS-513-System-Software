#ifndef EMPLOYEE_H
#define EMPLOYEE_H

struct Employee {
    int id; // Unique ID for the employee
    char name[25]; // Name of the employee
    char gender; // M -> Male, F -> Female, O -> Other
    int age; // Age of the employee

    // Login Credentials
    char login[30]; // Format: name-id (name will be the first word in the structure member `name`)
    char password[30]; // Password for the employee

    int employeeType; // 0 for Administrator, 1 for Manager, 2 for Bank Employee
};

#endif // EMPLOYEE_RECORD