#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
// Function Prototypes =================================

bool admin_operation_handler(int connFD);
bool modify_customer_info(int connFD);
int add_employee(int connFD);
bool modify_employee_role(int connFD);
bool find_employee_by_id(int employeeId, struct Employee *employee);
bool update_employee_role(int employeeId, int newRole);
void change_admin_password(int connFD);


// bool modify_employee_info(int connFD) {
//     ssize_t readBytes, writeBytes;
//     char readBuffer[1000], writeBuffer[1000];
//     struct Employee employee;
//     int employeeID;
//     off_t offset;

//     // Ask for Employee ID
//     writeBytes = write(connFD, "Enter the Employee ID to modify: ", 34);    
//     if (writeBytes == -1) {
//         perror("Error while writing message to client!");
//         return false;
//     }
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error while reading employee ID from client!");
//         return false;
//     }

//     employeeID = atoi(readBuffer);

//     int employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDONLY);
//     if (employeeFileDescriptor == -1) {
//         perror("Employee File doesn't exist");
//         return false;
//     }

//     offset = lseek(employeeFileDescriptor, employeeID * sizeof(struct Employee), SEEK_SET);
//     if (offset == -1) {
//         perror("Error while seeking to required employee record!");
//         close(employeeFileDescriptor);
//         return false;
//     }

//     struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};

//     // Lock the record to be read
//     int lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
//     if (lockingStatus == -1) {
//         perror("Couldn't obtain lock on employee record!");
//         close(employeeFileDescriptor);
//         return false;
//     }

//     readBytes = read(employeeFileDescriptor, &employee, sizeof(struct Employee));
//     if (readBytes == -1) {
//         perror("Error while reading employee record from the file!");
//         close(employeeFileDescriptor);
//         return false;
//     }

//     // Unlock the record
//     lock.l_type = F_UNLCK;
//     fcntl(employeeFileDescriptor, F_SETLK, &lock);

//     close(employeeFileDescriptor);

//     // Send modification options
//     writeBytes = write(connFD, "Modify Employee Menu:\n1. Name\n2. Age\n3. Gender\nChoose an option: ", 78);
//     if (writeBytes == -1) {
//         perror("Error while writing modify menu to client!");
//         return false;
//     }

//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error while getting modification choice from client!");
//         return false;
//     }

//     int choice = atoi(readBuffer);
//     bzero(readBuffer, sizeof(readBuffer));

//     switch (choice) {
//     case 1:
//         writeBytes = write(connFD, "Enter new name: ", 16);
//         if (writeBytes == -1) {
//             perror("Error while writing new name prompt to client!");
//             return false;
//         }
//         readBytes = read(connFD, employee.name, sizeof(employee.name));
//         if (readBytes == -1) {
//             perror("Error while getting response for employee's new name from client!");
//             return false;
//         }
//         break;
//     case 2:
//         writeBytes = write(connFD, "Enter new age: ", 15);
//         if (writeBytes == -1) {
//             perror("Error while writing new age prompt to client!");
//             return false;
//         }
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//         if (readBytes == -1) {
//             perror("Error while getting response for employee's new age from client!");
//             return false;
//         }
//         employee.age = atoi(readBuffer);
//         break;
//     case 3:
//         writeBytes = write(connFD, "Enter new gender (M/F/O): ", 26);
//         if (writeBytes == -1) {
//             perror("Error while writing new gender prompt to client!");
//             return false;
//         }
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//         if (readBytes == -1) {
//             perror("Error while getting response for employee's new gender from client!");
//             return false;
//         }
//         employee.gender = readBuffer[0];
//         break;
//     default:
//         writeBytes = write(connFD, "Invalid choice!\n", 16);
//         if (writeBytes == -1) {
//             perror("Error while sending invalid choice message to client!");
//             return false;
//         }
//         return false;
//     }

//     // Open the employee file to write the updated record
//     employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
//     if (employeeFileDescriptor == -1) {
//         perror("Error while opening employee file");
//         return false;
//     }
    
//     offset = lseek(employeeFileDescriptor, employeeID * sizeof(struct Employee), SEEK_SET);
//     if (offset == -1) {
//         perror("Error while seeking to required employee record!");
//         close(employeeFileDescriptor);
//         return false;
//     }

//     lock.l_type = F_WRLCK;
//     lock.l_start = offset;
//     lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
//     if (lockingStatus == -1) {
//         perror("Error while obtaining write lock on employee record!");
//         return false;
//     }

//     writeBytes = write(employeeFileDescriptor, &employee, sizeof(struct Employee));
//     if (writeBytes == -1) {
//         perror("Error while writing updated employee info into file");
//     }

//     lock.l_type = F_UNLCK;
//     fcntl(employeeFileDescriptor, F_SETLK, &lock);

//     close(employeeFileDescriptor);

//     writeBytes = write(connFD, "Employee information modified successfully!", 40);
//     if (writeBytes == -1) {
//         perror("Error while sending success message to client!");
//         return false;
//     }
//     return true;
// }


