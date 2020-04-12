#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

sem_t *mutex;
char *buffer1;
char *buffer2;

void readf1(char* f1)
{	
	sem_wait(mutex);
	int fd=open(f1,O_RDONLY);
	if(fd<0)
	{
		printf("not opened");
	}
	int size= lseek(fd,0,SEEK_END);
	close(fd);
	fd=open(f1,O_RDONLY);
	char * buf = (char *) malloc(size);
	read(fd,buf,size);
	close(fd);
	buf[size]='\0';
	strcat(buffer1, buf);
}

void readf2(char* f2)
{
	sem_wait(mutex);
	int fd=open(f2,O_RDONLY);
	if(fd<0)
	{
		printf("not opened");
	}
	int size= lseek(fd,0,SEEK_END);
	close(fd);
	fd=open(f2,O_RDONLY);
	char * buf = (char *) malloc(size);
	read(fd,buf,size);
	close(fd);
	strcat(buffer1, buf);	
}

int main(int argc,char** args)
{
	if(argc==3)
	{
		char*f1=args[1];
		char*f2=args[2];

		pid_t pid1,pid2,pid3,pid4;

		int id_mutex=shmget(904, 1024, 0666 | IPC_CREAT | IPC_EXCL);
		int id_buffer1=shmget(918, 1024, 0666 | IPC_CREAT | IPC_EXCL);
		int id_buffer2=shmget(934, 1024, 0666 | IPC_CREAT | IPC_EXCL);

		if (id_mutex==-1 || id_buffer1==-1 || id_buffer2==-1)
		{   
			printf("ERROR!");
			return 1;
		}

		mutex= (sem_t*) shmat(id_mutex, NULL, 0);
		sem_init(mutex, 1, 1);
		
		buffer1=(char*) shmat(id_buffer1, NULL, 0);
		buffer2=(char*) shmat(id_buffer2, NULL, 0);

		pid1=fork();
		if(pid1!=0)
		{
			pid2=fork();
		}
		if((pid1!=0)&&(pid2!=0))
		{
			wait(NULL);
			pid3=fork();
		}
		if((pid1!=0)&&(pid2!=0)&&(pid3!=0))
		{
			wait(NULL);
			pid4=fork();
		}
		
		if(pid1==0)
		{
			printf("I am process A and I am reading file-1.txt and writing to buffer1\n");
			readf1(f1);
			sem_post(mutex);
		}
		if(pid2==0)
		{
			printf("I am process B and I am reading file-2.txt and writing to buffer1\n");
			readf2(f2);
			sem_post(mutex);	
		}
		if(pid3==0 && pid2>0 && pid1>0)
		{
			printf("I am process C and I am reading buffer1 and writing on buffer2\n");
			strcpy(buffer2,buffer1);
		}
		if(pid4==0 && pid2>0 && pid1>0 && pid3>0)
		{
			printf("I am process D and I am reading buffer2 and writing on screen\n");
			printf("%s",buffer2);
		}

		if((pid1!=0)&&(pid2!=0))
		{
			wait(NULL);
		}

		sem_destroy(mutex);
		shmdt(mutex);
		shmctl(id_mutex, IPC_RMID, NULL);

		shmdt(buffer1);
		shmctl(id_buffer1, IPC_RMID, NULL);

		shmdt(buffer2);
		shmctl(id_buffer2, IPC_RMID, NULL);
	}
	else
		printf("Invalid Input! Enter both file names\n");

	return 0;
}
