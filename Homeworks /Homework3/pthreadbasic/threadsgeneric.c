/*Code to Understand Posix Pthread Functions 
Author: Arundhathi Swami
Subject: Advanced Practical Embeddded Systems 
Date Started: 26 Sept 2017 
Date Last Modified: 5 Oct 2017 
credits: https://computing.llnl.gov\
Functions Used = pthread_create, pthread_join, pthread_exit, pthread_self*/

#include <pthread.h> 
#include <stdio.h> 
#include <stdint.h> 
#include <stdlib.h>
#include <limits.h> 
#include <unistd.h> 
#include <errno.h> 

#define NUM_OF_THREADS 2 
int32_t count = 0; 

//function template for entry point for the thread 
void *ThreadFunction(void* arg)
{ 
  count += 1;  
  printf("In thread %d: Function Started\n", count);
  printf("Thread ID: %lu \n", pthread_self()); 
  pthread_exit(NULL); 
}

void main(int argc, char *argv[])
{
  //creating thread pool of 5 threads 
  pthread_t thread_to_join, calling_thread;  
  int32_t returnvalue; 
 
  //pthread_create allows for passing in a single argument to the thread start routine. The easiest way to pass multiple arguments is to create a structure with arguments as its members and pass in a pointer to the structure as the argument in pthread_create
  returnvalue = pthread_create(&calling_thread, NULL, ThreadFunction,NULL); 
 
  //arguments for pthread_create = 1.- pointer to unique Variable of type pthread_t which contains thread identifier number , 2.- attributes, 3.-pointer to entry point function, 4.-pointer being passed as argument to the function 
//when successful, the function returns zero 
  if (returnvalue){
  	printf("ERROR: return code from pthread_create() is %d\n", returnvalue);
  }
   
  returnvalue = pthread_create(&thread_to_join, NULL, ThreadFunction,NULL); 
  if (returnvalue){
  	printf("ERROR: return code from pthread_create() is %d\n", returnvalue);
  }
  
  returnvalue = pthread_join(calling_thread, NULL); 
  if (returnvalue){
  	printf("Error: Failed, return code from pthread_join() is %d\n", returnvalue);
  }
  
  returnvalue = pthread_join(thread_to_join, NULL); 
  if (returnvalue){
  	printf("Error: Failed, return code from pthread_join() is %d\n", returnvalue);
  }
  
  
//pthread_join suspends execution of calling thread until target thread terminates. When a pthread executes successfully, the target(called) thread has been terminated after completing its function. The value passed to pthread_exit() by the terminating (called) thread, shall be made available in the location referenced by the second argument of pthread_join function. If the thread calling the join function is cancelled, then the target thread shall not be detached.    
  
     pthread_exit(NULL); 
}


