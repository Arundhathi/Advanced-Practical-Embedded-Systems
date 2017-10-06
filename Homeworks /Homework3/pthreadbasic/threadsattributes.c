/*Code to understand basics of POSIX - Pthreads 
Author: Arundhathi Swami
Subject: Advanced Practical Embeddded Systems 
Date Started: 26 Sept 2017 
Date Last Modified: 5 Oct 2017 
Functions Used = pthread_create, pthread_join, pthread_exit, pthread_self, pthread_attr_setattr, pthread_attr_getattr, pthread_attr_init*/

#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <sys/types.h>
#include <unistd.h> 
#include <sys/syscall.h>
#include <limits.h> 
#include <errno.h>  

int32_t count = 0; 


void *FunctionCalled(void* arg)
{
	count += 1; 	
	int32_t returnvalue;	
//getattr and setattr : an attribute is an opaque object and cannot be modified by assignment. A set of functions is provided to initialise, configure and destroy each object type. Once an attribute is initialized and configured, it has process-wide scope
   //providing deafult attribute values is done 
	pthread_attr_t tattr; //vaiable for changed attribute  
   pthread_attr_t dattr; //variable fpr default attribute
   returnvalue = pthread_attr_init(&tattr); 
   returnvalue = pthread_attr_init(&dattr); 
   size_t stacksize = (PTHREAD_STACK_MIN+ 0x5000);
   size_t got_stack = 0;
   size_t default_stack = 0;
	
	printf("In the Function: %d with ID %lu\n", count, pthread_self()); 
	returnvalue = pthread_attr_getstacksize(&dattr, &default_stack);
   if (returnvalue){
  	printf("Error: Get Stack size Failed %d\n", returnvalue);
   } 
   printf("Default Stack Size: %zu\n", default_stack); 
   int32_t detachstate; 

   returnvalue = pthread_attr_getdetachstate(&dattr, &detachstate); 
   if (returnvalue){
  	printf("Error: Get Stack Detach State Failed %d\n", returnvalue);
   } 
	if (detachstate == 0)
	{
		char dstate[] = "PTHREAD_CREATE_JOINABLE"; 
      printf("Default Detachment State: %s\n", dstate);
	}
	else 
	{
		char dstate[] = "PTHREAD_CREATE_DETACHED";
      printf("Default Detachment State: %s\n", dstate);
	} 
   

//default detachment states are defined in an enum initialized in the pthread.h library. the members of the enum are 
//1.PTHREAD_CREATE_JOINABLE = 0
//2. PTHREAD_CREATE_DETACHED = 1
//Note: Threads are joinable by default 

	
	//sets the stack size to a different value than default 
   returnvalue = pthread_attr_setstacksize(&tattr, stacksize); 
   if (returnvalue){
  	printf("Error: Set Stack size Failed %d\n", returnvalue);
   }
	returnvalue = pthread_attr_getstacksize(&tattr, &got_stack);
   if (returnvalue){
  	printf("Error: Get Stack size Failed %d\n", returnvalue);
   }
   printf("Modified Stack Size: %zu\n", got_stack); 
	pthread_exit(NULL); 

}

int main()
{
	//creating thread pool of 5 threads 
  pthread_t thread_to_join, calling_thread;  
  int32_t returnvalue; 
 
	//pthread_create allows for passing in a single argument to the thread start routine. The easiest way to pass multiple arguments is to create a structure with arguments as its members and pass in a pointer to the structure as the argument in pthread_create
  returnvalue = pthread_create(&calling_thread, NULL, FunctionCalled,NULL); 
 
  //arguments for pthread_create = 1.- Variable of type pthread_t, 2.- attributes, 3.-pointer to entry point function, 4.-pointer being passed as argument to the function 
//when successful, the function returns zero 
  if (returnvalue){
  	printf("ERROR; return code from pthread_create() is %d\n", returnvalue);
  }
   
  returnvalue = pthread_create(&thread_to_join, NULL, FunctionCalled,NULL); 
  if (returnvalue){
  	printf("ERROR; return code from pthread_create() is %d\n", returnvalue);
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
  return 1;  
	 
}






























