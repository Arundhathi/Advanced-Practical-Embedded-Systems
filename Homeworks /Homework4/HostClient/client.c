/****************************
 * File: Client for COmmunication with BBG
 * Subject: APES
 * Author: Arundhathi Swami 
 * Date: November 18 2017 
 * *************************/ 
/*Date: 17th November 2017*/

#include<stdio.h>
#include<stdint.h> 
#include<stdlib.h> 
#include<string.h> 
#include<math.h> 
#include<unistd.h> 
#include<sys/types.h>
#include<sys/socket.h> 
#include<sys/stat.h> 
#include<netinet/in.h> 
#include<netdb.h> 
#include<fcntl.h>
#include <sys/dir.h>

#define MAX_CONNECTIONS 1
#define CONNECTION_TIME_LIMIT 10 /* TIMEOUT OF 2 SWCONDS INTERVAL*/
#define MAX_COMMAND_BUFFER_SIZE 100
#define DEFAULT_PERIOD 2
#define DEFAULT_POWER 0 //off
#define DEFAULT_DC 50

#define DEFAULT_BLINKY 1
#define DEFAULT_FREQUENCY 5
#define DEFAULT_DUTYCYCLE 50


void error(char* error_message) //prints out error related to system call 
{
	perror(error_message); 
	//exit(1); 
}


typedef enum ERRORS
{
	SUCCESS = 0,
	FAILURE, 
} error_t; 

typedef struct pwm_opt{
  uint8_t  power;
  uint8_t  period;
  uint8_t dutycycle;
} pwm_characteristics; 

int socketfiledescriptor[MAX_CONNECTIONS], portno, clientlength, returnvalue, newsockfd; 

error_t parseCommand(char incommand[MAX_COMMAND_BUFFER_SIZE], char filename[MAX_COMMAND_BUFFER_SIZE], char subdirectoryname[MAX_COMMAND_BUFFER_SIZE])
{
	int index = 0; 
	char* ptrin = incommand; 
	char store[MAX_COMMAND_BUFFER_SIZE];
	char storein[MAX_COMMAND_BUFFER_SIZE];
	ptrin = strtok(incommand, " \n"); 
	
	while (ptrin != NULL)
	{
		if (index == 0) 
		{
			strncpy(store, ptrin , strlen(ptrin)); 
		}
		else if (index == 1)
		{ 
			strncpy(filename, ptrin, strlen(ptrin)); 		
		}
		else 
		{
			strncpy(subdirectoryname, ptrin, strlen(ptrin)); 
		}
		index++; 
		
		ptrin = strtok(NULL, " ,-\n"); 
	
	}
	return SUCCESS; 
}


