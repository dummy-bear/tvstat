/*
 *  reading settings
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mysql/mysql.h>

#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>


int errlev=3;

void myerr (int level, char *s)
{
        if (level<=errlev) fprintf(stderr, "%s\n", s);
        if (!level) {
                //close_device();
                exit(EXIT_FAILURE);
        }
}

int getsetint(MYSQL *mysql,char *name)
{
        MYSQL_RES *res;
        MYSQL_ROW row;

        char str[1024];

        snprintf (str,1023,"SELECT value FROM settings WHERE name=\"%s\";",name);
        if (mysql_query(mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
        res=mysql_store_result(mysql);
        row=mysql_fetch_row(res);
        return atoi(row[0]);
}

char *getsetstr(MYSQL *mysql,char *name, char *val)
{
        MYSQL_RES *res;
        MYSQL_ROW row;
        char str[1024];

        snprintf (str,1023,"SELECT value FROM settings WHERE name=\"%s\";",name);
        if (mysql_query(mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
        res=mysql_store_result(mysql);
        row=mysql_fetch_row(res);
        strcpy(val,row[0]);
        myerr (3,str);
        return val;
}


