#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include "contact.h"
#include "groups.h"
#include "auth.h"

Contact contacts[MAX_CONTACTS];
int contactCount = 0;

static void safeCopy(char *dst, const char *src, size_t dstSize) {
    if (!dst || dstSize == 0) return;
    if (!src) { dst[0] = '\0'; return; }
    size_t len = strlen(src);
    if (len >= dstSize) len = dstSize - 1;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

void safeReadLine(char *buffer, int maxLen) {
    if (!buffer || maxLen <= 0) return;
    if (fgets(buffer, maxLen, stdin) != NULL) {
        buffer[strcspn(buffer, "\r\n")] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

int safeReadInt(int *outVal) {
    char buf[64];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
    char *endptr;
    long val = strtol(buf, &endptr, 10);
    if (endptr == buf) return 0;
    if (outVal) *outVal = (int)val;
    return 1;
}

static void getCurrentTimestamp(char *buffer, size_t maxLen) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t) {
        strftime(buffer, maxLen, "%Y-%m-%d %H:%M", t);
    } else {
        safeCopy(buffer, "2026-09-25", maxLen);
    }
}

static int containsIgnoreCase(const char *haystack, const char *needle) {
    if (!haystack || !needle) return 0;
    if (!*needle) return 1;

    size_t nlen = strlen(needle);
    size_t hlen = strlen(haystack);
    if (nlen > hlen) return 0;

    for (size_t i = 0; i <= hlen - nlen; i++) {
        size_t j = 0;
        while (j < nlen && tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j])) {
            j++;
        }
        if (j == nlen) return 1;
    }
    return 0;
}

static void getUserContactsFilePath(char *dest, size_t maxLen) {
    const char *user = getCurrentUser();
    if (!user || strlen(user) == 0 || strcmp(user, "Guest") == 0) {
        snprintf(dest, maxLen, "contacts.txt");
    } else {
        snprintf(dest, maxLen, "data/%s_contacts.txt", user);
    }
}

void clearContacts(void) {
    contactCount = 0;
    memset(contacts, 0, sizeof(contacts));
}

void saveContacts(void) {
    char filepath[256];
    getUserContactsFilePath(filepath, sizeof(filepath));

    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        printf("[!] Error: Could not open %s for saving.\n", filepath);
        return;
    }

    fprintf(fp, "%d\n", contactCount);
    for (int i = 0; i < contactCount; i++) {
        fprintf(fp, "%d|%s|%s|%s|%s|%s|%s|%s|%d|%s|%s\n",
                contacts[i].id,
                contacts[i].name,
                contacts[i].phone,
                contacts[i].workPhone,
                contacts[i].email,
                contacts[i].address,
                contacts[i].company,
                contacts[i].group[0] ? contacts[i].group : "General",
                contacts[i].isFavorite,
                contacts[i].notes[0] ? contacts[i].notes : "-",
                contacts[i].createdAt[0] ? contacts[i].createdAt : "2026-09-25");
    }
    fclose(fp);
}

