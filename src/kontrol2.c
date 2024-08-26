/*
 *  Daily test module
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mysql/mysql.h>

#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>

//константы доступа к бд
char mshost[]="localhost";
char msuser[]="root";
char mspass[]="dbpass";
char msbase[]="tvstat";

main(int argc, char **argv)
{
	MYSQL mysql;
	MYSQL_RES *res, *mres;
	MYSQL_ROW row, mrow;
	
	char mailstr[8024],str[8024],str1[8000];
	int i,j,k;
	FILE *f;
	int izmer;

	int region;
	int nrkp;
	char mypasswd[256];
	char serv[]="10.52.2.9";
	float korrl;
	//int myqual=100;

	int sobytie,ass[4];
	float kor1[4];
	char kogda[128],cityname[255],profcomment[255],smtp[255],sender[255],mailpass[255],receipt[255],mailfoot[4000];

	mysql_init (&mysql);
	if (!mysql_real_connect(&mysql,mshost,msuser,mspass,msbase,0,0,0)) {fprintf (stderr,"error connecting mysql\n");exit(1);}

	if (mysql_query(&mysql,"SET NAMES 'utf8'")) {printf ("mysql select error\n");return;}

	region=getsetint(&mysql,"region");
	nrkp=getsetint(&mysql,"nrkp");

	getsetstr(&mysql,"addpass",mypasswd);
	getsetstr(&mysql,"cityname",cityname);
	getsetstr(&mysql,"mail footer",mailfoot);
	getsetstr(&mysql,"smtp",smtp);
	getsetstr(&mysql,"sender",sender);
	getsetstr(&mysql,"mail password",mailpass);
	getsetstr(&mysql,"mail receipt",receipt);

	snprintf (str,1023,"SELECT count(ID) FROM meas WHERE ID=0 AND kogda>DATE_SUB(now(),Interval 1 day);");
	myerr (3,str);
	if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
	res=mysql_store_result(&mysql);
	row=mysql_fetch_row(res);
	izmer=atoi(row[0]);

	snprintf (str1,8000,"<HTML>\nСистема tvstat в %s функционирует нормально. За последние 24 часа произведено %d сканирований. ",cityname,izmer);

	snprintf (str,1023,"SELECT kogda,description FROM log WHERE kogda>DATE_SUB(now(),Interval 1 day);");
        myerr (3,str);
        if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
        res=mysql_store_result(&mysql);

	if (mysql_num_rows(res))
	{
		snprintf (str,8000,"%s<P>\n События за последние 24 часа: <BR>\n",str1);
		for (i=0;i<mysql_num_rows(res);i++)
	        {
			row=mysql_fetch_row(res);
			snprintf (str1,8000,"%s %s %s <BR>\n",str,row[0],row[1]);
			strncpy (str,str1,8000);
		}
	}
	else snprintf (str,8000,"%s<P>\n Событий за последние 24 часа не было. <BR>\n",str1);
	
	k=0;
        for (j=0;j<=strlen(receipt);j++)
        {
        	if ((receipt[j]!=' ')&&(receipt[j]!='\0')) str1[k++]=receipt[j];
                else
                {
                	str1[k]='\0';

                        f=fopen ("mailk.txt","w");
                        fprintf (f,"MIME-Version: 1.0\nContent-type: text/html;charset=\"UTF-8\"\nFrom: %d-%s <%s>\nTo:%s\nSubject: Контрольное письмо tvstat.\n\n%s %s </HTML>",region,sender,sender,str1,str,mailfoot);
                        fclose (f);
			myerr (3,smtp);

                        mailk (smtp,sender,mailpass,str1);
                        k=0;
        	}
	}
        myerr (3,"mail sent");
	mysql_close(&mysql);

}
