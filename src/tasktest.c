#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

//константы доступа к бд
char mshost[]="localhost";
char msuser[]="root";
char mspass[]="dbpass";
char msbase[]="tvstat";



main()
{
	MYSQL mysql;
	MYSQL_RES *res;
        MYSQL_ROW row;
	
	char str[1024],str1[1024];
	char tvrecdev[200],rvrecdev[200],alsarecdev[200],alsarecnum[200];
        struct tm tm1;
        time_t tim1;

	int i;
	int rad;
	float freq;
	struct v4l2_frequency freq1;

        mysql_init (&mysql);
        if (!mysql_real_connect(&mysql,mshost,msuser,mspass,msbase,0,0,0)) {myerr(0,"Error connecting to database");return;}

	// is there a tasks to be finished
	myerr(3,"start");

	getsetstr(&mysql, "tvrecdev", tvrecdev);
	getsetstr(&mysql, "rvrecdev", rvrecdev);
	getsetstr(&mysql, "alsarecdev", alsarecdev);
	getsetstr(&mysql, "alsarecnum", alsarecnum);
	myerr (3,"settings read");

	snprintf (str,1023,"SELECT typ,ID FROM tasks WHERE stop<now() AND status='r';");

	if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error 1\n");return;}
	res=mysql_store_result(&mysql);

	if (mysql_num_rows(res))
	{
		row=mysql_fetch_row(res);
		myerr(3, "there is a task to kill");
		switch (row[0][0])
		{
		case 'A': system ("killall arecord");
		//	  system ("killall radio");
			  break;
		case 'b': system ("killall vlc");
			  break;
		case 'r': system ("killall mencoder");
			  break;
		}
		snprintf (str1,1023,"INSERT INTO tasklog VALUES (now(),%s, \"killall %c\");",row[1],row[0][0]);
		if (mysql_query(&mysql,str1)) {fprintf (stderr,"mysql select error 1\n");return;}
		snprintf (str,1023,"UPDATE tasks SET status='s' WHERE stop<now() AND status='r';");
		myerr(3,str);
		if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error 1\n");return;}
		sleep (25);
	}


	// If there were tasks with errors let's try to stop it
	snprintf (str,1023,"SELECT typ FROM tasks WHERE stop>now() AND status='e';");

        if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error 1\n");return;}
        res=mysql_store_result(&mysql);

        if (mysql_num_rows(res))
        {
                row=mysql_fetch_row(res);
                myerr(3, "there is a task with error to kill");
                switch (row[0][0])
                {
                case 'A': system ("killall arecord");
                //        system ("killall radio");
                          break;
                case 'b': system ("killall vlc");
                          break;
                case 'r': system ("killall mencoder");
                          break;
                }
                snprintf (str,1023,"UPDATE tasks SET status='w' WHERE stop>now() AND status='e';");
                myerr(3,str);
                if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error 1\n");return;}
		sleep (10);
        }


	snprintf (str,1023,"SELECT ID,typ,freq,cod FROM tasks WHERE start<now() AND status='w' order by start desc limit 1;");
	if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error 3\n");return;}

        res=mysql_store_result(&mysql);
        if (mysql_num_rows(res)>0) //Если есть задания, которые пора начинать
        {
                row=mysql_fetch_row(res);
		myerr(3,"there is a task to start");
                time (&tim1);
                memcpy(&tm1,localtime(&tim1),sizeof(tm1));
                myerr (3,"starting task1");

		snprintf (str,1023,"UPDATE tasks SET status='r',result=\"%s\" WHERE ID=%s;",row[0],row[0]);
		myerr(3,str);
		if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error\n");return 1;}
		//show in database that task is running

		snprintf (str,1023,"mkdir /var/www/video/%s",row[0]);
		system (str);
		//create folder for result

                switch (row[1][0])
                {
                case 'r': snprintf (str,1023,"mencoder tv:// -tv freq=%s:normid=%s:device=%s:buffersize=30:alsa:adevice=hw.%s,0 -o /var/www/video/%s/%s-%d.%02d.%02d-%d-%02d-%02d.avi -ovc lavc -oac mp3lame -msglevel all=0",row[2],row[3],tvrecdev,alsarecnum,row[0],row[2],1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);

			snprintf (str1,1023,"INSERT INTO tasklog VALUES (now(),%s, \"%s\");",row[0],str);
        	        if (mysql_query(&mysql,str1)) {fprintf (stderr,"mysql select error 1\n");return;}

			  system(str);
			  break;
		case 'A': //for (i=0;row[3][i]!='\0';i++) if (row[3][i]=='.') row[3][i]=',';
			  rad = open (rvrecdev, O_RDWR);
			  freq = atof (row[2]);
			  freq1.frequency = freq*16000;
			  freq1.tuner = 0;
			  freq1.type = V4L2_TUNER_RADIO;
			  ioctl(rad, VIDIOC_S_FREQUENCY, &freq1);
			  snprintf (str,1023,"arecord -f cd -t wav -D sysdefault:CARD=%s --max-file-time 3600 --use-strftime /var/www/video/%s/\%Y-%%m-%%d-radio-%%H-%%M-%%v-%sMHz.wav",alsarecdev,row[0],row[2]);
			  myerr(3,str);


			snprintf (str1,1023,"INSERT INTO tasklog VALUES (now(),%s, \"%s\");",row[0],str);
                        if (mysql_query(&mysql,str1)) {fprintf (stderr,"mysql select error 1\n");return;}

			  system(str);
			  close(rad);
			break;
		case 'b': 
			break;
		}

		snprintf (str1,1023,"INSERT INTO tasklog VALUES (now(),%s, \"task finished\");",row[0]);
                if (mysql_query(&mysql,str1)) {fprintf (stderr,"mysql select error 1\n");return;}

		snprintf (str,1023,"SELECT ID,typ,freq,cod FROM tasks WHERE ID=%s AND stop>now();",row[0]);
		if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error\n");return 1;}
		res=mysql_store_result (&mysql);

		//there were errors if we got here earlier than the stop time arrived
		if (mysql_num_rows(res)>0)
		{
			snprintf (str,1023,"UPDATE tasks SET status='e' WHERE ID=%s;",row[0]);
			if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error\n");return 1;}

			snprintf (str,1023,"SELECT text FROM sob WHERE ID=200;");
			if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error\n");return 1;}
			res=mysql_store_result(&mysql);
			row=mysql_fetch_row(res);

			snprintf (str,1023,"INSERT INTO log VALUES ( now(), 200, \"%s\", 'n');",row[0]);
                        if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error\n");return 1;}

		}
	}

/*	snprintf (str,1023,"SELECT ID,typ,freq,cod FROM tasks WHERE start<now() AND status='e' order by start desc limit 1;");
        if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error 3\n");return;}

        res=mysql_store_result(&mysql);
        if (mysql_num_rows(res)>0) //Если есть задания, которые пора начинать
        {
                row=mysql_fetch_row(res);
                myerr(3,"there is a task to start");
                time (&tim1);
                memcpy(&tm1,localtime(&tim1),sizeof(tm1));
                myerr (3,"starting task1");

                snprintf (str,1023,"UPDATE tasks SET status='r',result=\"%s\" WHERE ID=%s;",row[0],row[0]);
                myerr(3,str);
                if (mysql_query(&mysql,str)) {fprintf (stderr,"mysql select error\n");return 1;}
                //show in database that task is running

                snprintf (str,1023,"mkdir /var/www/video/%se",row[0]);
                system (str);
                //create folder for result

	 switch (row[1][0])
                {
                case 'r': snprintf (str,1023,"mencoder tv:// -tv freq=%s:normid=%s:device=%s:buffersize=30:alsa:adevice=hw.%s,0 -o /var/www/video/%s/%s-%d.%02d.%02d-%d-%02d-%02d.avi -ovc lavc -oac mp3lame -msglevel all=0",row[2],row[3],tvrecdev,alsarecnum,row[0],row[2],1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
                          system(str);
                          break;
                case 'A': //for (i=0;row[3][i]!='\0';i++) if (row[3][i]=='.') row[3][i]=',';
                          rad = open (rvrecdev, O_RDWR);
                          freq = atof (row[2]);
                          freq1.frequency = freq*16000;
                          freq1.tuner = 0;
                          freq1.type = V4L2_TUNER_RADIO;
                          ioctl(rad, VIDIOC_S_FREQUENCY, &freq1);
                          snprintf (str,1023,"arecord -f cd -t wav -D sysdefault:CARD=%s --max-file-time 3600 --use-strftime /var/www/video/%s/\%Y-%%m-%%d-radio-%%H-%%M-%%v-%sMHz.wav",alsarecdev,row[0],row[2]);
                          myerr(3,str);
                          system(str);
                          close(rad);
                        break;
                case 'b':
                        break;
                }


	}
*/

	mysql_close(&mysql);

}