void loadContacts(void) {
    clearContacts();
    char filepath[256];
    getUserContactsFilePath(filepath, sizeof(filepath));

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        // Fallback for legacy contacts.txt if divyanshsood22
        if (strcmp(getCurrentUser(), "divyanshsood22") == 0) {
            fp = fopen("contacts.txt", "r");
        }
    }
    if (!fp) return;

    char line[512];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return;
    }

    while (fgets(line, sizeof(line), fp) && contactCount < MAX_CONTACTS) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        if (strchr(line, '|')) {
            Contact *c = &contacts[contactCount];
            memset(c, 0, sizeof(Contact));

            char *token = strtok(line, "|");
            if (token) c->id = atoi(token);

            token = strtok(NULL, "|");
            if (token) safeCopy(c->name, token, sizeof(c->name));

            token = strtok(NULL, "|");
            if (token) safeCopy(c->phone, token, sizeof(c->phone));

            token = strtok(NULL, "|");
            if (token) safeCopy(c->workPhone, token, sizeof(c->workPhone));

            token = strtok(NULL, "|");
            if (token) safeCopy(c->email, token, sizeof(c->email));

            token = strtok(NULL, "|");
            if (token) safeCopy(c->address, token, sizeof(c->address));

            token = strtok(NULL, "|");
            if (token) safeCopy(c->company, token, sizeof(c->company));

            token = strtok(NULL, "|");
            if (token) safeCopy(c->group, token, sizeof(c->group));

            token = strtok(NULL, "|");
            if (token) c->isFavorite = atoi(token);

            token = strtok(NULL, "|");
            if (token && strcmp(token, "-") != 0) safeCopy(c->notes, token, sizeof(c->notes));

            token = strtok(NULL, "|");
            if (token) safeCopy(c->createdAt, token, sizeof(c->createdAt));

            if (c->id <= 0) c->id = contactCount + 1;
            if (c->group[0] == '\0') safeCopy(c->group, "General", sizeof(c->group));

            contactCount++;
        } else {
            Contact *c = &contacts[contactCount];
            memset(c, 0, sizeof(Contact));
            c->id = contactCount + 1;
            if (sscanf(line, "%63s %31s %63s %127s", c->name, c->phone, c->email, c->address) >= 2) {
                safeCopy(c->group, "General", sizeof(c->group));
                c->isFavorite = 0;
                c->notes[0] = '\0';
                c->company[0] = '\0';
                c->workPhone[0] = '\0';
                safeCopy(c->createdAt, "Legacy Record", sizeof(c->createdAt));
                contactCount++;
            }
        }
    }
    fclose(fp);
    // Ensure saved to user's dedicated file
    saveContacts();
}

void addContact(void) {
    if (contactCount >= MAX_CONTACTS) {
        printf("\n[!] Limit reached. Cannot add more than %d contacts.\n", MAX_CONTACTS);
        return;
    }

    Contact *c = &contacts[contactCount];
    memset(c, 0, sizeof(Contact));

    printf("\n========================================\n");
    printf("     ADD NEW CONTACT (%s)\n", getCurrentUser());
    printf("========================================\n");

    printf("Full Name*: ");
    safeReadLine(c->name, sizeof(c->name));
    if (strlen(c->name) == 0) {
        printf("[!] Name is mandatory. Contact not added.\n");
        return;
    }

    printf("Primary Phone*: ");
    safeReadLine(c->phone, sizeof(c->phone));
    if (strlen(c->phone) == 0) {
        printf("[!] Phone number is mandatory. Contact not added.\n");
        return;
    }

    printf("Work Phone (optional): ");
    safeReadLine(c->workPhone, sizeof(c->workPhone));

    printf("Email Address: ");
    safeReadLine(c->email, sizeof(c->email));

    printf("Physical Address: ");
    safeReadLine(c->address, sizeof(c->address));

    printf("Company / Organization: ");
    safeReadLine(c->company, sizeof(c->company));

    printf("Available Groups:\n");
    for (int i = 0; i < groupCount; i++) {
        printf("  [%d] %s\n", i + 1, groups[i].name);
    }
    printf("Assign Group (enter name or number, default 'General'): ");
    char groupInput[MAX_GROUP_LEN];
    safeReadLine(groupInput, sizeof(groupInput));
    if (strlen(groupInput) == 0) {
        safeCopy(c->group, "General", sizeof(c->group));
    } else {
        int gIdx = atoi(groupInput);
        if (gIdx >= 1 && gIdx <= groupCount) {
            safeCopy(c->group, groups[gIdx - 1].name, sizeof(c->group));
        } else {
            safeCopy(c->group, groupInput, sizeof(c->group));
        }
    }

    printf("Mark as Favorite? (y/n, default n): ");
    char favBuf[16];
    safeReadLine(favBuf, sizeof(favBuf));
    c->isFavorite = (favBuf[0] == 'y' || favBuf[0] == 'Y') ? 1 : 0;

    printf("Notes / Tags: ");
    safeReadLine(c->notes, sizeof(c->notes));

    c->id = contactCount + 1;
    getCurrentTimestamp(c->createdAt, sizeof(c->createdAt));

    contactCount++;
    saveContacts();

    printf("\n[+] Contact '%s' added successfully with ID #%d!\n", c->name, c->id);
}

