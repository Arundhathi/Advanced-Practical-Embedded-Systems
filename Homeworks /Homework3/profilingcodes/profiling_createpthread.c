/*Code to Profile Posix Functions 
Author: Arundhathi Swami
Subject: Advanced Practical Embeddded Systems 
Date Started: 30 Sept 2017 
Date Last Modified: 5 Oct 2017 
credits: https://computing.llnl.gov\
Functions Used = pthread_create, pthread_join, pthread_exit, pthread_self*/


#include <stdio.h> 
#include <stdint.h> 
#include <stdlib.h>
#include <limits.h> 
#include <unistd.h> 
#include <errno.h> 
#include <limits.h>
#include <time.h> 
#include <pthread.h> 

uint32_t count = 0; 

//function template for entry point to the thread 
void *ThreadFunction(void* arg)
{ 
  count += 1;  
  printf("In thread %d: Function Started\n", count);
  printf("Thread ID: %lu \n", pthread_self()); 
  pthread_exit(NULL); 
}

void main(int argc, char *argv[])
{
  //creating thread to join main
  pthread_t thread_to_join;  
  int32_t returnvalue;
  clock_t initiate, terminate, elapsed;  
 //clock_t: is a struct to store processor time. Basically an integer that tells you the relative number of clock ticks elapsed since epoch time. For linux epoch is 1st Janury 1970 

//clock(): It returns the implementations best approximation of processor time (in ticks) used by the process since the beginning of an implementation dependent time related to the process invocation. returns -1 if processor time returned is unreadable. In general PCs that run at 66MHz, the number of clock ticks per second is 66 Million. Number of ms per tick is also dependent on the OS and the installation of the OS currently running. 
  
  printf("Program to profile time required for pthread_create()\n");	
  initiate = clock(); //initiates clock
  printf("Initiated = %ld ticks\n", initiate); 
  returnvalue = pthread_create(&thread_to_join, NULL, ThreadFunction,NULL); //creates thread with default attributes
  if (returnvalue){
  	printf("ERROR; return code from pthread_create() is %d\n", returnvalue);
  }
   
  
  terminate = clock(); //terminates clock count 
  printf("Terminated = %ld ticks\n", terminate);
  elapsed = (terminate-initiate); 
  printf("Elapsed = %ld ticks\n", elapsed);  
//to gracefully exit 
  returnvalue = pthread_join(thread_to_join, NULL); //waits for thread to join back to main
  if (returnvalue){
  	printf("ERROR; return code from pthread_create() is %d\n", returnvalue);
  }
  pthread_exit(NULL); 
}