bool modify_employee_info(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000];
    struct Employee employee;
    int employeeID;
    off_t offset;

    // Prompt for Employee ID
    if (write(connFD, "Enter the Employee ID to modify: ", 34) == -1) {
        perror("Error while writing message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error while reading employee ID from client!");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    employeeID = atoi(readBuffer);

    // Open the employee file
    int employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDWR);
    if (employeeFileDescriptor == -1) {
        perror("Error opening employee file");
        return false;
    }

    // Seek to the employee record
    offset = lseek(employeeFileDescriptor, employeeID * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error while seeking to required employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    // Lock the record for reading
    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Couldn't obtain lock on employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    // Read the employee record
    if (read(employeeFileDescriptor, &employee, sizeof(struct Employee)) != sizeof(struct Employee)) {
        perror("Error while reading employee record from the file!");
        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);
        close(employeeFileDescriptor);
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);

    // Prompt for modification options
    if (write(connFD, "Modify Employee Menu:\n1. Name\n2. Age\n3. Gender\nChoose an option: ", 78) == -1) {
        perror("Error while writing modify menu to client!");
        close(employeeFileDescriptor);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) {
        perror("Error while getting modification choice from client!");
        close(employeeFileDescriptor);
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate the input
    int choice = atoi(readBuffer);

    // Handle modification choice
    switch (choice) {
        case 1: // Modify Name
            if (write(connFD, "Enter new name: ", 16) == -1) {
                perror("Error while writing new name prompt to client!");
                close(employeeFileDescriptor);
                return false;
            }
            bzero(employee.name, sizeof(employee.name));
            readBytes = read(connFD, employee.name, sizeof(employee.name) - 1);
            if (readBytes <= 0) {
                perror("Error while getting response for employee's new name from client!");
                close(employeeFileDescriptor);
                return false;
            }
            employee.name[readBytes] = '\0'; // Null-terminate the input
            break;

        case 2: // Modify Age
            if (write(connFD, "Enter new age: ", 15) == -1) {
                perror("Error while writing new age prompt to client!");
                close(employeeFileDescriptor);
                return false;
            }
            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
            if (readBytes <= 0) {
                perror("Error while getting response for employee's new age from client!");
                close(employeeFileDescriptor);
                return false;
            }
            readBuffer[readBytes] = '\0'; // Null-terminate the input
            employee.age = atoi(readBuffer);
            break;

        case 3: // Modify Gender
            if (write(connFD, "Enter new gender (M/F/O): ", 26) == -1) {
                perror("Error while writing new gender prompt to client!");
                close(employeeFileDescriptor);
                return false;
            }
            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
            if (readBytes <= 0) {
                perror("Error while getting response for employee's new gender from client!");
                close(employeeFileDescriptor);
                return false;
            }
            employee.gender = readBuffer[0];
            break;

        default: // Invalid Choice
            if (write(connFD, "Invalid choice!\n", 16) == -1) {
                perror("Error while sending invalid choice message to client!");
            }
            close(employeeFileDescriptor);
            return false;
    }

    // Seek back to the employee record
    if (lseek(employeeFileDescriptor, offset, SEEK_SET) == -1) {
        perror("Error while seeking to required employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    // Lock the record for writing
    lock.l_type = F_WRLCK;
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error while obtaining write lock on employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    // Write the updated employee record
    if (write(employeeFileDescriptor, &employee, sizeof(struct Employee)) != sizeof(struct Employee)) {
        perror("Error while writing updated employee info into file");
        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);
        close(employeeFileDescriptor);
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);

    // Close the file descriptor
    close(employeeFileDescriptor);

    // Notify the client of success
    if (write(connFD, "Employee information modified successfully!", 40) == -1) {
        perror("Error while sending success message to client!");
        return false;
    }

    return true;
}




// bool admin_operation_handler(int connFD) {
//     if (login_handler(true, connFD, NULL)) {
//         ssize_t writeBytes, readBytes;            
//         char readBuffer[1000], writeBuffer[1000]; 

//         bzero(writeBuffer, sizeof(writeBuffer));
//         strcpy(writeBuffer, ADMIN_LOGIN_SUCCESS); 
//         write(connFD, writeBuffer, strlen(writeBuffer)); // Send login success message
        
//         while (1) {
//             bzero(writeBuffer, sizeof(writeBuffer));
//             strcat(writeBuffer, "\n");
//             strcat(writeBuffer, ADMIN_MENU); // Prepare the admin menu
//             writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//             if (writeBytes == -1) {
//                 perror("Error while writing ADMIN_MENU to client!");
//                 return false;
//             }

//             bzero(readBuffer, sizeof(readBuffer));
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//             if (readBytes == -1) {
//                 perror("Error while reading client's choice for ADMIN_MENU");
//                 return false;
//             }

//             int choice = atoi(readBuffer);
//             switch (choice) {
//                 case 1:
//                     add_employee(connFD);
//                     break;
//                 case 2:
//                     modify_customer_info(connFD);
//                     break;
//                 case 3:
//                     modify_employee_info(connFD);
//                     break;
//                 case 4:
//                     modify_employee_role(connFD);
//                     break;
//                 case 5:
//                     change_admin_password(connFD);
//                     break;
//                 case 6: // Logout
//                     writeBytes = write(connFD, "You have successfully logged out.\n", strlen("You have successfully logged out.\n"));
//                     return true; // Return to the main menu, keeping the connection open
//                 case 7: // Exit
//                     const char *exitMessage = "Exiting the application. Goodbye!🌟 type ok \n";
//                     ssize_t writeBytes = write(connFD, exitMessage, strlen(exitMessage));
//                     if (writeBytes == -1) {
//                         perror("Error sending exit message to client");
//                     }

//                     // Dummy read for acknowledgment from the client
//                     char readBuffer[100];
//                     bzero(readBuffer, sizeof(readBuffer)); // Clear the buffer

//                     // Reading the acknowledgment from the client
//                     ssize_t readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
//                     if (readBytes == -1) {
//                         perror("Error reading acknowledgment from client");
//                     } else if (readBytes > 0) {
//                         readBuffer[readBytes] = '\0'; // Null-terminate the received string
//                         printf("Received acknowledgment from client: %s\n", readBuffer);
//                     } else {
//                         printf("No acknowledgment received from client.\n");
//                     }

//                     // Close the client connection
//                     close(connFD);
//                     return false; 
//                 default:
//                     writeBytes = write(connFD, "Invalid choice. Please try again.\n", 36);
//                     if (writeBytes == -1) {
//                         perror("Error sending invalid choice message to client");
//                     }
//                     break; // Just notify the user and stay in the menu
//             }
//         }
//     } else {
//         // ADMIN LOGIN FAILED
//         const char *loginFailMessage = "Login failed. Please check your credentials and try again.\n";
//         ssize_t writeBytes = write(connFD, loginFailMessage, strlen(loginFailMessage));
//         if (writeBytes == -1) {
//             perror("Error sending login failure message to client");
//         }
//         return false;
//     }
//     return true;
// }

bool admin_operation_handler(int connFD) {
    if (!login_handler(true, connFD, NULL)) {
        // ADMIN LOGIN FAILED
        const char *loginFailMessage = "Login failed. Please check your credentials and try again.\n";
        if (write(connFD, loginFailMessage, strlen(loginFailMessage)) == -1) {
            perror("Error sending login failure message to client");
        }
        return false;
    }

    // ADMIN LOGIN SUCCESS
    if (write(connFD, ADMIN_LOGIN_SUCCESS, strlen(ADMIN_LOGIN_SUCCESS)) == -1) {
        perror("Error sending login success message to client");
        return false;
    }

    char readBuffer[1000], writeBuffer[1000];
    ssize_t readBytes, writeBytes;

    while (1) {
        // Display admin menu
        bzero(writeBuffer, sizeof(writeBuffer));
        snprintf(writeBuffer, sizeof(writeBuffer), "\n%s", ADMIN_MENU);
        if (write(connFD, writeBuffer, strlen(writeBuffer)) == -1) {
            perror("Error while writing ADMIN_MENU to client!");
            return false;
        }

        // Read admin's choice
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
        if (readBytes <= 0) {
            perror("Error while reading client's choice for ADMIN_MENU");
            return false;
        }
        readBuffer[readBytes] = '\0'; // Null-terminate the input
        int choice = atoi(readBuffer);

        switch (choice) {
            case 1: // Add Employee
                if (!add_employee(connFD)) {
                    perror("Error while adding employee");
                }
                break;

            case 2: // Modify Customer Info
                if (!modify_customer_info(connFD)) {
                    perror("Error while modifying customer info");
                }
                break;

            case 3: // Modify Employee Info
                if (!modify_employee_info(connFD)) {
                    perror("Error while modifying employee info");
                }
                break;

            case 4: // Modify Employee Role
                if (!modify_employee_role(connFD)) {
                    perror("Error while modifying employee role");
                }
                break;

            case 5: // Change Admin Password
                change_admin_password(connFD);
                break;

            case 6: // Logout
                if (write(connFD, "You have successfully logged out.\n", 35) == -1) {
                    perror("Error sending logout message to client");
                }
                return true; // Return to the main menu

            case 7: // Exit
                if (write(connFD, "Exiting the application. Goodbye!🌟 type ok \n", 45) == -1) {
                    perror("Error sending exit message to client");
                }

                // Dummy read for acknowledgment
                bzero(readBuffer, sizeof(readBuffer));
                readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
                if (readBytes > 0) {
                    readBuffer[readBytes] = '\0'; // Null-terminate the input
                    printf("Received acknowledgment from client: %s\n", readBuffer);
                } else {
                    printf("No acknowledgment received from client.\n");
                }

                close(connFD); // Close the client connection
                return false;

            default: // Invalid Choice
                if (write(connFD, "Invalid choice. Please try again.\n", 35) == -1) {
                    perror("Error sending invalid choice message to client");
                }
                break;
        }
    }
    return true;
}




// void change_admin_password(int connFD) {
//     char newPassword[100];
//     FILE *file;

//     // Prompt for new password
//     const char *promptMessage = "Enter new password: ";
//     write(connFD, promptMessage, strlen(promptMessage));

//     // Read new password from the client
//     bzero(newPassword, sizeof(newPassword));
//     ssize_t readBytes = read(connFD, newPassword, sizeof(newPassword) - 1);
//     if (readBytes == -1) {
//         perror("Error reading new password from client");
//         return;
//     }
//     newPassword[readBytes] = '\0'; // Ensure null-termination

//     // Open the credentials file for writing
//     file = fopen(CREDENTIALS_FILE, "w");
//     if (!file) {
//         perror("Error opening credentials file");
//         const char *errorMessage = "Error updating password. Please try again.\n";
//         write(connFD, errorMessage, strlen(errorMessage));
//         return;
//     }

//     // Write new credentials to the file
//     fprintf(file, "#ifndef ADMIN_CREDENTIALS\n#define ADMIN_CREDENTIALS\n\n");
//     fprintf(file, "#define ADMIN_LOGIN_ID \"admin\"\n#define ADMIN_PASSWORD \"%s\"\n\n", newPassword);
//     fprintf(file, "#endif\n");

//     // Close the file
//     fclose(file);

//     // Send success message back to the client
//     const char *successMessage = "Password updated successfully!\n";
//     write(connFD, successMessage, strlen(successMessage));
// }

void change_admin_password(int connFD) {
    char newPassword[100];
    FILE *file;

    // Prompt for new password
    const char *promptMessage = "Enter new password: ";
    if (write(connFD, promptMessage, strlen(promptMessage)) == -1) {
        perror("Error sending password prompt to client");
        return;
    }

    // Read new password from the client
    bzero(newPassword, sizeof(newPassword));
    ssize_t readBytes = read(connFD, newPassword, sizeof(newPassword) - 1);
    if (readBytes <= 0) {
        perror("Error reading new password from client");
        return;
    }
    newPassword[readBytes] = '\0'; // Ensure null-termination

    // Validate password length
    if (strlen(newPassword) < 6) {
        const char *errorMessage = "Password must be at least 6 characters long.\n";
        if (write(connFD, errorMessage, strlen(errorMessage)) == -1) {
            perror("Error sending password length error to client");
        }
        return;
    }

    // Open the credentials file for writing
    file = fopen(CREDENTIALS_FILE, "w");
    if (!file) {
        perror("Error opening credentials file");
        const char *errorMessage = "Error updating password. Please try again.\n";
        if (write(connFD, errorMessage, strlen(errorMessage)) == -1) {
            perror("Error sending file error message to client");
        }
        return;
    }

    // Write new credentials to the file
    if (fprintf(file, "#ifndef ADMIN_CREDENTIALS\n#define ADMIN_CREDENTIALS\n\n") < 0 ||
        fprintf(file, "#define ADMIN_LOGIN_ID \"admin\"\n#define ADMIN_PASSWORD \"%s\"\n\n", newPassword) < 0 ||
        fprintf(file, "#endif\n") < 0) {
        perror("Error writing to credentials file");
        fclose(file);
        const char *errorMessage = "Error updating password. Please try again.\n";
        if (write(connFD, errorMessage, strlen(errorMessage)) == -1) {
            perror("Error sending file write error message to client");
        }
        return;
    }

    // Close the file
    if (fclose(file) != 0) {
        perror("Error closing credentials file");
        const char *errorMessage = "Error finalizing password update. Please try again.\n";
        if (write(connFD, errorMessage, strlen(errorMessage)) == -1) {
            perror("Error sending file close error message to client");
        }
        return;
    }

    // Send success message back to the client
    const char *successMessage = "Password updated successfully!\n";
    if (write(connFD, successMessage, strlen(successMessage)) == -1) {
        perror("Error sending success message to client");
    }
}




// bool modify_customer_info(int connFD){
//     ssize_t readBytes, writeBytes;
//     char readBuffer[1000], writeBuffer[1000];

//     struct Customer customer;

//     int customerID;

//     off_t offset;
//     int lockingStatus;

//     writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_ID, strlen(ADMIN_MOD_CUSTOMER_ID));
//     if (writeBytes == -1)
//     {
//         perror("Error while writing ADMIN_MOD_CUSTOMER_ID message to client!");
//         return false;
//     }
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1)
//     {
//         perror("Error while reading customer ID from client!");
//         return false;
//     }