void showContacts(void) {
    if (contactCount == 0) {
        printf("\n[!] No contacts found for user '%s'. Select option 2 to add contacts.\n", getCurrentUser());
        return;
    }

    printf("\n=======================================================================================================\n");
    printf(" [USER: %s] CONTACTS LIST\n", getCurrentUser());
    printf("=======================================================================================================\n");
    printf(" %-4s | %-3s | %-20s | %-15s | %-22s | %-12s | %-12s\n",
           "ID", "Fav", "Name", "Phone", "Email", "Group", "Company");
    printf("=======================================================================================================\n");

    for (int i = 0; i < contactCount; i++) {
        printf(" #%-3d | %-3s | %-20.20s | %-15.15s | %-22.22s | %-12.12s | %-12.12s\n",
               contacts[i].id,
               contacts[i].isFavorite ? "[*]" : "   ",
               contacts[i].name,
               contacts[i].phone,
               contacts[i].email[0] ? contacts[i].email : "-",
               contacts[i].group[0] ? contacts[i].group : "General",
               contacts[i].company[0] ? contacts[i].company : "-");
    }
    printf("=======================================================================================================\n");
    printf(" Total: %d contacts\n", contactCount);
}

void searchContact(void) {
    if (contactCount == 0) {
        printf("\n[!] Contact list is empty.\n");
        return;
    }

    char query[64];
    printf("\nSearch query (Name, Phone, Email, Company, or Group): ");
    safeReadLine(query, sizeof(query));

    if (strlen(query) == 0) {
        printf("[!] Empty query.\n");
        return;
    }

    int matchCount = 0;
    printf("\n--- Search Results for '%s' ---\n", query);

    for (int i = 0; i < contactCount; i++) {
        if (containsIgnoreCase(contacts[i].name, query) ||
            containsIgnoreCase(contacts[i].phone, query) ||
            containsIgnoreCase(contacts[i].workPhone, query) ||
            containsIgnoreCase(contacts[i].email, query) ||
            containsIgnoreCase(contacts[i].company, query) ||
            containsIgnoreCase(contacts[i].group, query)) {

            matchCount++;
            printf("\n[%d] %s %s\n", contacts[i].id, contacts[i].name, contacts[i].isFavorite ? "★ (Favorite)" : "");
            printf("    Phone:      %s", contacts[i].phone);
            if (contacts[i].workPhone[0]) printf(" | Work: %s", contacts[i].workPhone);
            printf("\n");
            if (contacts[i].email[0])   printf("    Email:      %s\n", contacts[i].email);
            if (contacts[i].company[0]) printf("    Company:    %s\n", contacts[i].company);
            if (contacts[i].address[0]) printf("    Address:    %s\n", contacts[i].address);
            if (contacts[i].group[0])   printf("    Group:      %s\n", contacts[i].group);
            if (contacts[i].notes[0])   printf("    Notes:      %s\n", contacts[i].notes);
            if (contacts[i].createdAt[0]) printf("    Created:    %s\n", contacts[i].createdAt);
            printf("--------------------------------------------------\n");
        }
    }

    if (matchCount == 0) {
        printf("No contacts matched your search query.\n");
    } else {
        printf("Found %d matching contact(s).\n", matchCount);
    }
}

