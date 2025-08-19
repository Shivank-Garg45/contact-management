#ifndef CONTACT_H
#define CONTACT_H

typedef struct {
    char ContactName[20];
    char ContactTel[20];
    char ContactEmail[40];
    char ContactAddress[40];
} Contact;

extern Contact contacts[100];
extern int contactCount;

void saveContacts(void);
void loadContacts(void);
void addContact(void);
void showContacts(void);
void searchContact(void);
void deleteContact(void);
void sortContacts(void);
void updateContact(void);

#endif
