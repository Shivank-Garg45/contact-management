#ifndef GROUPS_H
#define GROUPS_H

#include "contact.h"

typedef struct {
    char GroupName[20];
    int memberCount;
    Contact members[3];
} Group;

extern Group groups[10];
extern int groupCount;

void createGroup(void);
void showGroups(void);

#endif
