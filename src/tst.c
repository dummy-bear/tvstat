#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <mysql/mysql.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>


#include <linux/videodev2.h>


main()
{
	struct v4l2_modulator radio1;
	int fd2;
	struct v4l2_frequency freq1;
	float freq;


	scanf ("%f",&freq);
	freq1.frequency=freq*16000;
        freq1.tuner = 0;
	freq1.type = V4L2_TUNER_RADIO;

        fd2 = open ("/dev/radio1", O_RDWR);
        ioctl(fd2,VIDIOC_S_FREQUENCY, &freq1);
	
	scanf ("%f",&freq);
	close (fd2);

//	system("arecord -f cd -t wav -D sysdefault:CARD=SAA7134 --max-file-time 3600 --use-strftime /var/www/video/2015/test.wav &");
//	system("pwd");
//	system("radio -c /dev/radio1 -f 100,5");
}
