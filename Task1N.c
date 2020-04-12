#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include<time.h>



int potentialCPatients=0;
sem_t coronaPatient;
sem_t fluPatient;
sem_t mutex;

int Test(int i)
{
	return (rand() % 2 + rand() % 2) %2; 	
}

void* myFunc(void* arg)
{
	int *a = (int*) arg;

	sem_wait(&mutex);
		potentialCPatients++;
	sem_post(&mutex);

	if(Test(*a)==1)
	{
		  sem_post(&coronaPatient);
		  sem_wait(&mutex);
			potentialCPatients--;
		  sem_post(&mutex);

		  printf("\nPatient %d  is a Corona Patient\n", *a) ;
		  printf("As we are concerned with the recovery of Corona Patient\n");
		  printf("We will take two more tests for this purpose\n");

		  if(Test(*a)==0 && Test(*a)==0)
		  {
			sem_wait(&coronaPatient);
			printf("Both tests results are false so it has been shown that patient %d has recovered from Corona\n", *a);
		  }
		  else
		  {
			printf("Both tests results are not false so it has been shown that patient %d has not recovered from Corona\n", *a);
		  }
	}
	else
	{
		  sem_post(&fluPatient);
		  sem_wait(&mutex);
			potentialCPatients--;
		  sem_post(&mutex);
		  printf("\nPatient %d  is a Flu Patient\n", *a) ;
	}
}


int main(int argc , char * argv[])
{
	srand(time(NULL));
	if(argc==2)
	{
		char *a = argv[1];
  		int N = atoi(a);

		printf("No Of Patients are %d\n", N);
		sem_init(&coronaPatient, 0, 0);
	  	sem_init(&fluPatient, 0, 0);
		sem_init(&mutex, 0, 1);

		pthread_t tid[N];
		int args[N];
		for(int i=0;i<N;i++)
		{
			args[i] = i+1;
			if (pthread_create(&tid[i], NULL, &myFunc, &args[i])==-1)
			{
			      printf("Thread Creation Failed!\n");
			      return 1;
			}
			pthread_join(tid[i],NULL);
		}
		printf("\n");
	}
	else
		printf("Invalid Input \n");
	return 0;
}