//     customerID = atoi(readBuffer);

//     int customerFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
//     if (customerFileDescriptor == -1)
//     {
//         // Customer File doesn't exist
//         bzero(writeBuffer, sizeof(writeBuffer));
//         strcpy(writeBuffer, CUSTOMER_ID_DOESNT_EXIT);
//         strcat(writeBuffer, "^");
//         writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//         if (writeBytes == -1)
//         {
//             perror("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
//             return false;
//         }
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
//         return false;
//     }
    
//     offset = lseek(customerFileDescriptor, customerID * sizeof(struct Customer), SEEK_SET);
//     if (errno == EINVAL)
//     {
//         // Customer record doesn't exist
//         bzero(writeBuffer, sizeof(writeBuffer));
//         strcpy(writeBuffer, CUSTOMER_ID_DOESNT_EXIT);
//         strcat(writeBuffer, "^");
//         writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//         if (writeBytes == -1)
//         {
//             perror("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
//             return false;
//         }
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
//         return false;
//     }
//     else if (offset == -1)
//     {
//         perror("Error while seeking to required customer record!");
//         return false;
//     }

//     struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};

//     // Lock the record to be read
//     lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
//     if (lockingStatus == -1)
//     {
//         perror("Couldn't obtain lock on customer record!");
//         return false;
//     }

