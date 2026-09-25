#ifndef AUTH_H
#define AUTH_H

#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 64
#define HASH_STR_LEN 65

int registerUser(void);
int loginUser(void);
void logoutUser(void);
int isLoggedIn(void);
const char* getCurrentUser(void);
void hashPassword(const char *plain, char *outHex);

#endif // AUTH_H
