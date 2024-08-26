CC=gcc

clean:
	rm -f kontrol1 tvcomm task1 tvstat1 mail.txt *.ppm

kontrol: 
	$(CC) -lmysqlclient setting.c kontrol2.c mail2.c -o kontrol1

comm:
	$(CC) -lmysqlclient setting.c mail2.c comm.c -o tvcomm

task:
	$(CC) -lmysqlclient setting.c task.c -o task1

scan:
	$(CC) -lm -lmysqlclient setting.c capt2.c -o tvstat1

all:
	$(CC) -lmysqlclient setting.c kontrol2.c mail2.c -o kontrol1
	$(CC) -lmysqlclient setting.c mail2.c comm.c -o tvcomm
	$(CC) -lmysqlclient setting.c task.c -o task1
	$(CC) -lm -lmysqlclient setting.c capt2.c -o tvstat1

