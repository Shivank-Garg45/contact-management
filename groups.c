#include <stdio.h>
#include <string.h>
#include "groups.h"

Group groups[10];
int groupCount = 0;

void createGroup(void) {
    if (groupCount >= 10) {
        puts("\n❌ Max group limit reached.");
        return;
    }

    printf("\n📁 New Group Name: ");
    scanf("%19s", groups[groupCount].GroupName);
    groups[groupCount].memberCount = 0;

    char choice = 'y';
    while (choice == 'y' && groups[groupCount].memberCount < 3) {
        char cname[20];
        printf("➕ Add Contact Name: ");
        scanf("%19s", cname);

        int found = 0;
        for (int i = 0; i < contactCount; i++) {
            if (!strcmp(contacts[i].ContactName, cname)) {
                groups[groupCount].members[groups[groupCount].memberCount++] = contacts[i];
                puts("✅ Contact added.");
                found = 1;
                break;
            }
        }

        if (!found) puts("❌ Contact not found.");

        if (groups[groupCount].memberCount < 3) {
            printf("Add another? (y/n): ");
            scanf(" %c", &choice);
        } else {
            puts("⚠️ Max 3 members per group.");
        }
    }

    groupCount++;
    puts("\n✅ Group created.");
}

void showGroups(void) {
    if (groupCount == 0) {
        puts("\n📭 No groups yet.");
        return;
    }

    puts("\n📚 Your Groups:");
    for (int i = 0; i < groupCount; i++) {
        printf("\n[%d] %s\n", i + 1, groups[i].GroupName);
        for (int j = 0; j < groups[i].memberCount; j++) {
            printf("   - %s | %s\n",
                   groups[i].members[j].ContactName,
                   groups[i].members[j].ContactTel);
        }
    }
}