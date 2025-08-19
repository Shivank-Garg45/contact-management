#include <stdio.h>
#include <string.h>
#include "contact.h"


// ...existing code...
// Define global variables for contacts
Contact contacts[100];
int contactCount = 0;

void saveContacts(void) {
    FILE *fp = fopen("contacts.txt", "w");
    fprintf(fp, "%d\n", contactCount);
    for (int i = 0; i < contactCount; i++) {
        fprintf(fp, "%s %s %s %s\n",
                contacts[i].ContactName,
                contacts[i].ContactTel,
                contacts[i].ContactEmail,
                contacts[i].ContactAddress);
    }
    fclose(fp);
}
void loadContacts(void) {
    FILE *fp = fopen("contacts.txt", "r");
    if (!fp) return;
    fscanf(fp, "%d\n", &contactCount);
    for (int i = 0; i < contactCount; i++) {
        fscanf(fp, "%19s %19s %39s %39s",
               contacts[i].ContactName,
               contacts[i].ContactTel,
               contacts[i].ContactEmail,
               contacts[i].ContactAddress);
    }
    fclose(fp);
}

void addContact(void) {
    printf("\n Add a New Contact");
    printf("\nName: ");    
    scanf("%19s", contacts[contactCount].ContactName);
    printf("Phone: ");     
    scanf("%19s", contacts[contactCount].ContactTel);
    printf("Email: ");     
    scanf("%39s", contacts[contactCount].ContactEmail);
    printf("Address: ");   
    scanf("%39s", contacts[contactCount].ContactAddress);

    contactCount++;
    printf("\n Contact added successfully!\n");
    saveContacts();
}
void showContacts(void) {
    if (contactCount == 0) { puts("\n📭 No contacts to show."); return; }
    puts("\n Your Contacts:");
    for (int i = 0; i < contactCount; i++) {
        printf("\n[%d] %s | %s | %s | %s\n",
               i + 1,
               contacts[i].ContactName,
               contacts[i].ContactTel,
               contacts[i].ContactEmail,
               contacts[i].ContactAddress);
    }
}
void searchContact(void) {
    char query[20];
    printf("\n Enter name or phone number: ");
    scanf("%19s", query);

    for (int i = 0; i < contactCount; i++) {
        if (!strcmp(contacts[i].ContactName, query) ||
            !strcmp(contacts[i].ContactTel,  query)) {
            printf("\n Found:\n%s | %s | %s | %s\n",
                   contacts[i].ContactName,
                   contacts[i].ContactTel,
                   contacts[i].ContactEmail,
                   contacts[i].ContactAddress);
            return;
        }
    }
    puts("\n Contact not found.");
}
void deleteContact(void) {
    char query[20];
    printf("\n Enter name or phone number to delete: ");
    scanf("%19s", query);

    for (int i = 0; i < contactCount; i++) {
        if (!strcmp(contacts[i].ContactName, query) ||
            !strcmp(contacts[i].ContactTel,  query)) {
            for (int j = i; j < contactCount - 1; j++) contacts[j] = contacts[j+1];
            contactCount--;
            puts("\n Contact deleted.");
            saveContacts();
            return;
        }
    }
    puts("\n Contact not found.");
}
void sortContacts(void) {
    for (int i = 0; i < contactCount - 1; i++)
        for (int j = 0; j < contactCount - i - 1; j++)
            if (strcmp(contacts[j].ContactName, contacts[j+1].ContactName) > 0) {
                Contact tmp = contacts[j]; contacts[j] = contacts[j+1]; contacts[j+1] = tmp;
            }
    puts("\n Contacts sorted (A‑Z).");
    saveContacts();
}
void updateContact(void) {
    char query[20];
    printf("\n  Enter name or phone to update: ");
    scanf("%19s", query);

    for (int i = 0; i < contactCount; i++) {
        if (!strcmp(contacts[i].ContactName, query) ||
            !strcmp(contacts[i].ContactTel,  query)) {
            puts("\nEnter new details:");
            printf("New Name: ");    
            scanf("%19s", contacts[i].ContactName);
            printf("New Phone: ");   
            scanf("%19s", contacts[i].ContactTel);
            printf("New Email: ");   
            scanf("%39s", contacts[i].ContactEmail);
            printf("New Address: "); 
            scanf("%39s", contacts[i].ContactAddress);
            puts("\n Contact updated.");
            saveContacts();
            return;
        }
    }
    puts("\n Contact not found.");
}