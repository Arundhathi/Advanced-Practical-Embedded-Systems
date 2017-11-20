/* Date: 18th November 2017
 * Name: Arundhathi Swami 
 * Subject: APES
 * Author: Arundhathi Swami
 */

#include<stdio.h> 
#include<sys/types.h> 
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/dir.h>
#include<errno.h>


typedef struct pwm_opt{
  uint8_t power;
  uint8_t period;
  uint8_t dutycycle;
}pwm_characteristics; 

int main(int argc, char* argv[])
{//main opens
  char command[100]; 
  char typeofquery[200]; 
  char query[100]; 
  if (argc<2)
  {
    printf("Wrong parameters. Try again\n");
    exit(0); 
  }
  int socketfiledescriptor, portno, clientlength, ret, newsockfd;
  struct sockaddr_in server_address, client_address; 
  socketfiledescriptor = socket(AF_INET, SOCK_STREAM, 0); 
  if(socketfiledescriptor <0)
  {
    fprintf(stderr, "ERROR: SOcket Creation Failed\n");
    exit(1); 
  }
  bzero((char*)&server_address, sizeof(server_address)); 
  portno = atoi(argv[1]); 
  server_address.sin_family = AF_INET; 
  server_address.sin_port = htons(portno); 
  server_address.sin_addr.s_addr = INADDR_ANY; 

  if(bind(socketfiledescriptor, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)
  {
    fprintf(stderr, "ERROR: Binding Socket\n"); 
  }
  listen(socketfiledescriptor, 5); 
  clientlength = sizeof(client_address);
  int child;
  pwm_characteristics pwm;
  int filedescriptor; 

  while(1)
  {//while open

    newsockfd = accept(socketfiledescriptor, (struct sockaddr *)&client_address, &clientlength); 
    if(newsockfd <0)
    {//sock failed
      printf("Error: failed to connect\n"); 
      exit(1); 
    }
    else
    {//sock connected
      child = fork(); 
      if (child == 0)
      {//child initited
        printf("Progress: Created Child\n"); 
        close(socketfiledescriptor); 
        memset(command, '\0', sizeof(command)); 
        int rec = recv(newsockfd, command, sizeof(command), 0);
        if (rec<0)
        {
          printf("Error: Socket read\n"); 
        }
        printf("Command Received: %s\n", command);
        if (strncmp(command, "query", sizeof("query")) == 0)
        {
          printf("Progress: In query\n"); 
          /*read struct from char driver*/
          printf("Progress: Initiating read through character driver\n"); 
          filedescriptor = open("/dev/chardriver", O_RDWR); 
          if(filedescriptor < 0)
          {
            perror("Failed To Open Char Driver Device\n"); 
            return errno; 
          }
          ret = read(filedescriptor, &pwm, sizeof(pwm)); 
          if(ret < 0)
          {
            perror("Failed to read the message from the device\n"); 
            return errno;
          }
          ret = close(filedescriptor);
          if (ret == 0)
          {
            printf("Closed Driver\n"); 
          }

          printf("Read Power State = %d\n", pwm.power); 
          printf("Read Period = %d\n", pwm.period); 
          printf("Read Duty Cycle = %d\n", pwm.dutycycle); 
          
          /*send struct*/  
          rec = send(newsockfd, &pwm, sizeof(pwm), 0);
          if(rec<0)
          {
            printf("Error: Writing to socket\n"); 
          }

        }
        else if (strncmp(command, "modify", sizeof(query)) == 0)
        {
          printf("Progress: In modify\n"); 
          /*accept struct*/
          int rec = recv(newsockfd, &pwm, sizeof(pwm), 0);
          if(rec<0)
          {
            printf("Error: Writing to socket\n"); 
          }


          printf("Received Power State = %d\n", pwm.power); 
          printf("Received Period = %d\n", pwm.period); 
          printf("Received Duty Cycle = %d\n", pwm.dutycycle); 
          /* write struct*/
          filedescriptor = open("/dev/chardriver", O_RDWR); 
          if(filedescriptor < 0)
          {
            perror("Failed To Open Char Driver Device\n"); 
            return errno; 
          }

          printf("Progress: Writing Message t0 device\n"); 
          ret = write(filedescriptor, &pwm, sizeof(pwm)); 
          if(ret < 0)
          {
            perror("Failed to write using device driver\n"); 
            return errno; 
          }

          ret = close(filedescriptor);
          if (ret == 0)
          {
            printf("Closed Driver\n"); 
          }


          /*accept struct, write struct*/ 
        }

       /* printf("Power State = %d\n", pwm.power); 
        printf("Period = %d\n", pwm.period); 
        printf("Duty Cycle = %d\n", pwm.dutycycle); */
        
        
        
        close(newsockfd); 
        printf("Progress: Exiting child\n"); 
        exit(1); 
      }
      else
      {
        printf("Progress: Back to main\n"); 
        close(newsockfd); 
      }
    }
  }//while loop

  return 0; 
}//main closes
