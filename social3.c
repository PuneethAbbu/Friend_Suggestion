#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_USERS 100
#define MAX_INTERESTS 10
#define MAX_NAME_LENGTH 50
#define MAX_FRIENDS 100

typedef struct {
    char name[MAX_NAME_LENGTH];
    char interests[MAX_INTERESTS][MAX_NAME_LENGTH];
    int numInterests;
    int requestedBy[MAX_USERS];
    int numRequestedBy;
    int friends[MAX_FRIENDS];
    int numFriends;
} User;

User users[MAX_USERS];
int numUsers = 0;

int findUserIndex(const char *name) {
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(users[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void addUser(const char *name, const char interests[][MAX_NAME_LENGTH], int numInterests) {
    if (numUsers < MAX_USERS) {
        strcpy(users[numUsers].name, name);
        users[numUsers].numInterests = numInterests;
        for (int i = 0; i < numInterests; i++) {
            strcpy(users[numUsers].interests[i], interests[i]);
        }
        users[numUsers].numRequestedBy = 0;
        users[numUsers].numFriends = 0;
        numUsers++;
    } else {
        printf("Maximum number of users reached.\n");
    }
}

/* Modified function: Instead of replacing existing interests,
   it now appends new interests to the user's current list. */
void updateUserInterests(const char *name, const char newInterests[][MAX_NAME_LENGTH], int newInterestsCount) {
    int userIndex = findUserIndex(name);
    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }
    int currentInterests = users[userIndex].numInterests;
    if (currentInterests + newInterestsCount > MAX_INTERESTS) {
        printf("Cannot add interests: exceeds maximum allowed interests (%d).\n", MAX_INTERESTS);
        return;
    }
    for (int i = 0; i < newInterestsCount; i++) {
        strcpy(users[userIndex].interests[currentInterests + i], newInterests[i]);
    }
    users[userIndex].numInterests += newInterestsCount;
    printf("New interests added for user %s.\n", name);
}

int findMutualInterests(const User *user1, const User *user2) {
    int mutualInterests = 0;
    for (int i = 0; i < user1->numInterests; i++) {
        for (int j = 0; j < user2->numInterests; j++) {
            if (strcmp(user1->interests[i], user2->interests[j]) == 0) {
                mutualInterests++;
            }
        }
    }
    return mutualInterests;
}

int isFriend(int userIndex, int friendIndex) {
    if (userIndex < 0 || userIndex >= numUsers || friendIndex < 0 || friendIndex >= numUsers) {
        return 0;
    }
    for (int i = 0; i < users[userIndex].numFriends; i++) {
        if (users[userIndex].friends[i] == friendIndex) {
            return 1;
        }
    }
    return 0;
}

int isRequested(int userIndex, int friendIndex) {
    if (userIndex < 0 || userIndex >= numUsers || friendIndex < 0 || friendIndex >= numUsers) {
        return 0;
    }
    for (int i = 0; i < users[userIndex].numRequestedBy; i++) {
        if (users[userIndex].requestedBy[i] == friendIndex) {
            return 1;
        }
    }
    return 0;
}

void suggestFriends(const char *userName) {
    int userIndex = findUserIndex(userName);
    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }

    printf("Suggested Friends:\n");

    // Suggestions based on interests
    for (int i = 0; i < numUsers; i++) {
        if (i != userIndex && !isFriend(userIndex, i) && !isRequested(userIndex, i)) {
            int mutualInterests = findMutualInterests(&users[userIndex], &users[i]);
            if (mutualInterests > 0) {
                printf("- %s (Mutual Interests: %d)\n", users[i].name, mutualInterests);
            }
        }
    }

    // Friend-of-a-friend suggestions (ignoring interests)
    for (int i = 0; i < users[userIndex].numFriends; i++) {
        int friendIndex = users[userIndex].friends[i];
        for (int j = 0; j < users[friendIndex].numFriends; j++) {
            int friendOfFriendIndex = users[friendIndex].friends[j];
            if (friendOfFriendIndex != userIndex && !isFriend(userIndex, friendOfFriendIndex) && !isRequested(userIndex, friendOfFriendIndex)) {
                printf("- %s (Friend of Friend)\n", users[friendOfFriendIndex].name);
            }
        }
    }
}

void sendRequest(const char *senderName, const char *receiverName) {
    int senderIndex = findUserIndex(senderName);
    int receiverIndex = findUserIndex(receiverName);

    if (senderIndex == -1 || receiverIndex == -1) {
        printf("Sender or receiver not found.\n");
        return;
    }

    users[receiverIndex].requestedBy[users[receiverIndex].numRequestedBy] = senderIndex;
    users[receiverIndex].numRequestedBy++;
    printf("Friend request sent from %s to %s.\n", senderName, receiverName);
}

void acceptRequest(const char *userName) {
    int userIndex = findUserIndex(userName);
    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }

    if (users[userIndex].numRequestedBy == 0) {
        printf("No friend requests to accept.\n");
        return;
    }

    printf("Friend Requests for %s:\n", userName);
    for (int i = 0; i < users[userIndex].numRequestedBy; i++) {
        printf("%d. %s\n", i + 1, users[users[userIndex].requestedBy[i]].name);
    }

    int choice;
    printf("Enter the number of the request to accept (0 to cancel): ");
    scanf("%d", &choice);

    if (choice > 0 && choice <= users[userIndex].numRequestedBy) {
        int friendIndex = users[userIndex].requestedBy[choice - 1];
        printf("Friend request from %s accepted.\n", users[friendIndex].name);

        users[userIndex].friends[users[userIndex].numFriends++] = friendIndex;
        users[friendIndex].friends[users[friendIndex].numFriends++] = userIndex;

        int requestToRemove = users[userIndex].requestedBy[choice - 1];
        int newNumRequestedBy = 0;
        int tempRequestedBy[MAX_USERS];

        for (int i = 0; i < users[userIndex].numRequestedBy; i++) {
            if (users[userIndex].requestedBy[i] != requestToRemove) {
                tempRequestedBy[newNumRequestedBy++] = users[userIndex].requestedBy[i];
            }
        }

        users[userIndex].numRequestedBy = newNumRequestedBy;
        memcpy(users[userIndex].requestedBy, tempRequestedBy, sizeof(int) * newNumRequestedBy);
    } else if (choice != 0) {
        printf("Invalid choice.\n");
    }
}