//     readBytes = read(customerFileDescriptor, &customer, sizeof(struct Customer));
//     if (readBytes == -1)
//     {
//         perror("Error while reading customer record from the file!");
//         return false;
//     }

//     // Unlock the record
//     lock.l_type = F_UNLCK;
//     fcntl(customerFileDescriptor, F_SETLK, &lock);

//     close(customerFileDescriptor);

//     writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_MENU, strlen(ADMIN_MOD_CUSTOMER_MENU));
//     if (writeBytes == -1)
//     {
//         perror("Error while writing ADMIN_MOD_CUSTOMER_MENU message to client!");
//         return false;
//     }
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1)
//     {
//         perror("Error while getting customer modification menu choice from client!");
//         return false;
//     }

//     int choice = atoi(readBuffer);
//     if (choice == 0)
//     { // A non-numeric string was passed to atoi
//         bzero(writeBuffer, sizeof(writeBuffer));
//         strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
//         writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//         if (writeBytes == -1)
//         {
//             perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
//             return false;
//         }
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
//         return false;
//     }

//     bzero(readBuffer, sizeof(readBuffer));
//     switch (choice)
//     {
//     case 1:
//         writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_NAME, strlen(ADMIN_MOD_CUSTOMER_NEW_NAME));
//         if (writeBytes == -1)
//         {
//             perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_NAME message to client!");
//             return false;
//         }
//         readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
//         if (readBytes == -1)
//         {
//             perror("Error while getting response for customer's new name from client!");
//             return false;
//         }
//         strcpy(customer.name, readBuffer);
//         break;
//     case 2:
//         writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_AGE, strlen(ADMIN_MOD_CUSTOMER_NEW_AGE));
//         if (writeBytes == -1)
//         {
//             perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_AGE message to client!");
//             return false;
//         }
//         readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
//         if (readBytes == -1)
//         {
//             perror("Error while getting response for customer's new age from client!");
//             return false;
//         }
//         int updatedAge = atoi(readBuffer);
//         if (updatedAge == 0)
//         {
//             // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
//             bzero(writeBuffer, sizeof(writeBuffer));
//             strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
//             writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//             if (writeBytes == -1)
//             {
//                 perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
//                 return false;
//             }
//             readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
//             return false;
//         }
//         customer.age = updatedAge;
//         break;
//     case 3:
//         writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_GENDER, strlen(ADMIN_MOD_CUSTOMER_NEW_GENDER));
//         if (writeBytes == -1)
//         {
//             perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
//             return false;
//         }
//         readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
//         if (readBytes == -1)
//         {
//             perror("Error while getting response for customer's new gender from client!");
//             return false;
//         }
//         customer.gender = readBuffer[0];
//         break;
//     default:
//         bzero(writeBuffer, sizeof(writeBuffer));
//         strcpy(writeBuffer, INVALID_MENU_CHOICE);
//         writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//         if (writeBytes == -1)
//         {
//             perror("Error while writing INVALID_MENU_CHOICE message to client!");
//             return false;
//         }
//         readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
//         return false;
//     }

