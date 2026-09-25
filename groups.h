#ifndef GROUPS_H
#define GROUPS_H

#define MAX_GROUPS 50
#define MAX_GROUP_NAME_LEN 32
#define MAX_GROUP_DESC_LEN 128

typedef struct {
    int id;
    char name[MAX_GROUP_NAME_LEN];
    char description[MAX_GROUP_DESC_LEN];
} Group;

extern Group groups[MAX_GROUPS];
extern int groupCount;

void loadGroups(void);
void saveGroups(void);
void clearGroups(void);
void createGroup(void);
void showGroups(void);
void deleteGroup(void);
void manageGroupsMenu(void);
int groupExists(const char *name);
int getContactCountInGroup(const char *groupName);

#endif // GROUPS_H