void updateContact(void) {
    if (contactCount == 0) {
        printf("\n[!] No contacts to update.\n");
        return;
    }

    printf("\nEnter Contact ID or search name to update: ");
    char target[64];
    safeReadLine(target, sizeof(target));

    int targetIdx = -1;
    int targetId = atoi(target);

    for (int i = 0; i < contactCount; i++) {
        if ((targetId > 0 && contacts[i].id == targetId) ||
            strcasecmp(contacts[i].name, target) == 0 ||
            strcmp(contacts[i].phone, target) == 0) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx == -1) {
        printf("[!] Contact not found.\n");
        return;
    }

    Contact *c = &contacts[targetIdx];
    printf("\nEditing Contact: %s (Press Enter to keep existing value)\n", c->name);

    char buf[128];

    printf("Name [%s]: ", c->name);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->name, buf, sizeof(c->name));

    printf("Phone [%s]: ", c->phone);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->phone, buf, sizeof(c->phone));

    printf("Work Phone [%s]: ", c->workPhone);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->workPhone, buf, sizeof(c->workPhone));

    printf("Email [%s]: ", c->email);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->email, buf, sizeof(c->email));

    printf("Address [%s]: ", c->address);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->address, buf, sizeof(c->address));

    printf("Company [%s]: ", c->company);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->company, buf, sizeof(c->company));

    printf("Group [%s]: ", c->group);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->group, buf, sizeof(c->group));

    printf("Notes [%s]: ", c->notes);
    safeReadLine(buf, sizeof(buf));
    if (strlen(buf) > 0) safeCopy(c->notes, buf, sizeof(c->notes));

    saveContacts();
    printf("\n[+] Contact updated successfully!\n");
}

void deleteContact(void) {
    if (contactCount == 0) {
        printf("\n[!] No contacts to delete.\n");
        return;
    }

    printf("\nEnter Contact ID or Name to delete: ");
    char target[64];
    safeReadLine(target, sizeof(target));

    int targetIdx = -1;
    int targetId = atoi(target);

    for (int i = 0; i < contactCount; i++) {
        if ((targetId > 0 && contacts[i].id == targetId) ||
            strcasecmp(contacts[i].name, target) == 0 ||
            strcmp(contacts[i].phone, target) == 0) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx == -1) {
        printf("[!] Contact not found.\n");
        return;
    }

    printf("Are you sure you want to delete '%s' (#%d)? (y/n): ",
           contacts[targetIdx].name, contacts[targetIdx].id);
    char confirm[16];
    safeReadLine(confirm, sizeof(confirm));
    if (confirm[0] != 'y' && confirm[0] != 'Y') {
        printf("Deletion cancelled.\n");
        return;
    }

    char deletedName[64];
    safeCopy(deletedName, contacts[targetIdx].name, sizeof(deletedName));

    for (int j = targetIdx; j < contactCount - 1; j++) {
        contacts[j] = contacts[j + 1];
    }
    contactCount--;
    saveContacts();

    printf("\n[-] Contact '%s' deleted successfully.\n", deletedName);
}

void toggleFavorite(void) {
    if (contactCount == 0) {
        printf("\n[!] No contacts available.\n");
        return;
    }

    printf("\nEnter Contact ID or Name to toggle favorite: ");
    char target[64];
    safeReadLine(target, sizeof(target));

    int targetIdx = -1;
    int targetId = atoi(target);

    for (int i = 0; i < contactCount; i++) {
        if ((targetId > 0 && contacts[i].id == targetId) ||
            strcasecmp(contacts[i].name, target) == 0) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx == -1) {
        printf("[!] Contact not found.\n");
        return;
    }

    contacts[targetIdx].isFavorite = !contacts[targetIdx].isFavorite;
    saveContacts();

    printf("\n[★] Contact '%s' is now %s!\n",
           contacts[targetIdx].name,
           contacts[targetIdx].isFavorite ? "MARKED AS FAVORITE" : "UNMARKED FROM FAVORITES");
}

void showFavorites(void) {
    int favs = 0;
    printf("\n=========================================================================\n");
    printf("            FAVORITE CONTACTS ★ (USER: %s)\n", getCurrentUser());
    printf("=========================================================================\n");
    for (int i = 0; i < contactCount; i++) {
        if (contacts[i].isFavorite) {
            favs++;
            printf(" #%-3d | %-20s | %-15s | %-12s\n",
                   contacts[i].id, contacts[i].name, contacts[i].phone, contacts[i].group);
        }
    }
    if (favs == 0) {
        printf(" No favorite contacts starred yet.\n");
    }
    printf("=========================================================================\n");
}

static int cmpNameAZ(const void *a, const void *b) {
    return strcasecmp(((const Contact*)a)->name, ((const Contact*)b)->name);
}

static int cmpNameZA(const void *a, const void *b) {
    return strcasecmp(((const Contact*)b)->name, ((const Contact*)a)->name);
}

