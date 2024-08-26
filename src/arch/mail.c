#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** returnable errors
**
** Error codes returned to the operating system.
**
*/
#define B64_SYNTAX_ERROR        1
#define B64_FILE_ERROR          2
#define B64_FILE_IO_ERROR       3
#define B64_ERROR_OUT_CLOSE     4
#define B64_LINE_SIZE_TO_MIN    5
#define B64_SYNTAX_TOOMANYARGS  6

/*
** b64_message
**
** Gather text messages in one place.
**
*/
#define B64_MAX_MESSAGES 7
static char *b64_msgs[ B64_MAX_MESSAGES ] = {
            "b64:000:Invalid Message Code.",
            "b64:001:Syntax Error -- check help (-h) for usage.",
            "b64:002:File Error Opening/Creating Files.",
            "b64:003:File I/O Error -- Note: output file not removed.",
            "b64:004:Error on output file close.",
            "b64:005:linesize set to minimum.",
            "b64:006:Syntax: Too many arguments."
};

#define b64_message( ec ) ((ec > 0 && ec < B64_MAX_MESSAGES ) ? b64_msgs[ ec ] : b64_msgs[ 0 ])


static void encodeblock( unsigned char *in, unsigned char *out, int len )
{
    out[0] = (unsigned char) cb64[ (int)(in[0] >> 2) ];
    out[1] = (unsigned char) cb64[ (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ (int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ (int)(in[2] & 0x3f) ] : '=');
}

encodestring (unsigned char *in, unsigned char *out)
{
	int i,len1;
	len1=strlen (in);
	for (i=0;i<(len1-1)/3;i++)
	{
		encodeblock (in+3*i,out+4*i,3);
		printf (" %d %s ",i,out);
	}
	encodeblock (in+3*i,out+4*i,(len1-1)%3+1);
printf (" %d %s ",i,out);

	out[4+4*i]='\r';
	out[5+4*i]='\n';
	out[6+4*i]='\0';
	printf ("%d %d %d\n",len1,i,len1%3);
}

oldmail(char *smtp, char *domain, char *sender, char *pwd, char *adres, char *text)
{
	char str[2024];
	int sock,a;
	struct sockaddr_in addr;
	unsigned long int addr1;
	int i;
	
	addr1=178*256*256*256+205*256*256+250*256+78;

//	addr1=94*256*256*256+100*256*256+177*256+1;	
	sock=socket(AF_INET, SOCK_STREAM, 0);
	
	addr.sin_family=AF_INET;
	addr.sin_port=htons(25);
	addr.sin_addr.s_addr=htonl(addr1);
	connect (sock,(struct sockaddr *) &addr, sizeof (addr));
	recv (sock,str,1023,0);
	printf ("%s\n",str);

	snprintf (str,1023,"HELO rfc16.ru\r\n");
	send (sock,str,strlen(str),0);
	printf ("%s\n",str);
	for (i=0;i<1023;i++) str[i]='\0';
	printf ("%s\n",str);

	snprintf (str,1023,"AUTH LOGIN\r\n");
        send (sock,str,strlen(str),0);
        printf ("%s\n",str);
        for (i=0;i<1023;i++) str[i]='\0';
        printf ("%s\n",str);

	recv (sock,str,1023,0);
        printf ("Received: %s\n",str);

	 recv (sock,str,1023,0);
        printf ("Received: %s\n",str);

	//printf ("16-Kazan@rfc16.ru->");
	printf ("%s->",sender);
        for (i=0;i<1023;i++) str[i]='\0';
	encodestring (sender,str);
	printf ("Sent:%s\n",str);
        send (sock,str,strlen(str),0);
        for (i=0;i<1023;i++) str[i]='\0';
        printf ("%s\n",str);
	recv (sock,str,1023,0);
        printf ("Received: %s\n",str);

	// printf ("202->");
	printf ("%s->",pwd);
        for (i=0;i<1023;i++) str[i]='\0';
	encodestring (pwd,str);
	printf ("Sent: %s\n",str);
        send (sock,str,strlen(str),0);
        for (i=0;i<1023;i++) str[i]='\0';
	recv (sock,str,1023,0);
        printf ("Received: %s\n",str);

	snprintf (str,1023,"MAIL FROM: <%s>\r\n",sender);
	send (sock,str,strlen(str),0);
        printf ("Sent: %s\n",str);
        for (i=0;i<1023;i++) str[i]='\0';
	recv (sock,str,1023,0);
	printf ("Received: %s\n",str);

	snprintf (str,1023,"RCPT TO: <%s>\r\n",adres);
	send (sock,str,strlen(str),0);
        printf ("%s\n",str);
        for (i=0;i<1023;i++) str[i]='\0';
        recv (sock,str,1023,0);
        printf ("%s\n",str);


/*	snprintf (str,1023,"RCPT TO: <sckf@mail.ru>\r\n");
	send (sock,str,strlen(str),0);
	printf ("RCPT TO: <sckf@mail.ru>\r\n");
	for (i=0;i<1023;i++) str[i]='\0';
	recv (sock,str,1023,0);
	printf ("%s\n",str);

	snprintf (str,1023,"RCPT TO: <sckf@rfc16.ru>\r\n");
	send (sock,str,strlen(str),0);
	for (i=0;i<1023;i++) str[i]='\0';
	recv (sock,str,1023,0);
	printf ("%s\n",str);

	snprintf (str,1023,"RCPT TO: <scpfo@rfc.nnov.ru>\r\n");
	send (sock,str,strlen(str),0);
	for (i=0;i<1023;i++) str[i]='\0';
	recv (sock,str,1023,0);
	printf ("%s\n",str);

	send (sock,"DATA\r\n",6,0);
*/
	snprintf (str,1023,"DATA\r\n");
	send (sock,str,strlen(str),0);
        printf ("Sent: %s\n",str);
        for (i=0;i<1023;i++) str[i]='\0';
        recv (sock,str,1023,0);
        printf ("Received: %s\n",str);

//	printf ("DATA\r\n");
//	for (i=0;i<1023;i++) str[i]='\0';
//	recv (sock,str,1023,0);
//	printf ("Received: %s\n",str);

	snprintf (str,1023,"Date: Today UT\r\nMIME-Version: 1.0\r\nContent-type: text/plain;charset=\"UTF-8\"\r\nContent-Transfer-Encoding: 8bit\r\nFrom: %s\r\nTo: %s\r\nX-Priority: 3\r\nsubject: %s\r\n\r\n.\r\n\r\n",sender,adres,text);
	send (sock,str,strlen (str)+5,0);
	printf ("Sent: %s",str);
	for (i=0;i<1023;i++) str[i]='\0';
	recv (sock,str,1023,0);
	printf ("Received: %s\n",str);

	send (sock,"QUIT\r\n",6,0);
	for (i=0;i<1023;i++) str[i]='\0';
	recv (sock,str,1023,0);
	printf ("Received: %s\n",str);
	close (sock);

}