old()
{

/*


	snprintf (str,1023,"SELECT * FROM tasks WHERE start<now() AND status='w' order by start desc limit 1;");
	if (mysql_query(&mysql1,str)) {fprintf (stderr,"mysql select error 3\n");return;}

	res=mysql_store_result(&mysql1);
	if (mysql_num_rows(res)>0) //Если есть задания, которые пора начинать
        {
                row=mysql_fetch_row(res);
		time (&tim1);
                memcpy(&tm1,localtime(&tim1),sizeof(tm1));
		printf ("starting task1");
 
                switch (row[3][0])
                {
                case 'r': snprintf (str,1023,"UPDATE tasks SET status='r',result=\"%s-%d.%02d.%02d-%d-%02d-%02d.avi\" WHERE start<now() AND status='w';",row[4],1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
                printf ("%s \n\n",str);
                if (mysql_query(&mysql1,str)) {fprintf (stderr,"mysql select error\n");return;}


snprintf (str,1023,"mencoder tv:// -tv freq=%s:normid=%s:device=%s:buffersize=30:alsa:adevice=hw.1,0 -o /var/www/video/%s-%d.%02d.%02d-%d-%02d-%02d.avi -ovc lavc -oac mp3lame -msglevel all=0",row[4],row[10],"/dev/video0",row[4],1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
			fprintf (stderr,"%s \n",str);
			sleep (10);
			system (str);
                         break;
                case 'b':freq1=atof(row[4])*1000;
			snprintf (str,1023,"UPDATE tasks SET status='r',result=\"%s-%d.%02d.%02d-%d-%02d-%02d.avi\" WHERE start<now() AND status='w';",row[4],1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
	                printf ("%s \n\n",str);
       			if (mysql_query(&mysql1,str)) {fprintf (stderr,"mysql select error\n");return;}

			if (strncmp(row[10],"17",2)) snprintf (str,1023,"vlc -I \"dummy\" v4l2:// :v4l2-device=/dev/video0 :v4l2-standard=pal :v4l2-input=0 :v4l2-tuner-frequency=%d :input-slave=alsa://hw:1,0 --alsa-samplerate=32000 --sout '#transcode{vcodec=mpgv,width=320,height=240,vb=3000,acodec=mpga,ab=96}:duplicate{dst=std{access=http,mux=ts,dst=0.0.0.0:8080}}' :no-v4l2-use-libv4l2 :no-v4l2-controls-reset",(int)freq1);
			else snprintf (str,1023,"vlc -I \"dummy\" v4l2:// :v4l2-device=/dev/video0 :v4l2-standard=secam :v4l2-input=0 :v4l2-tuner-frequency=%d :input-slave=alsa://hw:1,0 --alsa-samplerate=32000 --sout '#transcode{vcodec=mpgv,width=320,height=240,vb=3000,acodec=mpga,ab=96}:duplicate{dst=std{access=http,mux=ts,dst=0.0.0.0:8080}}' :no-v4l2-use-libv4l2 :no-v4l2-controls-reset",(int)freq1);

			printf ("%s \n\n\n\n\n",str);
			system (str);
			break;
                case 'A':freq1=atof(row[4]);//запись РВ
			snprintf (str,1023,"UPDATE tasks SET status='r',result=\"/%s/\" WHERE start<now() AND status='w';",row[0]);
                printf ("%s \n\n",str);
                if (mysql_query(&mysql1,str)) {fprintf (stderr,"mysql select error\n");return;}

			snprintf  (str,1023,"arecord -f cd -t wav -D default:CARD=SAA7134 --max-file-time 3600 --use-strftime /var/www/tv/video/%s/\%Y-%%m-%%d-listen-%sMHz.wav & radio -c /dev/radio1 -f %s",row[0],row[4],row[4]);
			printf ("%s \n\n\n\n\n",str);
			system (str);
                        break;
                case 'a':system ("killall arecord");
                        system ("sh tr1.sh");
                }
        }

        mysql_close (&mysql1);
*/	
	return 0;
}

