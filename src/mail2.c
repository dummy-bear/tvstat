#include <stdlib.h>
#include <stdio.h>

mail(char *smtp, char *sender, char *passwd, char *receipt)
{
	char str[1024];
	snprintf (str,1023,"curl --url \"smtps://%s\" --ssl-reqd --mail-from \"%s\" --mail-rcpt \"%s\" --upload-file mail.txt --user \"%s:%s\" --insecure",smtp,sender,receipt,sender,passwd);
	printf ("%s\n",str);
	system (str);
}

mailk(char *smtp, char *sender, char *passwd, char *receipt)
{
        char str[1024];
        snprintf (str,1023,"curl --url \"smtps://%s\" --ssl-reqd --mail-from \"%s\" --mail-rcpt \"%s\" --upload-file mailk.txt --user \"%s:%s\" --insecure",smtp,sender,receipt,sender,passwd);
        printf ("%s\n",str);
        system (str);
}


/*
main()
{
	mail("smtp.yandex.ru:465","bugulma@tvstat.ru","pass","adress");
}

*/
