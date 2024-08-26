/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see http://linuxtv.org/docs.php for more information
 */

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

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define SECAM 17
#define PAL 4

enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};

struct buffer {
        void   *start;
        size_t  length;
};


//константы доступа к бд
char mshost[]="localhost";
char msuser[]="root";
char mspass[]="dbpass";
char msbase[]="tvstat";

int tvchans,rchans; //количество каналов
int scanchan;
//int errlev=3;  //уровень вывода ошибок

struct {
int id;
char name[256];
int tvk;
float freq;
int cod;
int status;
float korr,bright;
time_t mt;
char scrname[1024];
} tvch[100];

struct {
int id;
float freq;
int status;
} rch[100]; // здесь будут храниться частоты для сканирования

char save=0; // to save or not to save a frame to file
struct tm tm1; //time of measurement

struct {
char dev_video[256];
char dev_radio[256];
char dev_alsa[256];
char num_alsa[256];
} sd;

static int fd = -1; //handle for opened video device file
static enum io_method   io = IO_METHOD_MMAP;   //method for accessing video contents
static int force_format;
struct buffer *buffers;
static unsigned int n_buffers;


static void errno_exit(const char *s)
{
        fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

/*static void myerr (int level, char *s)
{
	if (level<=errlev) fprintf(stderr, "%s\n", s);
	if (!level) {
		close_device();
		exit(EXIT_FAILURE);
	}
}
*/
main(int argc, char **argv)
{
        int i;

        init();

        for (scanchan=0;scanchan<tvchans;scanchan++) if (scandev_free()) scantv(scanchan);
        for (i=0;i<rchans;i++) if (scandev_free()) scanrv (i);

        printf ("finish\n");
	return 0;

}

// Загрузить все необходимые данные для работы
init()
{
        MYSQL mysql;
        MYSQL_RES *res;
        MYSQL_ROW row;

        int i;
	char str[1024];
	//fprintf (stderr,"init start\n");

        mysql_init (&mysql);
        if (!mysql_real_connect(&mysql,mshost,msuser,mspass,msbase,0,0,0)) {fprintf (stderr,"error connecting mysql\n");exit(1);}

// Присваеваем переменные для устройств сканирования

	 if (mysql_query(&mysql,"SET NAMES 'utf8'")) {printf ("mysql select error\n");return;}
        strcpy (str,"SELECT * FROM settings;");
	fprintf (stderr,"%s\n",str);

	if (mysql_query(&mysql,str))  {fprintf (stderr,"error reading mysql\n");exit(1);}

	res=mysql_store_result(&mysql);
	if (mysql_num_rows(res))
	{
		row=mysql_fetch_row(res);
		strcpy(sd.dev_video,row[3]);
//	        strcpy(sd.dev_radio,row[1]);
//        	strcpy(sd.dev_alsa,row[2]);
//	        strcpy(sd.num_alsa,row[3]);
	}
	else {fprintf (stderr,"tvstat не настроена: нет данных об устройстве сканирования\n");exit (1);}


// Считываем список тв-каналов для сканирования

	snprintf (str,1023,"SELECT * FROM tvlist ORDER BY ID;");
	fprintf (stderr,"%s\n",str);
      if (mysql_query(&mysql,str)) {printf ("mysql select error\n");return;}
        res=mysql_store_result(&mysql);
        tvchans=mysql_num_rows(res);
	
        for (i=0;i<tvchans;i++)
        {
		row=mysql_fetch_row(res);
		tvch[i].id=atoi(row[0]);
                tvch[i].tvk=atoi(row[1]);
                tvch[i].freq=atof(row[2]);
                tvch[i].status=row[4][0];
		tvch[i].cod=atoi(row[6]);
//                freq[i]=chann[i].freq;
                strncpy(tvch[i].name,row[3],255);
		fprintf (stderr,"%d - %d - %f - %c - %d - %s\n",tvch[i].id,tvch[i].tvk,tvch[i].freq,tvch[i].status,tvch[i].cod,tvch[i].name);
	}
	

	fprintf (stderr,"Считаны настройки.\n sd.dev_video = %s \n sd.dev_radio = %s \n sd.dev_alsa = %s \n sd.num_alsa = %s \n",sd.dev_video,sd.dev_radio,sd.dev_alsa,sd.num_alsa);
}

scandev_free()
{
	FILE *f;
	if (f=fopen("bcast1","r")) {fclose (f); fprintf (stderr,"device busy\n");return 0;}
	else if (f=fopen("scan1","r")) {fclose (f); fprintf (stderr,"device busy\n");return 0;}
	else return 1;
}


// Дальше - функции, обслуживающие процесс сканирования

tunefreq(float freq, int std)
{
	// настроить частоту
	struct v4l2_frequency freq1;
	struct v4l2_tuner tun1;

	struct v4l2_input input;
	v4l2_std_id std_id;
	int i,sec_level,pal_level;

	freq1.frequency=freq*16;
	freq1.tuner=0;
	freq1.type=V4L2_TUNER_ANALOG_TV;

	ioctl(fd,VIDIOC_S_FREQUENCY,&freq1);
	
	printf ("Frequency %g MHz tuned!",freq);
        memset (&input, 0, sizeof (input));

        if (-1 == ioctl (fd, VIDIOC_G_INPUT, &input.index))
        {
                perror ("VIDIOC_G_INPUT");
                exit (EXIT_FAILURE);
        }

        if (-1 == ioctl (fd, VIDIOC_ENUMINPUT, &input))
        {
                perror ("VIDIOC_ENUM_INPUT");
                exit (EXIT_FAILURE);
        }

        if (0 == (input.std & V4L2_STD_PAL_BG)) {
                fprintf (stderr, "Oops. B/G PAL is not supported.\n");
                exit (EXIT_FAILURE);
        }

        /* Note this is also supposed to work when only B
   or G/PAL is supported. */

        if (std==PAL) std_id = V4L2_STD_PAL;
        else if (std==SECAM) std_id=V4L2_STD_SECAM;

        if (-1 == ioctl (fd, VIDIOC_S_STD, &std_id))
        {

                perror ("VIDIOC_S_STD");
                exit (EXIT_FAILURE);
        }
	myerr (3,"Установлена кодировка");
}

getlevel()
{
	return 0;
}


static int xioctl(int fh, int request, void *arg)
{
        int r;

        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);

        return r;
}