//     customerFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
//     if (customerFileDescriptor == -1)
//     {
//         perror("Error while opening customer file");
//         return false;
//     }
//     offset = lseek(customerFileDescriptor, customerID * sizeof(struct Customer), SEEK_SET);
//     if (offset == -1)
//     {
//         perror("Error while seeking to required customer record!");
//         return false;
//     }

//     lock.l_type = F_WRLCK;
//     lock.l_start = offset;
//     lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
//     if (lockingStatus == -1)
//     {
//         perror("Error while obtaining write lock on customer record!");
//         return false;
//     }

//     writeBytes = write(customerFileDescriptor, &customer, sizeof(struct Customer));
//     if (writeBytes == -1)
//     {
//         perror("Error while writing update customer info into file");
//     }

//     lock.l_type = F_UNLCK;
//     fcntl(customerFileDescriptor, F_SETLKW, &lock);

//     close(customerFileDescriptor);

//     writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_SUCCESS, strlen(ADMIN_MOD_CUSTOMER_SUCCESS));
//     if (writeBytes == -1)
//     {
//         perror("Error while writing ADMIN_MOD_CUSTOMER_SUCCESS message to client!");
//         return false;
//     }
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

//     return true;
// }

bool modify_customer_info(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    struct Customer customer;
    int customerID;
    off_t offset;

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_ID, strlen(ADMIN_MOD_CUSTOMER_ID));
    if (writeBytes == -1) {
        perror("Error while writing ADMIN_MOD_CUSTOMER_ID message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) {
        perror("Error while reading customer ID from client!");
        return false;
    }
    readBuffer[readBytes] = '\0';

    customerID = atoi(readBuffer);
    if (customerID < 0) {
        snprintf(writeBuffer, sizeof(writeBuffer), "%s^", CUSTOMER_ID_DOESNT_EXIT);
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        return false;
    }

    int customerFileDescriptor = open(CUSTOMER_FILE, O_RDWR);
    if (customerFileDescriptor == -1) {
        snprintf(writeBuffer, sizeof(writeBuffer), "%s^", CUSTOMER_ID_DOESNT_EXIT);
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        return false;
    }

    offset = customerID * sizeof(struct Customer);
    off_t fileSize = lseek(customerFileDescriptor, 0, SEEK_END);
    if (fileSize == -1 || offset >= fileSize) {
        snprintf(writeBuffer, sizeof(writeBuffer), "%s^", CUSTOMER_ID_DOESNT_EXIT);
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        close(customerFileDescriptor);
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};
    if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Couldn't obtain lock on customer record!");
        close(customerFileDescriptor);
        return false;
    }

    if (lseek(customerFileDescriptor, offset, SEEK_SET) == -1) {
        perror("Error seeking to customer record!");
        close(customerFileDescriptor);
        return false;
    }

    readBytes = read(customerFileDescriptor, &customer, sizeof(struct Customer));
    if (readBytes != sizeof(struct Customer)) {
        perror("Error reading customer record!");
        close(customerFileDescriptor);
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_MENU, strlen(ADMIN_MOD_CUSTOMER_MENU));
    if (writeBytes == -1) {
        perror("Error writing ADMIN_MOD_CUSTOMER_MENU message to client!");
        close(customerFileDescriptor);
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) {
        perror("Error reading choice from client!");
        close(customerFileDescriptor);
        return false;
    }
    readBuffer[readBytes] = '\0';

    int choice = atoi(readBuffer);
    if (choice <= 0) {
        snprintf(writeBuffer, sizeof(writeBuffer), "%s^", ERRON_INPUT_FOR_NUMBER);
        write(connFD, writeBuffer, strlen(writeBuffer));
        read(connFD, readBuffer, sizeof(readBuffer));
        close(customerFileDescriptor);
        return false;
    }

    switch (choice) {
        case 1:
            write(connFD, ADMIN_MOD_CUSTOMER_NEW_NAME, strlen(ADMIN_MOD_CUSTOMER_NEW_NAME));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
            if (readBytes <= 0) return false;
            readBuffer[readBytes] = '\0';
            strncpy(customer.name, readBuffer, sizeof(customer.name)-1);
            customer.name[sizeof(customer.name)-1] = '\0';
            break;
        case 2:
            write(connFD, ADMIN_MOD_CUSTOMER_NEW_AGE, strlen(ADMIN_MOD_CUSTOMER_NEW_AGE));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
            if (readBytes <= 0) return false;
            readBuffer[readBytes] = '\0';
            customer.age = atoi(readBuffer);
            if (customer.age <= 0) {
                snprintf(writeBuffer, sizeof(writeBuffer), "%s^", ERRON_INPUT_FOR_NUMBER);
                write(connFD, writeBuffer, strlen(writeBuffer));
                read(connFD, readBuffer, sizeof(readBuffer));
                close(customerFileDescriptor);
                return false;
            }
            break;
        case 3:
            write(connFD, ADMIN_MOD_CUSTOMER_NEW_GENDER, strlen(ADMIN_MOD_CUSTOMER_NEW_GENDER));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
            if (readBytes <= 0) return false;
            customer.gender = readBuffer[0];
            break;
        default:
            snprintf(writeBuffer, sizeof(writeBuffer), "%s^", INVALID_MENU_CHOICE);
            write(connFD, writeBuffer, strlen(writeBuffer));
            read(connFD, readBuffer, sizeof(readBuffer));
            close(customerFileDescriptor);
            return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    if (fcntl(customerFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock!");
        close(customerFileDescriptor);
        return false;
    }

    if (lseek(customerFileDescriptor, offset, SEEK_SET) == -1) {
        perror("Error seeking for write!");
        close(customerFileDescriptor);
        return false;
    }

    writeBytes = write(customerFileDescriptor, &customer, sizeof(customer));
    if (writeBytes != sizeof(customer)) {
        perror("Error writing updated record!");
        close(customerFileDescriptor);
        return false;
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);
    close(customerFileDescriptor);

    snprintf(writeBuffer, sizeof(writeBuffer), "%s^", ADMIN_MOD_CUSTOMER_SUCCESS);
    write(connFD, writeBuffer, strlen(writeBuffer));
    read(connFD, readBuffer, sizeof(readBuffer)); // dummy read

    return true;
}




// int add_employee(int connFD) {
//     ssize_t readBytes, writeBytes;
//     char readBuffer[500], writeBuffer[500];
//     struct Employee newEmployee;
//     int  employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDWR | O_CREAT | O_APPEND, 0777);

//     if (employeeFileDescriptor == -1) {
//         perror("Error opening employee file");
//         return -1;
//     }

//     // Check file size to see if it's empty
//     off_t fileSize = lseek(employeeFileDescriptor, 0, SEEK_END);
//     if (fileSize == -1) {
//         perror("Error checking file size");
//         close(employeeFileDescriptor);
//         return -1;
//     } else if (fileSize == 0) {
//         // File is empty; assign the first ID
//         newEmployee.id = 0;
//     } else {
//         // File is not empty; seek to the last employee record
//         if (lseek(employeeFileDescriptor, -sizeof(struct Employee), SEEK_END) == -1) {
//             perror("Error seeking to last employee record");
//             close(employeeFileDescriptor);
//             return -1;
//         }

//         struct flock lock = {F_RDLCK, SEEK_END, -sizeof(struct Employee), sizeof(struct Employee), getpid()};
//         if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
//             perror("Error obtaining read lock on last employee record");
//             close(employeeFileDescriptor);
//             return -1;
//         }

//         readBytes = read(employeeFileDescriptor, &newEmployee, sizeof(struct Employee));
//         if (readBytes == -1) {
//             perror("Error reading last employee record");
//             close(employeeFileDescriptor);
//             return -1;
//         }

//         lock.l_type = F_UNLCK;
//         fcntl(employeeFileDescriptor, F_SETLK, &lock);
        
//         // Increment ID for the new employee
//         newEmployee.id++;
//     }

//     // Send prompt for employee name
//     writeBytes = write(connFD, "Enter the employee name: ", 25);
//     if (writeBytes == -1) {
//         perror("Error writing employee name prompt to client!");
//         return -1;
//     }

//     // Read employee name from client
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading employee name from client!");
//         return -1;
//     }
//     strcpy(newEmployee.name, readBuffer);

//     // Send prompt for employee gender
//     writeBytes = write(connFD, "Enter the employee gender (M/F/O): ", 36);
//     if (writeBytes == -1) {
//         perror("Error writing employee gender prompt to client!");
//         return -1;
//     }

//     // Read employee gender from client
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading employee gender from client!");
//         return -1;
//     }
//     newEmployee.gender = readBuffer[0];

//     // Send prompt for employee age
//     writeBytes = write(connFD, "Enter the employee age: ", 25);
//     if (writeBytes == -1) {
//         perror("Error writing employee age prompt to client!");
//         return -1;
//     }

//     // Read employee age from client
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading employee age from client!");
//         return -1;
//     }
//     newEmployee.age = atoi(readBuffer);

//     // Send prompt for employee password
//     writeBytes = write(connFD, "Enter the password: ", 20);
//     if (writeBytes == -1) {
//         perror("Error writing password prompt to client!");
//         return -1;
//     }

//     // Read password from client
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading password from client!");
//         return -1;
//     }
//     strcpy(newEmployee.password, readBuffer);

//     // Set employee type
//     writeBytes = write(connFD, "Enter employee type (1 for Manager, 2 for Bank Employee): ", 58);
//     if (writeBytes == -1) {
//         perror("Error writing employee type prompt to client!");
//         return -1;
//     }

//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading employee type from client!");
//         return -1;
//     }
//     newEmployee.employeeType = atoi(readBuffer);

//     // Write employee record to file
//     employeeFileDescriptor = open(EMPLOYEE_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
//     if (employeeFileDescriptor == -1) {
//         perror("Error while creating / opening employee file!");
//         return -1;
//     }

//     writeBytes = write(employeeFileDescriptor, &newEmployee, sizeof(newEmployee));
//     if (writeBytes == -1) {
//         perror("Error while writing employee record to file!");
//         close(employeeFileDescriptor);
//         return -1;
//     }

//     close(employeeFileDescriptor); // Close the file after writing

//     // Send success message
//     writeBytes = write(connFD, "Employee added successfully!\n^", 30);
//     if (writeBytes == -1) {
//         perror("Error sending success message to client!");
//         return -1;
//     }
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));

