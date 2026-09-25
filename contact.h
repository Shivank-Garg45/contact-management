#ifndef CONTACT_H
#define CONTACT_H

#include <stdio.h>

#define MAX_CONTACTS 1000
#define MAX_NAME_LEN 64
#define MAX_PHONE_LEN 32
#define MAX_EMAIL_LEN 64
#define MAX_ADDR_LEN 128
#define MAX_COMPANY_LEN 64
#define MAX_GROUP_LEN 32
#define MAX_NOTES_LEN 256
#define MAX_DATE_LEN 32

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    char workPhone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    char address[MAX_ADDR_LEN];
    char company[MAX_COMPANY_LEN];
    char group[MAX_GROUP_LEN];
    int isFavorite;
    char notes[MAX_NOTES_LEN];
    char createdAt[MAX_DATE_LEN];
} Contact;

extern Contact contacts[MAX_CONTACTS];
extern int contactCount;

void safeReadLine(char *buffer, int maxLen);
int safeReadInt(int *outVal);

void loadContacts(void);
void saveContacts(void);
void clearContacts(void);
void addContact(void);
void showContacts(void);
void searchContact(void);
void updateContact(void);
void deleteContact(void);
void toggleFavorite(void);
void sortContacts(void);

void showFavorites(void);
void filterContactsByGroup(const char *groupName);
void exportContactsCSV(void);
void exportContactsVCF(void);
void importContactsCSV(void);
void showAnalytics(void);

#endif // CONTACT_H