void close_device(void)
{
        if (-1 == close(fd))
                errno_exit("close");
	remove ("scan1");

        fd = -1;
}

static void open_device(void)
// итог - ссылка на открытое устройство sd.dev_video в дескрипторе fd или выход по ошибке
{
        struct stat st;
	FILE *f;

        if (-1 == stat(sd.dev_video, &st)) {
                fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                         sd.dev_video, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is no device\n", sd.dev_video);
                exit(EXIT_FAILURE);
        }

        fd = open(sd.dev_video, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd) {
                fprintf(stderr, "Cannot open '%s': %d, %s\n",
                         sd.dev_video, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
	else {
	        f=fopen("scan1","w");
        	fprintf (f,"%s",sd.dev_video);
	        fclose (f);
	}

}


static void stop_capturing(void)
{
        enum v4l2_buf_type type;

        switch (io) {
        case IO_METHOD_READ:
                /* Nothing to do. */
                break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
                        errno_exit("VIDIOC_STREAMOFF");
                break;
        }
}


static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;
	fprintf (stderr,"Start capturing\n");

        switch (io) {
        case IO_METHOD_READ:
                /* Nothing to do. */
                break;

        case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_MMAP;
                        buf.index = i;

                        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                                errno_exit("VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                        errno_exit("VIDIOC_STREAMON");
                break;

        case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_USERPTR;
                        buf.index = i;
                        buf.m.userptr = (unsigned long)buffers[i].start;
                        buf.length = buffers[i].length;

                        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                                errno_exit("VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                        errno_exit("VIDIOC_STREAMON");
                break;
        }
}


static void uninit_device(void)
{
        unsigned int i;

        switch (io) {
        case IO_METHOD_READ:
                free(buffers[0].start);
                break;

        case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i)
                        if (-1 == munmap(buffers[i].start, buffers[i].length))
                                errno_exit("munmap");
                break;

        case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i)
                        free(buffers[i].start);
                break;
        }

        free(buffers);
}

static void init_read(unsigned int buffer_size)
{
        buffers = calloc(1, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        buffers[0].length = buffer_size;
        buffers[0].start = malloc(buffer_size);

        if (!buffers[0].start) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }
}

static void init_mmap(void)
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count = 4;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "memory mapping\n", sd.dev_video);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf(stderr, "Insufficient buffer memory on %s\n",
                         sd.dev_video);
                exit(EXIT_FAILURE);
        }

        buffers = calloc(req.count, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;

                CLEAR(buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }
}

