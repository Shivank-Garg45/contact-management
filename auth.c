#include <stdio.h>
#include <string.h>
#include "auth.h"

typedef struct
{
    char Name[20];
    char Password[20];
} USER;

void registerUser(void)
{
    USER u;
    printf("\n Name: ");    
    scanf("%19s", u.Name);
    printf(" Password: ");     
    scanf("%19s", u.Password);

    FILE *fp = fopen("customers.txt", "a");
    if (fp) {
        fprintf(fp, "%s %s\n", u.Name, u.Password);
        fclose(fp);
        puts("\n Registered.");
    } else {
        puts("\n Error opening file for registration.");
    }
}

char loginUser(void)
{
    FILE *fp = fopen("customers.txt", "r");
    if (!fp)
    {
        puts("\n No users. Please register.");
        return '0';
    }

    char name[20], pass[20];
    USER stored;
    printf("\n Username: "); 
    scanf("%19s", name);
    printf(" Password: "); 
    scanf("%19s", pass);

    while (fscanf(fp, "%19s %19s", stored.Name, stored.Password) == 2) {
        if (!strcmp(name, stored.Name) && !strcmp(pass, stored.Password))
        {
            fclose(fp);
            printf("\n Welcome, %s!\n", stored.Name);
            return '1';
        }
    }
    fclose(fp);
    puts("\n Incorrect credentials.");
    return '0';
}