/*Code to Understand Posix pthread Functions 
Author: Arundhathi Swami
Subject: Advanced Practical Embeddded Systems 
Date Started: 26 Sept 2017 
Date Last Modified: 5 Oct 2017 
credits: https://computing.llnl.gov\
Functions Used = pthread_create, pthread_join, pthread_exit, pthread_self, cond_init, cond_signal, cond_wait, cond_destroy*/

#include <pthread.h> 
#include <stdio.h> 
#include <stdint.h> 
#include <stdlib.h>
#include <limits.h> 
#include <unistd.h> 
#include <errno.h> 
#include <time.h> 

int32_t count = 0; 
pthread_mutex_t lock; //initializing mutex lock type
pthread_cond_t cond_var; //initializing condition variable 

//function template for entry point for the thread 
void *IncrementFunction(void* arg)
{ 
  pthread_mutex_lock(&lock);
  if (count == 0)
  {
   printf("Count in inc: %d\n", count); 
   pthread_cond_signal(&cond_var); //this function unblocks one thread that is blocked on the condition variable pointed to by cond_var i.e decrement function 
  }
  count += 1;  
  printf("In Increment thread, updated count value: %d\n", count);
  printf("Thread ID: %lu \n", pthread_self()); 
  pthread_mutex_unlock(&lock);
  pthread_exit(NULL); 
}

//function template for entry point for the thread 
void *DecrementFunction(void* arg)
{ 
  printf("Count in dec: %d\n", count); 
  pthread_mutex_lock(&lock); //locks the mutex to the current fucntion 
  while (count == 0)
  {
    printf("Count in dec while: %d\n", count);   
	 if (pthread_cond_wait(&cond_var, &lock))
	 { 
	    printf("Error: Wait Condition Failed to Initialize\n"); 
    } /*waits till count value becomes anything but zero and then executes the rest of the code. This function blocks on the condition variable. It also frees the  mutex lock till the condition is met. Paramters to this to this function: 1. pthread_cond_t *restrict condition_variable 
2. pthread_mutex_t *retsrict lock_vaiable */
  }
  count -= 1;  
  printf("In Decrement thread, updated count value: %d\n", count);
  printf("Thread ID: %lu \n", pthread_self());
  pthread_mutex_unlock(&lock); 
  pthread_exit(NULL); 
}


void main(int argc, char *argv[])
{
  //creating thread pool of 5 threads 
  pthread_t thread_to_join, calling_thread;  
  int32_t returnvalue; 

   if (pthread_mutex_init(&lock, NULL) != 0)
  {
    printf("Error: Mutex init failed\n");
     
  } 

   if (pthread_cond_init(&cond_var, NULL) != 0)//default attributes sent to condition variable initialzisation process. returns zero after success or errnos for errors 
  {
    printf("Error: Mutex init failed\n");
     
  } 
 
  //pthread_create allows for passing in a single argument to the thread start routine. The easiest way to pass multiple arguments is to create a structure with arguments as its members and pass in a pointer to the structure as the argument in pthread_create
  returnvalue = pthread_create(&calling_thread, NULL, IncrementFunction,NULL); 
 
  //arguments for pthread_create = 1.- Variable of type pthread_t, 2.- attributes, 3.-pointer to entry point function, 4.-pointer being passed as argument to the function 
//when successful, the function returns zero 
  if (returnvalue){
  	printf("Error: return code from pthread_create() is %d\n", returnvalue);
  }
   
  returnvalue = pthread_create(&thread_to_join, NULL, DecrementFunction,NULL); 
  if (returnvalue){
  	printf("Error: return code from pthread_create() is %d\n", returnvalue);
  }
  returnvalue = pthread_join(thread_to_join, NULL); 
  if (returnvalue){
  	printf("Error: Failed, return code from pthread_join() is %d\n", returnvalue);
  }
  returnvalue = pthread_join(calling_thread, NULL);  
  if (returnvalue){
  	printf("Error: Failed, return code from pthread_join() is %d\n", returnvalue);
  }
  
//pthread_join suspends execution of calling thread until target thread terminates. When a pthread executes successfully, the target(called) thread has been terminated after completing its function. The value passed to pthread_exit() by the terminating (called) thread, shall be made available in the location referenced by the second argument of pthread_join function. If the thread calling the join function is cancelled, then the target thread shall not be detached.

  int32_t ret1 = pthread_mutex_destroy(&lock); 
  int32_t ret2 = pthread_cond_destroy(&cond_var);  //destroys allocated condition variable    
  if (ret1 || ret2)
  {
		printf("Error: Destroy Conditions Failed\n"); 
  }

  pthread_exit(NULL); 
}

