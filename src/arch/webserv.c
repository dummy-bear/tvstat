#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*main()
{
	fprintf (stderr,"hello\n");
	sendwebdata (1,1,99,13,"11:12 21.07.2014","noname");
}
i*/

int region=16;
int nrkp=19;
char mypasswd[]="dfae3xcsdfs23ffa";
char serv[]="10.52.2.9";
int myqual=100;

sendwebdata(int id,int kanal, int avs, int korrel, char *tim, char *file)
{
        char str[2024], tim1[128];
        int sock,a;
        struct sockaddr_in addr;
        unsigned long int addr1;
        int i;

        addr1=10*256*256*256+52*256*256+2*256+9;
//      sprintf (str,"E....]@...i.....N...    P.}....,.)DP...y...89372831609.Message");
//      printf ("%s\n",str);

//      addr1=94*256*256*256+100*256*256+177*256+1;
        sock=socket(AF_INET, SOCK_STREAM, 0);

        addr.sin_family=AF_INET;
        addr.sin_port=htons(80);
        addr.sin_addr.s_addr=htonl(addr1);
        connect (sock,(struct sockaddr *) &addr, sizeof (addr));
//        recv (sock,str,1023,0);
//        printf ("%s\n",str);

	for (i=0,a=0;i<strlen(tim);i++) 
	{
//		printf ("%d, %d,%s==%s\n",i,a,tim, tim1);
	//	tim1[a+1]='\0';
		if (tim[i]==' ') {strcpy((char*)(tim1+a),"%20");a+=3;}
		else tim1[a++]=tim[i];
	}
	tim1[a]='\0';
        sprintf (str,"GET /tv/updmeas.php?ID=%d&region=%d&nrkp=%d&passwd=%s&kogda=%s&kanal=%d&avs=%d&quality=%d&korrel=%d&file=%s\nHost: %s\nUser-Agent: Tormozilla/5.0 (X11; U; Linux 8086; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\nAccept: text/html\nConnection: close\n\n",id,region,nrkp,mypasswd, tim1,kanal,avs,myqual,korrel,file,serv);
        fprintf (stderr,"%s",str);
        send (sock,str,strlen(str),0);


        recv (sock,str,1023,0);
//        for (i=0;i<1023;i++) str[i]='\0';
        printf ("%s\n",str);

}

