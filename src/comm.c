/*
 *  Communication module
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

sendwebdata(int region, int nrkp,char *mypasswd, char *serv, int myqual, int id,int kanal, int avs, int korrel, char *tim, char *file)
{
        char str[2024], tim1[128];
        int sock,a;
        struct sockaddr_in addr;
        unsigned long int addr1;
        int i;

        addr1=10*256*256*256+52*256*256+2*256+9;

        sock=socket(AF_INET, SOCK_STREAM, 0);

        addr.sin_family=AF_INET;
        addr.sin_port=htons(80);
        addr.sin_addr.s_addr=htonl(addr1);
        connect (sock,(struct sockaddr *) &addr, sizeof (addr));

        for (i=0,a=0;i<strlen(tim);i++)
        {
                if (tim[i]==' ') {strcpy((char*)(tim1+a),"%20");a+=3;}
                else tim1[a++]=tim[i];
        }
        tim1[a]='\0';
        sprintf (str,"GET /tv/updmeas.php?ID=%d&region=%d&nrkp=%d&passwd=%s&kogda=%s&kanal=%d&avs=%d&quality=%d&korrel=%d&file=%s\nHost: %s\nUser-Agent: Tormozilla/5.0 (X11; U; Linux 8086; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\nAccept: text/html\nConnection: close\n\n",id,region,nrkp,mypasswd, tim1,kanal,avs,myqual,korrel,file,serv);
        send (sock,str,strlen(str),0);


        recv (sock,str,1023,0);

}

main(int argc, char **argv)
{
	MYSQL mysql;
	MYSQL_RES *res, *mres;
	MYSQL_ROW row, mrow;
	
	char str[8024],str1[8000];
	int i,j,k;
	FILE *f;


int region;
int nrkp;
char mypasswd[256];
char serv[]="10.52.2.9";
float korrl;

	int sobytie,ass[4];
	float kor1[4];
	char kogda[128],cityname[255],profcomment[255],smtp[128],sender[255],mailpass[255],receipt[255],mailfoot[4000];

	mysql_init (&mysql);
	if (!mysql_real_connect(&mysql,mshost,msuser,mspass,msbase,0,0,0)) {fprintf (stderr,"error connecting mysql\n");exit(1);}

	if (mysql_query(&mysql,"SET NAMES 'utf8'")) {printf ("mysql select error\n");return;}

	snprintf (str,1023,"UPDATE meas SET ss=1 WHERE ss=0;");
	if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}

	fprintf (stderr,"ss=1\n");

	region=getsetint(&mysql,"region");
	nrkp=getsetint(&mysql,"nrkp");

	getsetstr(&mysql,"addpass",mypasswd);
	getsetstr(&mysql,"cityname",cityname);
	getsetstr(&mysql,"mail footer",mailfoot);
	getsetstr(&mysql,"smtp",smtp);
	getsetstr(&mysql,"sender",sender);
	getsetstr(&mysql,"mail password",mailpass);
	getsetstr(&mysql,"mail receipt",receipt);

	printf ("%d-%d-%s\n",region,nrkp,mypasswd);


	snprintf (str,1023,"SELECT * FROM tvlist ORDER BY ID;");
	myerr (3,str);
	if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
	res=mysql_store_result(&mysql);
	for (i=0;i<mysql_num_rows(res);i++)
        {
		sobytie=0;
		row=mysql_fetch_row(res);
		snprintf (str,1023,"SELECT avs,korr,kogda FROM meas WHERE ID=%s ORDER BY kogda DESC limit 0,3;",row[0]);
		myerr(3,str);
		if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
	        mres=mysql_store_result(&mysql);


                for (j=0;j<3;j++)
                {
                       mrow=mysql_fetch_row(mres);
			if (j==0) strncpy(kogda,mrow[2],128);
                       ass[j]=atoi(mrow[0]);
                       kor1[j]=atof(mrow[1]);

			snprintf (str,1023,"%d %g %s\n",ass[j],kor1[j],kogda);
			myerr(3,str);
                }

                if ((ass[0]<50)&&(ass[1]<50)&&(ass[2]>50)) sobytie=101; 	//пропал сигнал
                else if ((ass[0]>50)&&(ass[1]<50)&&(ass[2]<50)) sobytie=102;  //появился сигнал
                else if ((kor1[0]<1e8)&&(kor1[1]>1e8)) sobytie=103; 	//замерло
                else if ((kor1[0]>1e8)&&(kor1[1]<1e8)) sobytie=104;	//отмерло

		if (sobytie&&(row[4][0]!='O')) sobytie+=20;

		if (sobytie)
		{
			snprintf (str,1023,"SELECT comment FROM profilaktika WHERE kanal=%s AND start<'%s' AND stop>'%s';",row[0],kogda,kogda);
			myerr(3,str);
			if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
	                mres=mysql_store_result(&mysql);
			if (mysql_num_rows(mres)) 
			{
				sobytie+=10;
				mrow=mysql_fetch_row(mres);
				snprintf (profcomment,254,"Сведения о профилактике: %s ",mrow[0]);
			}
			// добавь событие в лог!

			snprintf (str,1023,"SELECT text FROM sob WHERE ID=%d",sobytie);
			myerr(3,str);
			if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
                        mres=mysql_store_result(&mysql);
                        if (mysql_num_rows(mres))
			{
				mrow=mysql_fetch_row(mres);
				snprintf (str1,7999,"%s %s %s твк (%s МГц) в %s. %s",mrow[0],row[3],row[1],row[2],cityname,profcomment); //строка-описание события
				if (sobytie<120)  snprintf (str,8023,"INSERT INTO log VALUES ('%s',%d,\"%s\",'n');",kogda,sobytie,str1);
				else  snprintf (str,8023,"INSERT INTO log VALUES ('%s',%d,\"%s\",'0');",kogda,sobytie,str1);
				myerr(3,str);
				if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
			}
			else myerr (2,"Нет события с таким кодом");

			if (sobytie<120) //отправь почту
			{
				k=0;
				for (j=0;j<=strlen(receipt);j++)
				{
					if ((receipt[j]!=' ')&&(receipt[j]!='\0')) str[k++]=receipt[j];
					else
					{
						str[k]='\0';
				
						f=fopen ("mail.txt","w");
						fprintf (f,"MIME-Version: 1.0\nContent-type: text/html;charset=\"UTF-8\"\nFrom: %d-%s <%s>\nTo:%s\nSubject: \"%s\"\n\n<HTML>\n%s %s %s </HTML>",region,sender,sender,str,str1,kogda,str1,mailfoot);
						fclose (f);
				
						mail (smtp,sender,mailpass,str);
						k=0;
					}
				}
				myerr (3,"mail sent");
			}


			snprintf (str,1023,"UPDATE log SET sent='s' WHERE kogda='%s';",kogda);

			myerr(3,str);
                        if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
                        mres=mysql_store_result(&mysql);
			// и укажи в логе, что отправил.
		}

	}


	// считываем лог, отправляем по почте


	snprintf (str,1023,"SELECT meas.ID,tvlist.kanal,meas.avs,meas.korr,meas.kogda,meas.filename FROM meas,tvlist WHERE meas.ID=tvlist.ID AND tvlist.state='O' AND ss=1;");
        if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
	res=mysql_store_result(&mysql);

	for (i=0;i<mysql_num_rows(res);i++)
	{
		row=mysql_fetch_row(res);
		korrl=8*log(atof(row[3])/720/576);
		sendwebdata (region,nrkp,mypasswd,"10.52.2.9",100,atoi(row[0]),atoi(row[1]),atoi(row[2]),(int)korrl,row[4],row[5]);
	}
	// отправка на сервер
	

	snprintf (str,1023,"UPDATE meas SET ss=2 WHERE ss=1;");
        if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}
        fprintf (stderr,"ss=2\n");

	mysql_close(&mysql);

}
