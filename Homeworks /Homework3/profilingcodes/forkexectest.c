/* Source Code for Profiling Time Required for Fork and Exec commands 
Author: Arundhathi Swami
Subject: Advanced Practical Embeddded Systems 
Date Started: 28 Sept 2017 
Date Last Modified: 4 Oct 2017 
Function: Uses the clock_t struct to hold values of ticks derived from clock() function 
*/


#include <stdio.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <stdlib.h> 
#include <time.h> 
#include <sys/wait.h> 
#include <stdint.h> 


uint32_t gcount = 2; 

int main()
{
  uint32_t count = 20; 
  int32_t returnvalue; 
  clock_t initiate, terminate, elapsed;
  //clock_t: is a struct to store processor time. Basically an integer that tells you the relative number of clock ticks elapsed since epoch time. For linux epoch is 1st Janury 1970 
  printf("Code to profile time taken for fork() and exec() functions\n"); 
  
//clock(): It returns the implementations best approximation of processor time (in ticks) used by the process since the beginning of an implementation dependent time related to the process invocation. returns -1 if processor time returned is unreadable. In general PCs that run at 66MHz, the number of clock ticks per second is 66 Million. Number of ms per tick is also dependent on the OS and the installation of the OS currently running.
  initiate = clock(); 
  printf("Initiated: %ld ticks\n", initiate);  
	//fork(): functions to spawn a new process identical to the parent process except that it has a new system process id. the process is copied in memory from the parent and a new process structure is assigned by the kernel to the child thread. the return value of the fork function is what discriminates the two threads of execution. a zero is returned by the fork function in the childs process and it returns  value greater than zero in the parent process. hence both functions get executed 
  pid_t pID = fork(); 
  printf("Child Process ID: %d\n", pID);
  if (pID == 0) 
  {
    printf("Child Initiated\n"); 
    count++; 
    gcount++;
    //execvp(const char *file, char *const argv[])
    //file points to the file name (executable) associated with the file being executed. 
    //argv is a null terminated array of character pointers 
    //this command enables the created child process to not have to run the same program as the parent process does. the exec type system calls allow a process to run any program files including a binary executable or shell script 
    char *args[]={"pwd", NULL}; 
    returnvalue = execvp(args[0], args); //exec returns a value only if it fails 
    if(returnvalue)
	 {
		printf("Error: Execvp failed\n"); 
	 }
  }
  else if (pID < 0) 
  {
    printf("Child Init Failed\n"); 
  }
  else 
  {
    printf("Parent Process\n"); 
  }
  //as soon as the execvp statement occurs in the program, the parent program is replaced by the execvp executable program. the rest of the parent program remains unexecuted
  int status;
  wait(&status); //wait for child thread to return  
  terminate = clock(); 
  printf("Global Count: %d\n", gcount); 
  printf("Local Count: %d\n", count);
  printf("Terminated: %ld ticks\n", terminate); 
  printf("Total time elapsed = %ld ticks\n", (terminate - initiate)); 
  return 1; 
}