void rejectRequest(const char *userName) {
    int userIndex = findUserIndex(userName);
    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }

    if (users[userIndex].numRequestedBy == 0) {
        printf("No friend requests to reject.\n");
        return;
    }

    printf("Friend Requests for %s:\n", userName);
    for (int i = 0; i < users[userIndex].numRequestedBy; i++) {
        printf("%d. %s\n", i + 1, users[users[userIndex].requestedBy[i]].name);
    }

    int choice;
    printf("Enter the number of the request to reject (0 to cancel): ");
    scanf("%d", &choice);

    if (choice > 0 && choice <= users[userIndex].numRequestedBy) {
        int requestToRemove = users[userIndex].requestedBy[choice - 1];
        printf("Friend request from %s rejected.\n", users[users[userIndex].requestedBy[choice - 1]].name);

        int newNumRequestedBy = 0;
        int tempRequestedBy[MAX_USERS];

        for (int i = 0; i < users[userIndex].numRequestedBy; i++) {
            if (users[userIndex].requestedBy[i] != requestToRemove) {
                tempRequestedBy[newNumRequestedBy++] = users[userIndex].requestedBy[i];
            }
        }

        users[userIndex].numRequestedBy = newNumRequestedBy;
        memcpy(users[userIndex].requestedBy, tempRequestedBy, sizeof(int) * newNumRequestedBy);
    } else if (choice != 0) {
        printf("Invalid choice.\n");
    }
}

void listUsers() {
    printf("List of Users:\n");
    for (int i = 0; i < numUsers; i++) {
        printf("- %s\n", users[i].name);
    }
}

void viewFriends(const char *userName) {
    int userIndex = findUserIndex(userName);
    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }

    if (users[userIndex].numFriends == 0) {
        printf("%s has no friends.\n", userName);
        return;
    }

    printf("Friends of %s:\n", userName);
    for (int i = 0; i < users[userIndex].numFriends; i++) {
        printf("- %s\n", users[users[userIndex].friends[i]].name);
    }
}

void trimWhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  // All spaces?
        return;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = 0;
}

void viewInterests(const char *userName) {
    char trimmedName[MAX_NAME_LENGTH];
    strcpy(trimmedName, userName);
    trimWhitespace(trimmedName);

    int userIndex = findUserIndex(trimmedName);
    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }

    if (users[userIndex].numInterests == 0) {
        printf("%s has no interests.\n", trimmedName);
        return;
    }

    printf("Interests of %s:\n", trimmedName);
    for (int i = 0; i < users[userIndex].numInterests; i++) {
        printf("- %s\n", users[userIndex].interests[i]);
    }
}

int main() {
    int choice;
    char name[MAX_NAME_LENGTH];
    char interests[MAX_INTERESTS][MAX_NAME_LENGTH];
    int numInterests;
    char senderName[MAX_NAME_LENGTH];
    char receiverName[MAX_NAME_LENGTH];

    while (1) {
        printf("\n1. Add User\n2. Suggest Friends\n3. Send Request\n4. Accept Request\n5. Reject Request\n6. List Users\n7. View Friends\n8. Update Interests\n9. View Interests\n10. EXIT\nEnter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter user name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                x: printf("Enter number of interests: ");
                scanf("%d", &numInterests);
                if(numInterests > MAX_INTERESTS || numInterests < 0) {
                    continue;
                    goto x;
                }
                getchar();

                for (int i = 0; i < numInterests; i++) {
                    printf("Enter interest %d: ", i + 1);
                    fgets(interests[i], sizeof(interests[i]), stdin);
                    interests[i][strcspn(interests[i], "\n")] = 0;
                }
                addUser(name, interests, numInterests);
                break;
            case 2:
                printf("Enter user name to suggest friends for: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                suggestFriends(name);
                break;
            case 3:
                printf("Enter sender name: ");
                fgets(senderName, sizeof(senderName), stdin);
                senderName[strcspn(senderName, "\n")] = 0;
                printf("Enter receiver name: ");
                fgets(receiverName, sizeof(receiverName), stdin);
                receiverName[strcspn(receiverName, "\n")] = 0;
                sendRequest(senderName, receiverName);
                break;
            case 4:
                printf("Enter user name to accept requests for: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                acceptRequest(name);
                break;
            case 5:
                printf("Enter user name to reject requests for: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                rejectRequest(name);
                break;
            case 6: 
                listUsers();
                break;
            case 7:
                printf("Enter user name to view friends: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                viewFriends(name);
                break;
            case 8:
                printf("Enter user name to update interests: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                x2: printf("Enter number of new interests to add: ");
                scanf("%d", &numInterests);
                if(numInterests > MAX_INTERESTS || numInterests < 0) {
                    continue;
                    goto x2;
                }
                getchar();

                for (int i = 0; i < numInterests; i++) {
                    printf("Enter new interest %d: ", i + 1);
                    fgets(interests[i], sizeof(interests[i]), stdin);
                    interests[i][strcspn(interests[i], "\n")] = 0;
                }
                updateUserInterests(name, interests, numInterests);
                break;
            case 9:
                printf("Enter user name to view interests: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                viewInterests(name);
                break;
            case 10:
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
