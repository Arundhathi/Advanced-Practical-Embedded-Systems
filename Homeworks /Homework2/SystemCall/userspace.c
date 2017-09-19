#include <stdio.h> 
#include <linux/kernel.h> 
#include <sys/syscall.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <time.h> 
#include <stdint.h>
#define BUFFER_SIZE 256 


int main()
{
	int32_t* inbufptr; //pointer to the buffer that takes in the values to be sorted
	int32_t* outbufptr;  //pointer that accepts sorted values from the kernel level sys call 
	inbufptr = (int32_t*)malloc(BUFFER_SIZE*sizeof(int32_t));
	outbufptr = (int32_t*)malloc(BUFFER_SIZE*sizeof(int32_t)); 
	time_t timevar;
	uint32_t size; 
	size  = BUFFER_SIZE; 
	int8_t syscall_return; 
	
//initialising random number generator 
	
	srand((unsigned)time(&timevar)); 
	
//validating inputs before calling sys calls 
	if (size <= 0)
	{
		printf("Error: Invalid Length");
		return 0;  
	}
	if (inbufptr == NULL || outbufptr == NULL)
	{
		printf("ERROR: Invalid Memory Allocation"); 
		return 0; 
	
	}
	 
//starts allocating random values to the input buffer
	printf("Randomly Assigned Buffer:\n"); 
	for (int i = 0; i<BUFFER_SIZE; i++)
	{
		*(inbufptr+i) = (rand()%1000); 
		printf("%d\n", *(inbufptr+i)); 
	}


	//calling system call 
	syscall_return = syscall(333, inbufptr, outbufptr, size); 
	printf("Return Value of Successful Syscall is: %d\n",syscall_return); 

	printf("Array After sorting in system call is:\n"); 
	for (int i = 0; i<size; i++)
	{
		printf("Element %d is: %d\n", i, *(outbufptr+i)); 
	}
/*Testing sys call for invalid parameters*/
	printf("\nTesting validation of parameters in kernel space:\n"); 
	syscall_return = syscall(333, inbufptr, outbufptr, 0); 
	printf("Return Value of Syscall with zero length parameter is: %d\n",syscall_return); 
	int32_t* testoutNULL = NULL; 
	int32_t* testinNULL = NULL;  
	syscall_return = syscall(333, testinNULL, outbufptr, size); 
	printf("Return Value of Syscall with invalid input buffer is: %d\n",syscall_return); 
	syscall_return = syscall(333, inbufptr ,testoutNULL, size); 
	printf("Return Value of Syscall with invalid output buffer is: %d\n",syscall_return); 
	
	free(inbufptr);
	free(outbufptr);  			 
 		
}