static void init_userp(unsigned int buffer_size)
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count  = 4;
        req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "user pointer i/o\n", sd.dev_video);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        buffers = calloc(4, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
                buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = malloc(buffer_size);

                if (!buffers[n_buffers].start) {
                        fprintf(stderr, "Out of memory\n");
                        exit(EXIT_FAILURE);
                }
        }
}



static void init_device(float freq,int std)
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
        unsigned int min;

        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s is no V4L2 device\n",
                                 sd.dev_video);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf(stderr, "%s is no video capture device\n",
                         sd.dev_video);
                exit(EXIT_FAILURE);
        }

        switch (io) {
        case IO_METHOD_READ:
                if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                        fprintf(stderr, "%s does not support read i/o\n",
                                 sd.dev_video);
                        exit(EXIT_FAILURE);
                }
                break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
                if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                        fprintf(stderr, "%s does not support streaming i/o\n",
                                 sd.dev_video);
                        exit(EXIT_FAILURE);
                }
                break;
        }


        /* Select video input, video standard and tune here. */
	printf ("goto tunefreq\n");
	tunefreq(freq,std);

        CLEAR(cropcap);

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
                        switch (errno) {
                        case EINVAL:
                                /* Cropping not supported. */
                                break;
                        default:
                                /* Errors ignored. */
                                break;
                        }
                }
        } else {
                /* Errors ignored. */
        }


        CLEAR(fmt);

        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (force_format) {
                fmt.fmt.pix.width       = 720;
                fmt.fmt.pix.height      = 576;
                fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
                fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

                if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
                        errno_exit("VIDIOC_S_FMT");

                /* Note VIDIOC_S_FMT may change width and height. */
        } else {
                /* Preserve original settings as set by v4l2-ctl for example */
                if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
                        errno_exit("VIDIOC_G_FMT");
        }

        /* Buggy driver paranoia. */
        min = fmt.fmt.pix.width * 2;
        if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;
        min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
        if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;

        switch (io) {
        case IO_METHOD_READ:
                init_read(fmt.fmt.pix.sizeimage);
                break;

        case IO_METHOD_MMAP:
                init_mmap();
                break;

        case IO_METHOD_USERPTR:
                init_userp(fmt.fmt.pix.sizeimage);
                break;
        }
}