static int cmpCompany(const void *a, const void *b) {
    return strcasecmp(((const Contact*)a)->company, ((const Contact*)b)->company);
}

static int cmpIdDesc(const void *a, const void *b) {
    return ((const Contact*)b)->id - ((const Contact*)a)->id;
}

void sortContacts(void) {
    if (contactCount < 2) {
        printf("\n[!] Not enough contacts to sort.\n");
        return;
    }

    printf("\n--- Sort Contacts ---\n");
    printf("1. By Name (A -> Z)\n");
    printf("2. By Name (Z -> A)\n");
    printf("3. By Company\n");
    printf("4. By Recently Added (Newest First)\n");
    printf("Select sort order: ");

    int choice = 0;
    if (!safeReadInt(&choice)) return;

    switch (choice) {
        case 1: qsort(contacts, contactCount, sizeof(Contact), cmpNameAZ); break;
        case 2: qsort(contacts, contactCount, sizeof(Contact), cmpNameZA); break;
        case 3: qsort(contacts, contactCount, sizeof(Contact), cmpCompany); break;
        case 4: qsort(contacts, contactCount, sizeof(Contact), cmpIdDesc); break;
        default: printf("[!] Invalid sort option.\n"); return;
    }

    saveContacts();
    printf("\n[+] Contacts sorted successfully!\n");
    showContacts();
}

void exportContactsCSV(void) {
    if (contactCount == 0) {
        printf("\n[!] No contacts to export.\n");
        return;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "%s_contacts_export.csv", getCurrentUser());

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("[!] Could not open %s for export.\n", filename);
        return;
    }

    fprintf(fp, "ID,Name,Phone,WorkPhone,Email,Address,Company,Group,IsFavorite,Notes,CreatedAt\n");
    for (int i = 0; i < contactCount; i++) {
        fprintf(fp, "%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d,\"%s\",\"%s\"\n",
                contacts[i].id,
                contacts[i].name,
                contacts[i].phone,
                contacts[i].workPhone,
                contacts[i].email,
                contacts[i].address,
                contacts[i].company,
                contacts[i].group,
                contacts[i].isFavorite,
                contacts[i].notes,
                contacts[i].createdAt);
    }
    fclose(fp);
    printf("\n[✓] Exported %d contacts to CSV: %s\n", contactCount, filename);
}

void exportContactsVCF(void) {
    if (contactCount == 0) {
        printf("\n[!] No contacts to export.\n");
        return;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "%s_contacts_export.vcf", getCurrentUser());

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("[!] Could not open %s for export.\n", filename);
        return;
    }

    for (int i = 0; i < contactCount; i++) {
        fprintf(fp, "BEGIN:VCARD\nVERSION:3.0\n");
        fprintf(fp, "FN:%s\n", contacts[i].name);
        fprintf(fp, "TEL;TYPE=CELL:%s\n", contacts[i].phone);
        if (contacts[i].workPhone[0]) fprintf(fp, "TEL;TYPE=WORK:%s\n", contacts[i].workPhone);
        if (contacts[i].email[0])     fprintf(fp, "EMAIL;TYPE=INTERNET:%s\n", contacts[i].email);
        if (contacts[i].company[0])   fprintf(fp, "ORG:%s\n", contacts[i].company);
        if (contacts[i].address[0])   fprintf(fp, "ADR:;;%s;;;;\n", contacts[i].address);
        if (contacts[i].notes[0])     fprintf(fp, "NOTE:%s\n", contacts[i].notes);
        if (contacts[i].group[0])     fprintf(fp, "CATEGORIES:%s\n", contacts[i].group);
        fprintf(fp, "END:VCARD\n\n");
    }
    fclose(fp);
    printf("\n[✓] Exported %d contacts to universal vCard (VCF): %s\n", contactCount, filename);
}

