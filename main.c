#include <stdio.h>
#include <stdlib.h>
#include "contact.h"
#include "groups.h"
#include "auth.h"

int main(void) {
    int  choice;
    char loggedIn = '0';

    puts("\n===============================");
    puts(  " CONTACT MANAGEMENT SYSTEM ");
    puts(  "===============================");


    while (loggedIn == '0') {
        int a;
        printf("\n1. Login\n2. Register\n3. Exit\nChoose: ");
        scanf("%d", &a);
        switch (a) {
            case 1: loggedIn = loginUser(); break;
            case 2: registerUser();         break;
            case 3: puts("\n Bye!"); return 0;
            default: puts(" Invalid.");    break;
        }
    }

    loadContacts();   

    char again = 'y';
    while (again == 'y') {
        puts("\n---------------------------------");
        puts("1. Add Contact");
        puts("2. Show Contacts");
        puts("3. Search Contact");
        puts("4. Delete Contact");
        puts("5. Sort Contacts");
        puts("6. Update Contact");
        puts("7. Create Group");
        puts("8. Show Groups");
        puts("9. Exit");
        puts("---------------------------------");
        printf("Select: "); scanf("%d", &choice);

        if (choice == 9) {
            puts("\n Exiting.");
            break;
        }

        switch (choice) {
            case 1: addContact();   break;
            case 2: showContacts(); break;
            case 3: searchContact();break;
            case 4: deleteContact();break;
            case 5: sortContacts(); break;
            case 6: updateContact();break;
            case 7: createGroup();  break;
            case 8: showGroups();   break;
            default: puts(" Invalid option.");
        }
        printf("\nAnother action? (y/n): ");
        scanf(" %c", &again);
    }

    return 0;
}