//     // Print employee details to the client
//     int writtenBytes = snprintf(writeBuffer, sizeof(writeBuffer),
//                 "\nEmployee Details:\n"
//                 "ID: %d\n"
//                 "Name: %s\n"
//                 "Gender: %c\n"
//                 "Age: %d\n"
//                 "Login: %s-%d\n"
//                 "Password: %s\n"
//                 "Employee Type: %d (1: Manager, 2: Bank Employee)\n",
//                 newEmployee.id, newEmployee.name, newEmployee.gender, newEmployee.age,
//                 newEmployee.name, newEmployee.id, newEmployee.password, newEmployee.employeeType);

//     // Check if snprintf truncated the output
//     if (writtenBytes >= sizeof(writeBuffer)) {
//         fprintf(stderr, "Error: Buffer overflow, data was truncated!\n");
//         return -1;
//     }

//     // // Append the termination character '^'
//     strcat(writeBuffer, "^");

//     writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
//     if (writeBytes == -1) {
//         perror("Error sending employee details to client!");
//         return -1;
//     }

//     // Dummy read from the client after sending the termination character
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading dummy confirmation from client!");
//         return -1;
//     }

//     return newEmployee.id;
// }

int add_employee(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[500], writeBuffer[500];
    struct Employee newEmployee;
    int employeeFileDescriptor;

    // Open employee file read-write to check last record
    employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDWR | O_CREAT, 0666);
    if (employeeFileDescriptor == -1) {
        perror("Error opening employee file");
        return -1;
    }

    // Determine new employee ID
    off_t fileSize = lseek(employeeFileDescriptor, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Error checking file size");
        close(employeeFileDescriptor);
        return -1;
    }

    if (fileSize == 0) {
        newEmployee.id = 0;
    } else {
        if (lseek(employeeFileDescriptor, -sizeof(struct Employee), SEEK_END) == -1) {
            perror("Error seeking to last employee record");
            close(employeeFileDescriptor);
            return -1;
        }

        struct flock lock = {F_RDLCK, SEEK_END, -sizeof(struct Employee), sizeof(struct Employee), getpid()};
        if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
            perror("Error obtaining read lock on last employee record");
            close(employeeFileDescriptor);
            return -1;
        }

        readBytes = read(employeeFileDescriptor, &newEmployee, sizeof(newEmployee));
        if (readBytes != sizeof(newEmployee)) {
            perror("Error reading last employee record");
            lock.l_type = F_UNLCK;
            fcntl(employeeFileDescriptor, F_SETLK, &lock);
            close(employeeFileDescriptor);
            return -1;
        }

        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);

        newEmployee.id++;
    }

    // Prompt for employee name
    writeBytes = write(connFD, "Enter the employee name: ", 25);
    if (writeBytes == -1) return -1;
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes <= 0) return -1;
    readBuffer[readBytes] = '\0';
    readBuffer[strcspn(readBuffer, "\n")] = '\0'; // strip newline
    strncpy(newEmployee.name, readBuffer, sizeof(newEmployee.name)-1);
    newEmployee.name[sizeof(newEmployee.name)-1] = '\0';

    // Prompt for gender
    writeBytes = write(connFD, "Enter the employee gender (M/F/O): ", 36);
    if (writeBytes == -1) return -1;
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) return -1;
    readBuffer[readBytes] = '\0';
    readBuffer[strcspn(readBuffer, "\n")] = '\0';
    if (strlen(readBuffer) != 1 || (readBuffer[0] != 'M' && readBuffer[0] != 'F' && readBuffer[0] != 'O')) {
        write(connFD, "Invalid gender. Must be M, F, or O.\n^", 36);
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        close(employeeFileDescriptor);
        return -1;
    }
    newEmployee.gender = readBuffer[0];

    // Prompt for age
    writeBytes = write(connFD, "Enter the employee age: ", 25);
    if (writeBytes == -1) return -1;
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) return -1;
    readBuffer[readBytes] = '\0';
    newEmployee.age = atoi(readBuffer);
    if (newEmployee.age <= 0) {
        write(connFD, "Invalid age.\n^", 15);
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        close(employeeFileDescriptor);
        return -1;
    }

    // Prompt for password
    writeBytes = write(connFD, "Enter the password: ", 20);
    if (writeBytes == -1) return -1;
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) return -1;
    readBuffer[readBytes] = '\0';
    readBuffer[strcspn(readBuffer, "\n")] = '\0';
    strncpy(newEmployee.password, readBuffer, sizeof(newEmployee.password)-1);
    newEmployee.password[sizeof(newEmployee.password)-1] = '\0';

    // Prompt for employee type
    writeBytes = write(connFD, "Enter employee type (1 for Manager, 2 for Bank Employee): ", 58);
    if (writeBytes == -1) return -1;
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) return -1;
    readBuffer[readBytes] = '\0';
    newEmployee.employeeType = atoi(readBuffer);
    if (newEmployee.employeeType != 1 && newEmployee.employeeType != 2) {
        write(connFD, "Invalid employee type.\n^", 25);
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        close(employeeFileDescriptor);
        return -1;
    }

    // Lock file for writing
    struct flock lock = {F_WRLCK, SEEK_END, 0, 0, getpid()};
    if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error locking file for writing");
        close(employeeFileDescriptor);
        return -1;
    }

    // Write new employee record
    writeBytes = write(employeeFileDescriptor, &newEmployee, sizeof(newEmployee));
    if (writeBytes != sizeof(newEmployee)) {
        perror("Error writing employee record");
        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);
        close(employeeFileDescriptor);
        return -1;
    }

    // Unlock
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);
    close(employeeFileDescriptor);

    // Send success message
    writeBytes = write(connFD, "Employee added successfully!\n^", 30);
    if (writeBytes == -1) return -1;
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));

    // Send employee details
    int writtenBytes = snprintf(writeBuffer, sizeof(writeBuffer),
        "\nEmployee Details:\n"
        "ID: %d\n"
        "Name: %s\n"
        "Gender: %c\n"
        "Age: %d\n"
        "Login: %s-%d\n"
        "Password: %s\n"
        "Employee Type: %d (1: Manager, 2: Bank Employee)\n^",
        newEmployee.id, newEmployee.name, newEmployee.gender,
        newEmployee.age, newEmployee.name, newEmployee.id,
        newEmployee.password, newEmployee.employeeType);

    if (writtenBytes >= sizeof(writeBuffer)) {
        fprintf(stderr, "Buffer overflow prevented\n");
        return -1;
    }

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1) return -1;

    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
    if (readBytes <= 0) return -1;

    return newEmployee.id;
}