void importContactsCSV(void) {
    printf("\nEnter CSV file path to import: ");
    char path[128];
    safeReadLine(path, sizeof(path));
    if (strlen(path) == 0) {
        snprintf(path, sizeof(path), "%s_contacts_export.csv", getCurrentUser());
    }

    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("[!] Error: Could not open '%s'.\n", path);
        return;
    }

    char line[512];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return;
    }

    int imported = 0;
    while (fgets(line, sizeof(line), fp) && contactCount < MAX_CONTACTS) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) < 3) continue;

        Contact *c = &contacts[contactCount];
        memset(c, 0, sizeof(Contact));

        char *ptr = line;
        char fields[11][128];
        int fieldIdx = 0;

        while (*ptr && fieldIdx < 11) {
            if (*ptr == '"') {
                ptr++;
                int k = 0;
                while (*ptr && *ptr != '"' && k < 127) {
                    fields[fieldIdx][k++] = *ptr++;
                }
                fields[fieldIdx][k] = '\0';
                if (*ptr == '"') ptr++;
                if (*ptr == ',') ptr++;
            } else {
                int k = 0;
                while (*ptr && *ptr != ',' && k < 127) {
                    fields[fieldIdx][k++] = *ptr++;
                }
                fields[fieldIdx][k] = '\0';
                if (*ptr == ',') ptr++;
            }
            fieldIdx++;
        }

        if (fieldIdx >= 3 && strlen(fields[1]) > 0) {
            c->id = contactCount + 1;
            safeCopy(c->name, fields[1], sizeof(c->name));
            safeCopy(c->phone, fields[2], sizeof(c->phone));
            if (fieldIdx > 3) safeCopy(c->workPhone, fields[3], sizeof(c->workPhone));
            if (fieldIdx > 4) safeCopy(c->email, fields[4], sizeof(c->email));
            if (fieldIdx > 5) safeCopy(c->address, fields[5], sizeof(c->address));
            if (fieldIdx > 6) safeCopy(c->company, fields[6], sizeof(c->company));
            if (fieldIdx > 7) safeCopy(c->group, fields[7], sizeof(c->group));
            if (fieldIdx > 8) c->isFavorite = atoi(fields[8]);
            if (fieldIdx > 9) safeCopy(c->notes, fields[9], sizeof(c->notes));
            if (fieldIdx > 10) safeCopy(c->createdAt, fields[10], sizeof(c->createdAt));
            else getCurrentTimestamp(c->createdAt, sizeof(c->createdAt));

            if (c->group[0] == '\0') safeCopy(c->group, "General", sizeof(c->group));
            contactCount++;
            imported++;
        }
    }
    fclose(fp);
    saveContacts();
    printf("\n[+] Successfully imported %d contacts for user '%s'!\n", imported, getCurrentUser());
}

void showAnalytics(void) {
    int favCount = 0;
    int hasEmail = 0;
    int hasAddress = 0;
    int hasCompany = 0;

    for (int i = 0; i < contactCount; i++) {
        if (contacts[i].isFavorite) favCount++;
        if (contacts[i].email[0]) hasEmail++;
        if (contacts[i].address[0]) hasAddress++;
        if (contacts[i].company[0]) hasCompany++;
    }

    printf("\n==================================================\n");
    printf("    CONTACT ANALYTICS (USER: %s)\n", getCurrentUser());
    printf("==================================================\n");
    printf(" Total Contacts Stored:     %d / %d\n", contactCount, MAX_CONTACTS);
    printf(" Starred Favorites:         %d (%.1f%%)\n", favCount, contactCount ? (favCount * 100.0 / contactCount) : 0.0);
    printf(" Contacts with Email:       %d (%.1f%%)\n", hasEmail, contactCount ? (hasEmail * 100.0 / contactCount) : 0.0);
    printf(" Contacts with Address:     %d (%.1f%%)\n", hasAddress, contactCount ? (hasAddress * 100.0 / contactCount) : 0.0);
    printf(" Contacts with Company:     %d (%.1f%%)\n", hasCompany, contactCount ? (hasCompany * 100.0 / contactCount) : 0.0);
    printf(" Total Groups Configured:   %d\n", groupCount);
    printf("--------------------------------------------------\n");
    printf(" Group Breakdown:\n");
    for (int g = 0; g < groupCount; g++) {
        int count = getContactCountInGroup(groups[g].name);
        printf("   - %-16s: %d contact(s)\n", groups[g].name, count);
    }
    printf("==================================================\n");
}
