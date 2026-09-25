#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "groups.h"
#include "contact.h"
#include "auth.h"

Group groups[MAX_GROUPS];
int groupCount = 0;

static void safeCopy(char *dst, const char *src, size_t dstSize) {
    if (!dst || dstSize == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    size_t len = strlen(src);
    if (len >= dstSize) len = dstSize - 1;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

static void getUserGroupsFilePath(char *dest, size_t maxLen) {
    const char *user = getCurrentUser();
    if (!user || strlen(user) == 0 || strcmp(user, "Guest") == 0) {
        snprintf(dest, maxLen, "groups.txt");
    } else {
        snprintf(dest, maxLen, "data/%s_groups.txt", user);
    }
}

void clearGroups(void) {
    groupCount = 0;
    memset(groups, 0, sizeof(groups));
}

int groupExists(const char *name) {
    for (int i = 0; i < groupCount; i++) {
        if (strcasecmp(groups[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

int getContactCountInGroup(const char *groupName) {
    int count = 0;
    for (int i = 0; i < contactCount; i++) {
        if (strcasecmp(contacts[i].group, groupName) == 0) {
            count++;
        }
    }
    return count;
}

void saveGroups(void) {
    char filepath[256];
    getUserGroupsFilePath(filepath, sizeof(filepath));

    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        printf("Error: Could not save groups to %s\n", filepath);
        return;
    }

    fprintf(fp, "%d\n", groupCount);
    for (int i = 0; i < groupCount; i++) {
        fprintf(fp, "%d|%s|%s\n",
                groups[i].id,
                groups[i].name,
                groups[i].description[0] ? groups[i].description : "No description");
    }
    fclose(fp);
}

void loadGroups(void) {
    clearGroups();
    char filepath[256];
    getUserGroupsFilePath(filepath, sizeof(filepath));

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        // Fallback for legacy groups.txt if divyanshsood22
        if (strcmp(getCurrentUser(), "divyanshsood22") == 0) {
            fp = fopen("groups.txt", "r");
        }
    }

    if (!fp) {
        // Initialize default groups for new user
        groupCount = 0;
        groups[groupCount].id = 1;
        safeCopy(groups[groupCount].name, "Family", sizeof(groups[groupCount].name));
        safeCopy(groups[groupCount].description, "Immediate and extended family", sizeof(groups[groupCount].description));
        groupCount++;

        groups[groupCount].id = 2;
        safeCopy(groups[groupCount].name, "Friends", sizeof(groups[groupCount].name));
        safeCopy(groups[groupCount].description, "Personal friends and buddies", sizeof(groups[groupCount].description));
        groupCount++;

        groups[groupCount].id = 3;
        safeCopy(groups[groupCount].name, "Work", sizeof(groups[groupCount].name));
        safeCopy(groups[groupCount].description, "Colleagues and professional network", sizeof(groups[groupCount].description));
        groupCount++;

        groups[groupCount].id = 4;
        safeCopy(groups[groupCount].name, "Clients", sizeof(groups[groupCount].name));
        safeCopy(groups[groupCount].description, "Customers and business partners", sizeof(groups[groupCount].description));
        groupCount++;

        saveGroups();
        return;
    }

    char line[256];
    if (fgets(line, sizeof(line), fp) != NULL) {}

    while (fgets(line, sizeof(line), fp) && groupCount < MAX_GROUPS) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        int id = 0;
        char name[MAX_GROUP_NAME_LEN] = "";
        char desc[MAX_GROUP_DESC_LEN] = "";

        char *token = strtok(line, "|");
        if (token) id = atoi(token);
        token = strtok(NULL, "|");
        if (token) safeCopy(name, token, sizeof(name));
        token = strtok(NULL, "|");
        if (token) safeCopy(desc, token, sizeof(desc));

        if (strlen(name) > 0) {
            groups[groupCount].id = id ? id : (groupCount + 1);
            safeCopy(groups[groupCount].name, name, sizeof(groups[groupCount].name));
            safeCopy(groups[groupCount].description, desc, sizeof(groups[groupCount].description));
            groupCount++;
        }
    }
    fclose(fp);
    saveGroups();
}

void showGroups(void) {
    if (groupCount == 0) {
        printf("\n[!] No groups configured yet.\n");
        return;
    }

    printf("\n========================================================================\n");
    printf(" GROUPS (USER: %s)\n", getCurrentUser());
    printf("========================================================================\n");
    printf(" %-4s | %-16s | %-8s | %s\n", "No.", "Group Name", "Members", "Description");
    printf("========================================================================\n");

    for (int i = 0; i < groupCount; i++) {
        int members = getContactCountInGroup(groups[i].name);
        printf(" [%2d] | %-16s | %-8d | %s\n",
               i + 1,
               groups[i].name,
               members,
               groups[i].description);

        int printed = 0;
        for (int c = 0; c < contactCount; c++) {
            if (strcasecmp(contacts[c].group, groups[i].name) == 0) {
                if (!printed) {
                    printf("      Members: ");
                    printed = 1;
                } else {
                    printf(", ");
                }
                printf("%s (%s)", contacts[c].name, contacts[c].phone);
            }
        }
        if (printed) printf("\n");
        printf("------------------------------------------------------------------------\n");
    }
}

void createGroup(void) {
    if (groupCount >= MAX_GROUPS) {
        printf("\n[!] Group limit of %d reached.\n", MAX_GROUPS);
        return;
    }

    char name[MAX_GROUP_NAME_LEN];
    char desc[MAX_GROUP_DESC_LEN];

    printf("\n--- Create New Group (%s) ---\n", getCurrentUser());
    printf("Group Name: ");
    safeReadLine(name, sizeof(name));

    if (strlen(name) == 0) {
        printf("[!] Group name cannot be empty.\n");
        return;
    }

    if (groupExists(name)) {
        printf("[!] A group named '%s' already exists.\n", name);
        return;
    }

    printf("Description: ");
    safeReadLine(desc, sizeof(desc));

    groups[groupCount].id = groupCount + 1;
    safeCopy(groups[groupCount].name, name, sizeof(groups[groupCount].name));
    safeCopy(groups[groupCount].description, desc[0] ? desc : "No description", sizeof(groups[groupCount].description));

    groupCount++;
    saveGroups();
    printf("\n[+] Group '%s' created and saved successfully!\n", name);
}

void deleteGroup(void) {
    if (groupCount == 0) {
        printf("\n[!] No groups to delete.\n");
        return;
    }

    showGroups();
    printf("\nEnter Group Number to delete (1-%d, 0 to cancel): ", groupCount);
    int choice = 0;
    if (!safeReadInt(&choice) || choice < 1 || choice > groupCount) {
        printf("Operation cancelled.\n");
        return;
    }

    int idx = choice - 1;
    char deletedName[MAX_GROUP_NAME_LEN];
    safeCopy(deletedName, groups[idx].name, sizeof(deletedName));

    for (int i = 0; i < contactCount; i++) {
        if (strcasecmp(contacts[i].group, deletedName) == 0) {
            strcpy(contacts[i].group, "General");
        }
    }
    saveContacts();

    for (int i = idx; i < groupCount - 1; i++) {
        groups[i] = groups[i + 1];
    }
    groupCount--;
    saveGroups();

    printf("\n[-] Group '%s' deleted. Assigned contacts moved to 'General'.\n", deletedName);
}

void manageGroupsMenu(void) {
    int opt = 0;
    do {
        printf("\n--- Group Management (%s) ---\n", getCurrentUser());
        printf("1. View All Groups & Members\n");
        printf("2. Create New Group\n");
        printf("3. Delete a Group\n");
        printf("4. Back to Main Menu\n");
        printf("Choose: ");
        if (!safeReadInt(&opt)) continue;

        switch (opt) {
            case 1: showGroups(); break;
            case 2: createGroup(); break;
            case 3: deleteGroup(); break;
            case 4: return;
            default: printf("[!] Invalid option.\n"); break;
        }
    } while (opt != 4);
}
