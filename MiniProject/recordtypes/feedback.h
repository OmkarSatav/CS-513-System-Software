#ifndef FEEDBACK_RECORD
#define FEEDBACK_RECORD

struct Feedback
{
    int id; // 0, 1, 2 ....
    char message[500];
    int state; //0->passed to manager but manager has not read, 1->manager has read
    int account; // Account number of the account the customer owns
};

#endif