//Code to understand basics of POSIX - Pthreads 
//Date: 26th Sept 2017 
//credits: https://computing.llnl.gov\
//Functions Used = pthread_create, pthread_join, pthread_exit, pthread_self, mutex_init, mutex_lock, mutex_unlock, mutex_trylock 



#include <pthread.h> 
#include <stdio.h> 
#include <stdint.h> 
#include <stdlib.h>
#include <limits.h> 
#include <unistd.h> 
#include <errno.h> 
#include <time.h> 

#define NUM_OF_THREADS 5 
int count = 0; 
pthread_mutex_t lock; 

//function template for entry point for the thread 
void *ThreadFunction(void* arg)
{ 
  int* id = (int*)arg; 
  int it = 0; 
  printf("\nCalling Function Started: %d\n", *id);   
  if (pthread_mutex_trylock(&lock)==0) 
  {
	// printf("Thread ID: %lu \n", pthread_self()); 
  	for ( it = 0; it<100000; it++); 
  	count += 1;  
  	printf("In Calling thread %d: Count:%d\n", *id, count);
  	pthread_mutex_unlock(&lock); 
  }
  printf("Calling Function Ended: %d\n", *id); 
  pthread_exit(NULL); 
}

//function template for entry point for the joined threads 
void *JoinedThreadFunction(void* argu)
{ 
  pthread_mutex_lock(&lock); 
  int* idt = (int*)argu;
  int i = 0;  
  printf("\nJoined Function Started: %d\n", *idt); 
 // printf("Thread ID: %lu \n", pthread_self()); 
  for ( i = 0; i<100000; i++); 
  count += 1; 
  printf("In Joined thread %d: Count:%d\n", *idt, count);
  printf("Joined Function Ended: %d\n", *idt); 
  pthread_mutex_unlock(&lock); 
  idt  = NULL;
  pthread_exit(NULL); 
}


void main(int argc, char *argv[])
{
  //creating thread pool of 5 threads 
  pthread_t thread_to_join[NUM_OF_THREADS], calling_thread;  
  int returnvalue; 
  int tid = 10;
  int n = 0; 
	
  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    printf("Error: Mutex init failed\n");
     
  } 
	//pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr): return type is int. if return is anything other than 0, initialisation of mutex failed, initializes the mutex object pointed to by mutex according to mutex attributes specified in mutexattr. Mutexes can be of three types that determine if the mutex can be locked by the same thread that unlocked it. Default type is normal which means that a thread which already owns the mutex will get suspended forever if it tries to relock the mutex. 
   //pthread_mutex_lock(pthread_mutex_t *mutex): locks the given mute. if the mutex is currently unlocked, it becomes locked andnowned by the calling thread. if the mutex is currently locked by another thread, pthread_mutex_lock suspends the calling thread until the mutex is unlocked by the thread currently owning it       
  //pthread_mutex_trylock(pthread_mutex_t *mutex): if the mutex is locked by another thread, it doesnt suspend the calling thread. It returns with the error code EBUSY 

  //pthread_mutex_unlock(pthread_mutex_t *mutex): unlocks the mutex. if the mutex attribute is normal or fast, unlock returns to the unlocked stage, if of the recursive type it decrements the locking count of the mutex and when this count reaches zero it actually unlocks the mutex

	//pthread_mutex_destroy(pthread_mutex_t *mutex): destroys mutex object freeing the resources it might hold. Any mutex that needs to be destroyed must be unlocked first 

  //pthread_create allows for passing in a single argument to the thread start routine. The easiest way to pass multiple arguments is to create a structure with arguments as its members and pass in a pointer to the structure as the argument in pthread_create
  returnvalue = pthread_create(&calling_thread, NULL, ThreadFunction, (void *)&tid); 
 
  //arguments for pthread_create = 1.- Variable of type pthread_t, 2.- attributes, 3.-pointer to entry point function, 4.-pointer being passed as argument to the function 
//when successful, the function returns zero 
  if (returnvalue){
  	printf("ERROR; return code from pthread_create() is %d\n", returnvalue);
  }
   
  for (n = 0; n<NUM_OF_THREADS; n++)
  {
		returnvalue = pthread_create(&thread_to_join[n], NULL, JoinedThreadFunction,(void*)&n); 
  		if (returnvalue){
  		printf("ERROR; return code from pthread_create() is %d\n", returnvalue);
  		}
  }
//  returnvalue = pthread_join(calling_thread, NULL); 
  for (n = 0; n<NUM_OF_THREADS; n++)
  {
	returnvalue = pthread_join(thread_to_join[n], NULL); 
  } 
  returnvalue = pthread_join(calling_thread, NULL); 
  
//pthread_join suspends execution of calling thread until target thread terminates. When a pthread executes successfully, the target(called) thread has been terminated after completing its function. The value passed to pthread_exit() by the terminating (called) thread, shall be made available in the location referenced by the second argument of pthread_join function. If the thread calling the join function is cancelled, then the target thread shall not be detached.    
  		
  pthread_mutex_destroy(&lock); 
  pthread_exit(NULL); 
}


