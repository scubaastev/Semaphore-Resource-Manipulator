//Steven Lang
//CECS 326-01
//Semaphore Program
//steven.lang@student.csulb.edu
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "semaphore.h"
#include <sys/ipc.h>
#include <sys/shm.h>
using namespace std;
enum {shared0, shared1, shared2, shared3}; //set up names of 4 semaphores



void parent_cleanup(SEMAPHORE &, int);
float getBeta(float, float);
int getRandom(int, int);


int main() {
	//create 
	SEMAPHORE sem(4);

	//increment each shared resource semaphore by 1
	sem.V(shared0);
	sem.V(shared1);
	sem.V(shared2);
	sem.V(shared3);
	//use ftok() function to return a key based on path and id
	key_t key = ftok("shmfile", 65);

	//initialize all necessary variables
	int shmid;
	long childpid =1;
	long wpid;
	int status = 0;
	float randBeta;
	int randNum;
	int randMem;
	int parentpid = getpid();


	//parameters for shmget:
	// key
	// size
	// shmflg
	//make shared memory segment and store it as shmid
	shmid = shmget(key, sizeof(float), IPC_CREAT | 0666);
	if (shmid == -1)
	{
		perror("shmget failed: ");
		exit(0);
	}
	// parameters for shmat:
	// int shmid
	// const void* shmaddr
	// int shmflg
	//attach the shared memory segment with an array of floats
	void *sharedFloats = (void *)0;
	sharedFloats = (float*)shmat(shmid, NULL, 0);
	static float *shTest;
	shTest = (float *)sharedFloats;
	
	//initialize each of the 4 float variables/shared resources to some non-zero value
	shTest[0] = 1.5;
	shTest[1] = 2.5;
	shTest[2] = 3.5;
	shTest[3] = 4.5; 


	for (int i = 0; i < 10; i++)
	{
		//create child processes for each time in the for loop
		childpid = fork();

		
		if (childpid==0) //child process
		{
			printf("[son] pid %d from [parent] pid %d\n", getpid(), getppid());
			//have each child process interate in the loop for some fixed number (4) of times
			for (int k = 0; k < 4; k++)
			{
				//generate a random value beta between -0.5 and 0.5
				randBeta = getBeta(-0.5, 0.5);
				//select one random shared variable				
				randNum = getRandom(0, 3);
				int number = randNum;
				//depending on the shared variable, execute P (wait) semaphore for corresponding shared variable
				switch(number)
				{
					case 0:
						sem.P(shared0);

						break;
					case 1:
						sem.P(shared1);
						break;
					case 2:
						sem.P(shared2);

						break;
					case 3:
						sem.P(shared3);

						break;
				}	
				cout<<"PROCESS "<<getpid()<<" OPERATING ON SHARED MEMORY #"<<number<<endl;
				cout<<" INITIAL SHARED MEMORY VALUE: "<<shTest[number]<<endl;
				//update by adding a fraction of its current value, according to value of the beta				
				shTest[number] = shTest[number] + (shTest[number] * randBeta);
				cout<<" UPDATED SHARED MEMORY VALUE: "<<shTest[number]<<endl;
				//depending on the shared variable, execute V (signal) semaphore for corresponding shared variable				
				switch(number)
				{
					case 0:
						sem.V(shared0);

						break;
					case 1:
						sem.V(shared1);

						break;
					case 2:
						sem.V(shared2);

						break;
					case 3:
						sem.V(shared3);

						break;
				}	

				
			}
				exit(0);

		}
		// return -1 if it did not fork successfully
		if (childpid < 0)
		{
			cout << "There was a spawn error, child did not fork successfully." << endl;
		}
		
	}

	while((wpid=wait(&status))>0);
	//after all the processes finish remove semaphores and shared memory
	cout<<"only parent process will reach this far"<<endl;
	parent_cleanup(sem, shmid);
	exit(0);



}




void parent_cleanup(SEMAPHORE &sem, int shmid) {

	int status;	/* child status */
	wait(0);	/* wait for child to exit */
	shmctl(shmid, IPC_RMID, NULL);	/* cleaning up */
	sem.remove();
} // parent_cleanup,


//generate a random float beta between the min and max
float getBeta(float min, float max)
{
	return (float)rand()/RAND_MAX;


}

//generate a random integer between min and max
int getRandom(int min, int max)
{
	return rand() % (max - min + 1) + min;
}