static void process_image(const void *p, int size)
{
        FILE *f1;
        float medium =0, disp=0;
        unsigned char an1;
        long int i,j;
        int x,y,dx,dy,cn;
//        struct tm tm1;
        time_t tim1;
        char str[1024],str1[512],ppmfile[16];
        struct v4l2_tuner tun1;
        int fd1;
//        MYSQL mysql1,mysql2;
//        MYSQL_RES *res;
//        MYSQL_ROW row;
        int ass[3];
        float kor1[3];
        unsigned char fr[77760];
        float sz;
	unsigned char bg;

	unsigned char frame[1244160];
	unsigned char frame2[1244160];
	float avbright[40][1000], avdisp[40][1000];
	float korrl;


                memcpy (frame,p,size);
                for (i=0;i<size;i++)
                        medium+=(float)frame[i]/(float)size;
                for (i=0;i<size;i++)
                        disp+=((float)frame[i]-medium)*((float)frame[i]-medium)/size;
                disp=sqrt(disp);

                if (save==1)
                {
			for (i=0;i<size;i+=3)
                        {
                                bg=frame[i];
                                frame[i]=frame[i+2];
//                                frame[i+1]=0;
                                frame[i+2]=bg;
			}
                        for (i=0;i<77760;i+=3)
                        {
			        x=(i/3)%180;y=(i/3)/180;
                                cn=0;
                                for (dx=0;dx<4;dx++)
                                        for (dy=0;dy<4;dy++) cn+=frame[(y*720*12+dy*720*3)+x*12+dx*3];
                                fr[i]=cn/16;
                                cn=0;

                                for (dx=0;dx<4;dx++)
                                        for (dy=0;dy<4;dy++) cn+=frame[(y*720*12+dy*720*3)+x*12+dx*3+1];
                                fr[i+1]=cn/16;
                                cn=0;
                                for (dx=0;dx<4;dx++)
                                        for (dy=0;dy<4;dy++) cn+=frame[(y*720*12+dy*720*3)+x*12+dx*3+2];
                                fr[i+2]=cn/16;

                        }
                        snprintf (ppmfile,16,"image%d.ppm",scanchan);
                        printf ("reading %s\n",ppmfile);
                        if (f1=fopen(ppmfile,"r"))
			{
                        	fread (frame2,15,1,f1);
                        	fread (frame2,size,1,f1);
                        	fclose (f1);
				myerr (3,"ppm read");
			}
			else myerr (2,"Cannot open ppmfile\n");

                        tvch[scanchan].korr=0;
                        for (i=0;i<size;i++)  tvch[scanchan].korr+=(float)(frame2[i]-frame[i])*(frame2[i]-frame[i]);

                        myerr (3,"korr counted\n");

                        f1=fopen(ppmfile,"w");
                        fprintf (f1,"P6\n720 576\n255\n");
                        fwrite(frame, size, 1, f1);
                        fclose (f1);
                        myerr (3,"ppm file written\n");

                        f1=fopen("img.ppm","w");
                        fprintf (f1,"P6\n180 144\n255\n");
                        fwrite(fr,77760,1,f1);
                        fclose (f1);
			myerr (3,"img.ppm written");

                        time (&tvch[scanchan].mt);
			
                        memcpy(&tm1,localtime(&tvch[scanchan].mt),sizeof(tm1));
			
			snprintf (str1,511,"/var/www/img/%d",1900+tm1.tm_year);
			mkdir (str1,511);
			snprintf (str1,511,"/var/www/pimg/%d",1900+tm1.tm_year);
                        mkdir (str1,511);
			snprintf (str1,511,"/var/www/img/%d/%02d",1900+tm1.tm_year,1+tm1.tm_mon);
                        mkdir (str1,511);
			snprintf (str1,511,"/var/www/pimg/%d/%02d",1900+tm1.tm_year,1+tm1.tm_mon);
                        mkdir (str1,511);
			snprintf (str1,511,"/var/www/img/%d/%02d/%02d",1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday);
                        mkdir (str1,511);
			snprintf (str1,511,"/var/www/pimg/%d/%02d/%02d",1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday);
                        mkdir (str1,511);
			myerr (3,"folders made");

//сделаем каталоги, если их ещё нет


                        snprintf (str1,511,"%d/%02d/%02d/%g-%d.%02d.%02d-%d-%02d-%02d.jpg",1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday,tvch[scanchan].freq,1900+tm1.tm_year,1+tm1.tm_mon,tm1.tm_mday,tm1.tm_hour,tm1.tm_min,tm1.tm_sec);
                        snprintf (str,1023,"cjpeg -quality 60 %s > /var/www/img/%s",ppmfile,str1);
                        system (str);
                        snprintf (str,1023,"cjpeg -quality 60 img.ppm > /var/www/pimg/%s",str1);
                        system (str);
			snprintf (tvch[scanchan].scrname,1000,"%s",str1);
			
			// Create 2 jpeg: view and preview
		}	

}