int main(int argc, char* argv[])
{//main begins
  if(argc < 3)
	{
		fprintf(stderr, "usage %s hostname port\n", argv[0]);
		exit(0); 
	}
	
  char command[100] = {}; 
	char power[50];//contains command both sent and received 
	memset(power, '\0', sizeof(power));
	char rcvd[5];//contains command both sent and received 
	memset(rcvd, '\0', sizeof(rcvd));
	char frequency[1];	//accepts file name from parser function
	memset(frequency, '\0', sizeof(frequency));
  char dcycle[1]; 
	memset(dcycle, '\0', sizeof(dcycle));
  char typeofquery[100]; 
	char query[100]; 
  char typeofmodification[100]; 
  char modify[100];
  char modificationvalue[100]; 
  int noofbytesent; 
	int activeservers[1] = {0};
	int nooffailedservers = 0;


	printf("hello\n");

      

	struct sockaddr_in server_address[MAX_CONNECTIONS], client_address; //struct sockaddr_in is a structure that contains an internet address 

	struct hostent *server; //hostent is a structure that defines a host computer on the internet 
  pwm_characteristics pwm; 
	pwm.power = DEFAULT_POWER; 
  pwm.period = DEFAULT_PERIOD;
  pwm.dutycycle = DEFAULT_DC;

  pwm_characteristics pwm_blinky; 
  pwm_blinky.power = DEFAULT_BLINKY; 
  pwm_blinky.period = DEFAULT_FREQUENCY;
  pwm_blinky.dutycycle = DEFAULT_DUTYCYCLE;  
  pwm_characteristics pwm_send; 
  pwm_characteristics pwm_rec; 
  while(1)
	{//while loop
		//section to send command to server 	
		printf("\n\nEnter Type of Command,Options are:\nQuery\nModify\n");
    memset(command, '\0', sizeof(command)); 
	  fgets(command, 100, stdin); //accepts string from command line 
		char* ptr_to_command = command;
	 	char* ptr_to_commands;
		int ret = parseCommand(ptr_to_command, frequency, dcycle);		
	  printf("Type of Command: %s\n", command);
   
    if (strncmp(command, "query", sizeof("query")) == 0)
    {//query
      printf("\n\nEnter Type of Query,Options are:\nSingle\nAll\n");
      memset(typeofquery, '\0', sizeof(typeofquery)); 
	    fgets(typeofquery, 100, stdin); //accepts string from command line 
		  ptr_to_command = typeofquery;
	 	  ptr_to_commands;
		  ret = parseCommand(ptr_to_command, frequency, dcycle);		
	    printf("Type of Query: %s\n", typeofquery);

      if(strncmp(typeofquery, "single", sizeof("single")) == 0)
      {
        printf("\n\nEnter Type of Single Query,Options are:\nPowerMode\nPeriod/Frequency\nDutyCycle \n");
        memset(query, '\0', sizeof(query)); 
	      fgets(query, 100, stdin); //accepts string from command line 
		    ptr_to_command = query;
	 	    ptr_to_commands;
		    ret = parseCommand(ptr_to_command, frequency, dcycle);		
	      printf("Query: %s\n", query);
      }
      else if(strncmp(typeofquery, "all", sizeof("all")) == 0)
      {
        strncpy(query, "all", sizeof("all")); 
      }
   }//query
   if (strncmp(command, "modify", sizeof("modify")) == 0)
    {//modify
     
      printf("\n\nEnter Type of Modification,Options are:\nPower\nPeriod/Frequency\nDutyCycle\nAll\nDefault\n");
      memset(typeofmodification, '\0', sizeof(typeofmodification)); 
	    fgets(typeofmodification, 100, stdin); //accepts string from command line 
		  ptr_to_command = typeofmodification;
	 	  ptr_to_commands;
		  ret = parseCommand(ptr_to_command, frequency, dcycle);		
	    printf("Type of Modification: %s\n", typeofmodification);

      if(strncmp(typeofmodification, "power", sizeof("power")) == 0)
      {
        printf("\n\nEnter Type of Power,Options are:\nOn(1)\nOff(0)\n");
        memset(modify, '\0', sizeof(modify)); 
	      fgets(modify, 100, stdin); //accepts string from command line 
		    ptr_to_command = modify;
	 	    ptr_to_commands;
		    ret = parseCommand(ptr_to_command, frequency, dcycle);		
        if (strncmp(modify, "1", sizeof("1")) != 0 && strncmp(modify, "0", sizeof("0")) != 0)
        {
          strncpy(modify, "0", sizeof("0")); 
        }
        printf("Power: %s\n", modify);
 
      	pwm_send.power = atoi(modify); 
        pwm_send.period = DEFAULT_PERIOD;
        pwm_send.dutycycle = DEFAULT_DC;


      }
      else if(strncmp(typeofmodification, "period", sizeof("all")) == 0)
      {
        printf("\n\nEnter Period or Frequency,Options are:\n1\n2\n");
        memset(modify, '\0', sizeof(modify)); 
	      fgets(modify, 100, stdin); //accepts string from command line 
		    ptr_to_command = modify;
	 	    ptr_to_commands;
		    ret = parseCommand(ptr_to_command, frequency, dcycle);		
	      printf("Period: %s\n", modify);
        if (atoi(modify) > 2)
        {
          strncpy(modify, "2", sizeof("2")); 
        }
        if (atoi(modify) < 1)
        {
          strncpy(modify, "1", sizeof("1")); 
        }
      	pwm_send.power = 1; 
        pwm_send.period = atoi(modify);;
        pwm_send.dutycycle = DEFAULT_DC;


      }
      else if (strncmp(typeofmodification, "dutycycle", sizeof("dutycycle")) == 0)
      {
        printf("\n\nEnter Duty Cycle upto 100%%\n");
        memset(modify, '\0', sizeof(modify)); 
	      fgets(modify, 100, stdin); //accepts string from command line 
		    ptr_to_command = modify;
	 	    ptr_to_commands;
		    ret = parseCommand(ptr_to_command, frequency, dcycle);		
	      printf("Duty Cycle: %s\n", modify);
        if (atoi(modify) > 100)
        {
          strncpy(modify, "100", sizeof("100")); 
        }
        if (atoi(modify) < 0)
        {
          strncpy(modify, "0", sizeof("0")); 
        }
        pwm_send.power = 1; 
        pwm_send.period = DEFAULT_PERIOD;
        pwm_send.dutycycle = atoi(modify);
     }
     else if(strncmp(typeofmodification, "all", sizeof("all")) == 0)
     { 
      printf("\n\nEnter Type of Power,Options are:\nOn(1)\nOff(0)\n");
      memset(modify, '\0', sizeof(modify)); 
	    fgets(modify, 100, stdin); //accepts string from command line 
		  ptr_to_command = modify;
	 	  ptr_to_commands;
		  ret = parseCommand(ptr_to_command, frequency, dcycle);	
      if (strncmp(modify, "1", sizeof("1")) != 0 && strncmp(modify, "0", sizeof("0")) != 0)
      {
        strncpy(modify, "0", sizeof("0")); 
      }
        pwm_send.power = atoi(modify); 
       
	    printf("Power: %s\n", modify);
      printf("\n\nEnter Period or Frequency,Options are:\n1\n2\n");
      memset(modify, '\0', sizeof(modify)); 
	    fgets(modify, 100, stdin); //accepts string from command line 
		  ptr_to_command = modify;
	 	  ptr_to_commands;
		  ret = parseCommand(ptr_to_command, frequency, dcycle);		
	    printf("Period: %s\n", modify);
      if (atoi(modify) > 2)
      {
        strncpy(modify, "2", sizeof("2")); 
      }
      if (atoi(modify) < 1)
      {
        strncpy(modify, "1", sizeof("1")); 
      }
      pwm_send.period = atoi(modify);

      printf("\n\nEnter Duty Cycle upto 100%%\n");
      memset(modify, '\0', sizeof(modify)); 
	    fgets(modify, 100, stdin); //accepts string from command line 
		  ptr_to_command = modify;
	 	  ptr_to_commands;
		  ret = parseCommand(ptr_to_command, frequency, dcycle);		
	    printf("Duty Cycle: %s\n", modify);
      if (atoi(modify) > 100)
      {
        strncpy(modify, "100", sizeof("100")); 
      }
      if (atoi(modify) < 0)
      {
        strncpy(modify, "0", sizeof("0")); 
      }
      pwm_send.dutycycle = atoi(modify);
    }
    else if(strncmp(typeofmodification, "default", sizeof("default")) == 0)
    { 
      pwm_send.power = 1; 
      pwm_send.period = DEFAULT_PERIOD;
      pwm_send.dutycycle = DEFAULT_DC;
    }
     printf("Sending: %d %d %d \n",pwm_send.power, pwm_send.period, pwm_send.dutycycle);


  }//modify




	  for (int i = 0; i< MAX_CONNECTIONS; i++)
		{
			portno = atoi(argv[2]); //concerts all 4 port numbers to integers 
			//argc checks the port number validity and existence 	
			if ((socketfiledescriptor[i] = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			{
				error("ERROR: Couldnt Create Socket\n"); 
			}//creates the socket
			server_address[i].sin_family = AF_INET;  //assigns af_inet as the address family 
			server = gethostbyname(argv[1]); 
			if (server == NULL)
			{
				fprintf(stderr, "ERROR: No such host found\n"); 
			}
		
			bcopy((char*)server->h_addr, (char*)&server_address[i].sin_addr, server->h_length); 
	
			server_address[i].sin_port = htons(portno); //assigns port number to appropriate structure member converting it from host name to network resolvable name 
   
			if (connect(socketfiledescriptor[i], (struct sockaddr *)&server_address[i], sizeof(server_address[i]))<0)
			{
				error("Error: Connecting to the server socket\n"); 
				activeservers[i] = 1; 
				nooffailedservers++;
			}
	   	clientlength = sizeof(struct sockaddr_in);
			//setting timeout of 1s
			struct timeval time; 
			time.tv_sec = CONNECTION_TIME_LIMIT; 
			time.tv_usec = 0;
	
			if(setsockopt(socketfiledescriptor[i], SOL_SOCKET, SO_RCVTIMEO, (char *) &time, sizeof(time)) < 0)
			{
				printf("Error: Setting socket options\n"); 
			}
   	}//end of socket creation for all 4 servers 
		printf("No of Failed Server Connections: %d\n", nooffailedservers);
    for (int i =0; i<MAX_CONNECTIONS; i++)
    {
      /*sending whether query or modify command*/
       int sent = send(socketfiledescriptor[i], command, sizeof(command), MSG_NOSIGNAL);
    	 if (sent < 0)
		 {
    		printf("Error: Writing to socket\n"); 
		 }
		 if (strncmp(command, "query", sizeof("query")) == 0)
    	 {//query
         /*receive struct, decode*/ 
			
			int rec = recv(socketfiledescriptor[i], &pwm_rec,sizeof(pwm_rec), 0);
			if(rec<0)
			{
				printf("Error: reading from socket\n"); 
			}
			if(strncmp(typeofquery, "all", sizeof("all")) == 0)
			{
				printf("PWM Parameters are as follows:\n"); 
				printf("PWM Power Mode(Interpret as 1 = ON, 0 = OFF: %d\n", pwm_rec.power); 
				printf("PWM Period/Frequency(Interpret in seconds): %d\n", pwm_rec.period); 
				printf("PWM Duty Cycle(Interpret in %%): %d\n", pwm_rec.dutycycle); 
			}
			else if(strncmp(typeofquery, "single", sizeof("single")) == 0)
			{
				if (strncmp(query, "powermode", sizeof("powermode")) == 0)
				{
					printf("PWM Power Mode(Interpret as 1 = ON, 0 = OFF: %d\n", pwm_rec.power); 
				}
				if (strncmp(query, "period", sizeof("period")) == 0)
				{
					printf("PWM Period/Frequency(Interpret in seconds): %d\n", pwm_rec.period); 
				}
				if (strncmp(query, "dutycycle", sizeof("dutycycle")) == 0)
				{
					printf("PWM Duty Cycle(Interpret in %%): %d\n", pwm_rec.dutycycle); 
				}
			}
		 }//query ends
		 if (strncmp(command, "modify", sizeof("modify")) == 0)
		 {//modify begins
			sent = send(socketfiledescriptor[i], &pwm_send, sizeof(pwm_send), 0);	
			if(sent < 0)
			{
				printf("Error: Writing to socket\n"); 
			}
		 }//modify ends
	 }//for loop
  }//while loop
  return 0; 
}//main  loop
