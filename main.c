#include <stdio.h>
#include <stdlib.h>
#include "contact.h"
#include "groups.h"
#include "auth.h"

static void printHeader(void) {
    puts("\n=========================================================");
    puts("     MULTI-USER CONTACT MANAGEMENT SYSTEM (C99)          ");
    puts("=========================================================");
}

static void printUserStatus(void) {
    printf(" [User: %s | Personal Contacts: %d | Groups: %d]\n",
           getCurrentUser(), contactCount, groupCount);
}

int main(void) {
    printHeader();

    while (1) {
        while (!isLoggedIn()) {
            printf("\n1. Login\n2. Register New User\n3. Exit\nChoose option: ");
            int authChoice = 0;
            if (!safeReadInt(&authChoice)) {
                printf("[!] Invalid input. Please enter 1, 2, or 3.\n");
                continue;
            }

            switch (authChoice) {
                case 1:
                    loginUser();
                    break;
                case 2:
                    registerUser();
                    break;
                case 3:
                    puts("\nGoodbye!");
                    return 0;
                default:
                    puts("[!] Invalid choice. Please try again.");
                    break;
            }
        }

        // Load isolated user data
        loadGroups();
        loadContacts();

        int choice = 0;
        int userSessionActive = 1;

        while (userSessionActive) {
            puts("\n---------------------------------------------------------");
            printUserStatus();
            puts("---------------------------------------------------------");
            puts(" 1.  View All Contacts");
            puts(" 2.  Add New Contact");
            puts(" 3.  Search Contacts (Name, Phone, Email, Company, Group)");
            puts(" 4.  Update Contact Details");
            puts(" 5.  Delete Contact");
            puts(" 6.  Star / Unstar Favorite Contact");
            puts(" 7.  View Favorite Contacts");
            puts(" 8.  Sort Contacts (A-Z, Z-A, Company, Date)");
            puts(" 9.  Manage Groups (Create, View, Delete, Members)");
            puts(" 10. Export to CSV & vCard (VCF)");
            puts(" 11. Import from CSV");
            puts(" 12. View Contact Analytics & Statistics");
            puts(" 13. Logout / Switch User Account");
            puts(" 14. Exit Application");
            puts("---------------------------------------------------------");
            printf("Select an option (1-14): ");

            if (!safeReadInt(&choice)) {
                puts("[!] Invalid option. Please enter a number between 1 and 14.");
                continue;
            }

            if (choice == 14) {
                puts("\nSaving user data... Exiting Contact Management System. Goodbye!");
                saveContacts();
                saveGroups();
                return 0;
            }

            switch (choice) {
                case 1:  showContacts(); break;
                case 2:  addContact(); break;
                case 3:  searchContact(); break;
                case 4:  updateContact(); break;
                case 5:  deleteContact(); break;
                case 6:  toggleFavorite(); break;
                case 7:  showFavorites(); break;
                case 8:  sortContacts(); break;
                case 9:  manageGroupsMenu(); break;
                case 10:
                    exportContactsCSV();
                    exportContactsVCF();
                    break;
                case 11: importContactsCSV(); break;
                case 12: showAnalytics(); break;
                case 13:
                    saveContacts();
                    saveGroups();
                    clearContacts();
                    clearGroups();
                    logoutUser();
                    userSessionActive = 0;
                    puts("\n[✓] Logged out successfully. Returning to Login menu.\n");
                    break;
                default:
                    puts("[!] Invalid option. Please choose between 1 and 14.");
                    break;
            }
        }
    }

    return 0;
}