// bool modify_employee_role(int connFD) {
//     ssize_t readBytes, writeBytes;
//     char readBuffer[1000], writeBuffer[1000];
//     int employeeId, newRole;
    
//     // Get the employee ID from the admin
//     writeBytes = write(connFD, "Enter the Employee ID to change role: ", 39);
//     if (writeBytes == -1) {
//         perror("Error writing GET_EMPLOYEE_ID message to client!");
//         return false;
//     }
    
//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading Employee ID from client!");
//         return false;
//     }

//     employeeId = atoi(readBuffer);

//     struct Employee employee;
//     if (!find_employee_by_id(employeeId, &employee)) {
//         write(connFD, "Employee not found.\n", 20);
//         return false;
//     }

//     // Show the current role
//     snprintf(writeBuffer, sizeof(writeBuffer), "Current Role: %d ( Manager, 2: Bank Employee)\n", employee.employeeType);
//     write(connFD, writeBuffer, strlen(writeBuffer));
    
//     // Get the new role from the admin
//     writeBytes = write(connFD, "Enter the new role (1 for Manager, 2 for Bank Employee): ", 58);
//     if (writeBytes == -1) {
//         perror("Error writing GET_NEW_ROLE message to client!");
//         return false;
//     }

//     bzero(readBuffer, sizeof(readBuffer));
//     readBytes = read(connFD, readBuffer, sizeof(readBuffer));
//     if (readBytes == -1) {
//         perror("Error reading new role from client!");
//         return false;
//     }