static int read_frame(void)
{
        struct v4l2_buffer buf;
        unsigned int i;

        switch (io) {
        case IO_METHOD_READ:
                if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("read");
                        }
                }

                process_image(buffers[0].start, buffers[0].length);
                break;

        case IO_METHOD_MMAP:
                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;

                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("VIDIOC_DQBUF");
                        }
                }

                assert(buf.index < n_buffers);

                process_image(buffers[buf.index].start, buf.bytesused);

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
                break;

        case IO_METHOD_USERPTR:
                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_USERPTR;

                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("VIDIOC_DQBUF");
                        }
                }

                for (i = 0; i < n_buffers; ++i)
                        if (buf.m.userptr == (unsigned long)buffers[i].start
                            && buf.length == buffers[i].length)
                                break;

                assert(i < n_buffers);

                process_image((void *)buf.m.userptr, buf.bytesused);

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
                break;
        }

        return 1;
}


meas()
{
	int i,avs=0;
	struct v4l2_tuner tun1;

	for (i=0;i<100;i++)
	{
                
		if (i==70) save=1; else save=0;
		tun1.index=0;
                tun1.reserved[0]=0;
                tun1.reserved[1]=0;
                tun1.reserved[2]=0;
                tun1.reserved[3]=0;

                ioctl (fd, VIDIOC_G_TUNER,&tun1);
                if (tun1.signal) avs++;

                for (;;) {
                        fd_set fds;
                        struct timeval tv;
                        int r;

                        FD_ZERO(&fds);
                        FD_SET(fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1, &fds, NULL, NULL, &tv);

                        if (-1 == r) {
                                if (EINTR == errno)
                                        continue;
                                errno_exit("select");
                        }

                        if (0 == r) {
                                fprintf(stderr, "select timeout\n");
                                exit(EXIT_FAILURE);
                        }

                        if (read_frame())
                                break;
                        /* EAGAIN - continue select loop. */
                }

	
	}
	return avs;
}

// Сканирование каналов
//------------------------------------------------------
scantv(int channel)
{
        MYSQL mysql;
        MYSQL_RES *res;
        MYSQL_ROW row;

	int lep,les;
	char str[256];

        open_device();
	myerr (3,"scantv.opened device\n");

	snprintf (str,255,"scantv.init device encod=%d",tvch[channel].cod);
	myerr (3,str);
	 init_device (tvch[channel].freq,tvch[channel].cod);
	start_capturing();
	les=meas();
	stop_capturing();
	uninit_device();

	myerr (3,"uninit device");	
        mysql_init (&mysql);
        if (!mysql_real_connect(&mysql,mshost,msuser,mspass,msbase,0,0,0)) {fprintf (stderr,"error connecting mysql\n");exit(1);}
        if (mysql_query(&mysql,"SET NAMES 'utf8'")) {printf ("mysql select error\n");return;}

 	myerr (3,"opened mysql"); 
	if (les<70) 
	{
		fprintf (stderr,"let's try other code\n");
		if (tvch[channel].cod==SECAM) init_device(tvch[channel].freq,PAL);
		else  init_device(tvch[channel].freq,SECAM);
		start_capturing();
		les=meas();
	        stop_capturing();
       		uninit_device();
		if (les>70) //change encoding for channel
		{
			if (tvch[channel].cod==SECAM) tvch[channel].cod=PAL;
			else tvch[channel].cod=SECAM;
			snprintf (str,255,"UPDATE tvlist SET cod=%d WHERE ID=%d;", tvch[channel].cod, tvch[channel].id);
			myerr(3,str);
			if (mysql_query(&mysql,str))  {myerr (1,"error updating channel in mysql\n");exit(1);}
		}

	}
	close_device();
	
	myerr (3,"closed device");

	//saving measurement in database
	snprintf (str,255,"INSERT INTO meas VALUES (%d,'%d-%d-%d %d:%d:%d',%d, %g, %g, %g, \"%s\", %d, 0 );",tvch[channel].id, 1900+tm1.tm_year, 1+tm1.tm_mon, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec, les, tvch[channel].bright, 0.0,tvch[channel].korr, tvch[channel].scrname, tvch[channel].cod);
	myerr (3,str);
	if (mysql_query(&mysql,str))  {myerr (1,"error updating channel in mysql\n");exit(1);}
	mysql_close(&mysql);


        return 0;
}
//------------------------------------------------------

scanrv(int channel)
{
        return 0;
}

