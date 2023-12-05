/*
 * @FileName: 
 * @Description: 
 * @Autor: Liujunjie/Aries-441
 * @StudentNumber: 521021911059
 * @Date: 2023-12-05 17:07:41
 * @E-mail: sjtu.liu.jj@gmail.com/sjtu.1518228705@sjtu.edu.cn
 * @LastEditTime: 2023-12-05 19:47:35
 */

#include <stdio.h>
#include <string.h>
#include </usr/include/mysql/mysql.h>

int main() {
    const char* user = "liu";
    const char* pass = "111";
    printf("ftpserver_check_user\n");

    MYSQL *con = mysql_init(NULL);
    if (con == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        return -1;
    }

    if (mysql_real_connect(con, "localhost", "root", "ljj0403!", "ftpusers", 0, NULL, 0) == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return -1;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT password FROM users WHERE username = '%s'", user);

    if (mysql_query(con, query)) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return -1;
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == NULL) {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    int auth = -1;
    if (row != NULL) {
        if (strcmp(pass, row[0]) == 0) {
            auth = 1;
        }
    }

    mysql_free_result(result);
    mysql_close(con);

    return auth;
}