//     newRole = atoi(readBuffer);
//     if (newRole < 0 || newRole > 2) {
//         write(connFD, "Invalid role. Please enter 0, 1, or 2.\n", 39);
//         return false;
//     }

//     if (update_employee_role(employeeId, newRole)) {
//         write(connFD, "Employee role updated successfully.\n", 36);
//         return true;
//     } else {
//         write(connFD, "Error updating employee role.\n", 30);
//         return false;
//     }
// }

bool modify_employee_role(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int employeeId, newRole;

    writeBytes = write(connFD, "Enter the Employee ID to change role: ", 39);
    if (writeBytes == -1) {
        perror("Error writing prompt");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) {
        perror("Error reading employee ID");
        return false;
    }
    readBuffer[readBytes] = '\0';

    employeeId = atoi(readBuffer);
    if (employeeId < 0) {
        write(connFD, "Invalid ID.\n", 12);
        return false;
    }

    struct Employee employee;
    if (!find_employee_by_id(employeeId, &employee)) {
        write(connFD, "Employee not found.\n", 20);
        return false;
    }

    snprintf(writeBuffer, sizeof(writeBuffer),
             "Current Role: %d (1=Manager, 2=Bank Employee)\n", employee.employeeType);
    write(connFD, writeBuffer, strlen(writeBuffer));

    writeBytes = write(connFD,
                       "Enter new role (1=Manager, 2=Bank Employee): ",
                       47);
    if (writeBytes == -1) return false;

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)-1);
    if (readBytes <= 0) return false;
    readBuffer[readBytes] = '\0';

    newRole = atoi(readBuffer);
    if (newRole != 1 && newRole != 2) {
        write(connFD, "Invalid role value.\n", 20);
        return false;
    }

    if (update_employee_role(employeeId, newRole)) {
        write(connFD, "Employee role updated successfully.\n^", 37);
        read(connFD, readBuffer, sizeof(readBuffer)); // dummy read
        return true;
    } else {
        write(connFD, "Error updating employee role.\n^", 32);
        read(connFD, readBuffer, sizeof(readBuffer));
        return false;
    }
}





// bool find_employee_by_id(int employeeId, struct Employee *employee) {
//     FILE *file = fopen(EMPLOYEE_FILE, "r");
//     if (file == NULL) {
//         perror("Error opening employee file.");
//         return false;
//     }

//     while (fread(employee, sizeof(struct Employee), 1, file)) {
//         if (employee->id == employeeId) {
//             fclose(file);
//             return true;
//         }
//     }

//     fclose(file);
//     return false;
// }

bool find_employee_by_id(int employeeId, struct Employee *employee) {
    FILE *file = fopen(EMPLOYEE_FILE, "rb");
    if (!file) {
        perror("Error opening employee file");
        return false;
    }

    while (fread(employee, sizeof(struct Employee), 1, file) == 1) {
        if (employee->id == employeeId) {
            fclose(file);
            return true;
        }
    }
    fclose(file);
    return false;
}





// bool update_employee_role(int employeeId, int newRole) {
//     FILE *file = fopen(EMPLOYEE_FILE, "r+");
//     if (file == NULL) {
//         perror("Error opening employee file.");
//         return false;
//     }

//     struct Employee employee;
//     while (fread(&employee, sizeof(struct Employee), 1, file)) {
//         if (employee.id == employeeId) {
//             // Found the employee, update the role
//             employee.employeeType = newRole;

//             // Move the file pointer back to the beginning of this record
//             fseek(file, -sizeof(struct Employee), SEEK_CUR);

//             // Write the updated employee record back to the file
//             fwrite(&employee, sizeof(struct Employee), 1, file);

//             fclose(file);
//             return true;
//         }
//     }

//     fclose(file);
//     return false;
// }


bool update_employee_role(int employeeId, int newRole) {
    FILE *file = fopen(EMPLOYEE_FILE, "rb+");
    if (!file) {
        perror("Error opening employee file");
        return false;
    }

    struct Employee employee;
    while (fread(&employee, sizeof(employee), 1, file) == 1) {
        if (employee.id == employeeId) {
            employee.employeeType = newRole;
            fseek(file, -sizeof(employee), SEEK_CUR);
            size_t written = fwrite(&employee, sizeof(employee), 1, file);
            fclose(file);
            return written == 1;
        }
    }
    fclose(file);
    return false;
}




#endif