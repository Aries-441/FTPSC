/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-12-03 23:49:46
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-04 13:35:02
 */
#include <stdio.h>
#include <string.h>

#define MAXSIZE 1024

// Check if a user has a specific permission
int user_has_permission(char *UserName, char *PermissionList) {
    char *token = strtok(PermissionList, ",");
    while (token != NULL) {
        if (strcmp(UserName, token) == 0) {
            return 1; // User has permission
        }
        token = strtok(NULL, ",");
    }
    return 0; // User does not have permission
}

// Check permissions for a user in an ACL file
int check_permissions(char *UserName, char *PermissionType, char *aclFilePath) {
    FILE *file = fopen(aclFilePath, "r");
    if (file == NULL) {
        printf("Could not open file %s\n", aclFilePath);
        return 0; // Could not open file
    }

    char line[MAXSIZE];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // Check if line contains permission type
        char *currentPermissionType = strtok(line, ":");
        char *permissionList = strtok(NULL, ":");

        if (strcmp(PermissionType, currentPermissionType) == 0 &&
            user_has_permission(UserName, permissionList)) {
            fclose(file);
            return 1; // User has permission
        }
    }

    fclose(file);
    return 0; // User does not have permission
}

int main() {
    char UserName[MAXSIZE] = "ljj";
    char PermissionType[MAXSIZE] = "DELE";
    if (check_permissions(UserName, PermissionType, "ctl.acl")) {
        printf("%s has %s permission\n", UserName, PermissionType);
    } else {
        printf("%s does not have %s permission\n", UserName, PermissionType);
    }
    return 0;
}
