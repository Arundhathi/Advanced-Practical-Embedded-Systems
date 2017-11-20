/*user space code to text char driver*/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h> 
#include<unistd.h> 

#define BUFFER_LENGTH 256
static char buffer[BUFFER_LENGTH];
//typedef struct pwm_change{
  
int main()
{
  int ret, filedescriptor; 
  char Send[BUFFER_LENGTH]; 
  printf("Progress: Starting device test code\n"); 
  filedescriptor = open("/dev/chardriver", O_RDWR); 
  if(filedescriptor < 0)
  {
    perror("Failed To Open Char Driver Device\n"); 
    return errno; 
  }
  printf("Type in a short string to send to the device\n"); 
  scanf("%[^\n]%*c", Send); 
  printf("Progress: Writing Message (%s) to device\n", Send); 
  ret = write(filedescriptor, Send, strlen(Send)); 
  if(ret < 0)
  {
    perror("Failed to write using device driver\n"); 
    return errno; 
  }

  printf("Press Enter to read back from device\n"); 
  ret = read(filedescriptor, buffer, BUFFER_LENGTH); 
  if(ret < 0)
  {
    perror("Failed to read the message from the device\n"); 
    return errno;
  }
  printf("Received message is: [%s]\n", buffer); 
 
  ret = close(filedescriptor);
  if (ret == 0)
  {
    printf("Closed Driver\n"); 
  }

  return 0; 
}


