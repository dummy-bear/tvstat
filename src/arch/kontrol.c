char mysmtp[]="178.205.250.78";
char mydomain[]="rfc16.ru";
char mysender[]="16-Bugulma@rfc16.ru";
char mypass[]="202";
char receiver[3][64]={"tvpfo@rfc.nnov.ru","sckf@rfc16.ru","sckf@mail.ru"};


main()
{
int i;
for (i=0;i<3;i++) 
{
	mail (mysmtp,mydomain,mysender,mypass,receiver[i],"НРКП-Бугульма контроль тв-тюнера \r\n\r\n Это тестовое сообщение. Не подлежит учёту. Система tvstat в г.Бугульма ещё тестируется, результаты контроля будут доступны по адресу: http://89.232.99.10:10048 \n");
	sleep (1);
}

}
