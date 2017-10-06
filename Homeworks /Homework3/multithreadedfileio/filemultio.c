/* Multi Threaded File Read Write Source Code 
Author: Arundhathi Swami
Subject:Advanced Practical Embeddded Systems 
Date Started: 29 Sept 2017 
Date Last Modified: 4 Oct 2017 
Function: This Source code allows user to write to a file and on signal CTRL+D manages two threads to read & count file contents and to print out the file stats respectively
Mapping Algorithm Credit: https://www.cs.utexas.edu/users/djimenez/utsa/cs1713-3/c/wordfreq.txt
*/

#include<stdio.h> 
#include<stdint.h> 
#include<pthread.h>
#include<unistd.h>
#include<limits.h>
#include<string.h> 
#include<stdlib.h> 
#include<signal.h> 
#include<stdbool.h> 
#include<ctype.h>

//Macros 
#define MAX_WORD_LENGTH 100 //defines maximum length a word from the file can be 
#define MAX_WORD_COUNT 1024 //maximum number of words that can be sorted into the map 
#define TOP_SORTED 5 //the number of most recurring words to be found 

//Data struct to store stats of file 
typedef struct data_counter{
  int32_t no_of_char; //collects number of chars
  int32_t no_of_words; //collects number of words 
  int32_t no_of_lines;//collects number of lines 
 }data_t; 

//Data struct to map the dictionary of words  
typedef struct map{
  char word[MAX_WORD_LENGTH]; //char array to hold word 
  int32_t occurenceCount; //int to store number of times word was repeated
}map_t; 

//global Variables
int32_t killsig = 0;  //global exit signal 
pthread_t readthread, printthread; //initialises two new threads
pthread_mutex_t lock; //initialises a mutex called lock 
FILE* fnew; //file pointer to new file 

volatile data_t current; //instance of struct that holds file stats 

map_t wordlist[MAX_WORD_COUNT]; //each word wil be assigned a struct to hold the word and its occurenceCount
char word_store[1000];//char array to store individual words 
int32_t iterate, no_of_highest_repeating_words; //ints to helps iterate the mapping loop and to set the highest number of repeating words which can either be 0 or 5 in this program  
int32_t no_of_words_to_process = 0; // int to update the number of words to process

//Function to insert a word into word map and count its occurence instances 
//Paramters: struct map type pointer for every word, pointer to number of words processed into the map, char pointer to word that needs to be processed 
void wordmapFunction(map_t *words, int32_t* no_of_words, char* inword)
{
  //check for validity of received word 
  if (strlen(inword) >= MAX_WORD_LENGTH || *no_of_words >=MAX_WORD_COUNT)
  {
    printf("Error: Invalid Word Encountered in Mapping\n");
    exit(1); 
  }

  //search for word in list of all present words and increment occurence count if present 
  for(int32_t i=0; i< *no_of_words; i++)
  {
    if (strcmp(inword, words[i].word) == 0)//word found
    {
      words[i].occurenceCount++;
      return;
    }
  }//for loop ends

  //if word wasnt in the map, add word to the map and set base occurence count to 1 
  strcpy(words[*no_of_words].word, inword); 
  words[*no_of_words].occurenceCount = 1; 

  //increment number of words 
  (*no_of_words)++;
}//end of mapping function 

//Function to sort word occurence counts in descending order as part of qsort
//Parameters: map struct type pointers to consecutive words' structs
int32_t wordSort(map_t *first, map_t *second)
{
  if(first->occurenceCount < second->occurenceCount)
    return +1; 

  if(first->occurenceCount > second->occurenceCount)
    return -1; 

}//function ends 

//Function to check if chars of word are alphabets or symbol chars
//Parameters: individual char c 
int32_t alphabetSort(char c)
{
  if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') //
  {
    return 1; 
  }
  else 
  {
    return 0; 
  }
}

//Function to remove a non alphabetical character from the word 
//Parameters: pointer to char that needs to be removed, int number of iterations till  specific char is removed 
void removeChar(char *c, int32_t n)
{
  while(c[n])
  {
    n++;
    c[n-1] = c[n]; //shifts all characters after nth character that needs to be removed 
  }
  c[n] = 0; //last char value is made 0 
}


//Function to weed out non alphabetic chars
//Paramters: Pointer to char or word  
void nonalphabetFilter(char *c)
{
  int32_t i;
  for (i=0; c[i]; i++) //iterates over the length of the word 
  {
    if(alphabetSort(c[i]) == 0) //checks if char being pointed to is an alphabet or not - function returns 0 if not an alphabet 
    {
      removeChar(c, i); //function to remove char at index i 
    }
  }
}

//Function to make all letters of word lower case so all words are treated equally 
//Parameters: Pointer to char of word 
void makeLower(char * c)
{
  int32_t i; 
  for(i=0; c[i]; i++)
  {
    c[i] = tolower(c[i]); 
  }
}

/*---------------------------------Handlers for Signals------------------*/ 

//Handler for SIGINT signal 
static void killHandler(int signo)
{
  printf("\nProgress: SIGINT Handler\n"); 
  if (signo == SIGINT)
  {
    killsig = 1; //set global thread exit signal 
	 int32_t returnvalue = 1; 
    returnvalue = pthread_kill(readthread, SIGUSR1); //USR1 =30 //sends signal to read thread
    if (returnvalue)
    {
      printf("Error: Failed to send signal to Read Thread\n"); 
    }
    returnvalue = pthread_kill(printthread, SIGUSR2); //USR1 =31 //sends signal to write thread
    if (returnvalue)
    {
      printf("Error: Failed to send signal to Write Thread\n"); 
    }
    int32_t ret; 
    ret = pthread_join(printthread, NULL); //waits for return from second thread
    if (ret == 0)//successfully exited thread 2 
    {
      fclose(fnew); //closes file 
      pthread_mutex_destroy(&lock); //destroys lock
      printf("Progress: Exiting Process\n"); 
      exit(0); //exit 
    }
    else
    {
      printf("Error: Failed to exit successfully\n"); 
    }
  }
}


//Handler for SIGUSR1 signal == Read Thread Signal 
/*This handler reads through the contents of the file and logs the number of characters, number of words and number of lines. it also maps out the top 5 recurring words in the file*/
static void readHandler(int signo)
{
  char ch;
  rewind(fnew); 
  int32_t returnvalue = 1; 
  printf("Progress: In Read thread Handler\n"); 
   if (fnew == NULL)
  {
    printf("Error: File not found in child\n"); 
  }
  else
  {//valid file 
    printf("Progress: File Found\n");
    
    returnvalue = pthread_mutex_lock(&lock); //locks mutex to gain access to global resources
    if (returnvalue)
    {
       printf("Error: Failed to lock the mutex\n"); 
    }
    else 
    {//valid mutex
       while((ch=getc(fnew)) != EOF) //extracts character from file 
   	 {
   	   current.no_of_char++; //increments no of characters 
  
   	   //increment word if new line or space character       
   	   if(ch == ' ' || ch == '\n')
   	   {
   	     current.no_of_words++; //checks for space or eol to detect a word
   	   }
   	   if(ch == '\n') //checks for eol to increment no of lines
   	   {
   	     current.no_of_lines++; 
   	   }
   	 }
   	 rewind(fnew); 

//Starts mapping function for words in the file 
       no_of_words_to_process = 0;
   	 while(fscanf(fnew, "%1023s", word_store) == 1)//while there are still words in the file 
   	 {
   	   if(alphabetSort(word_store[0])) //checks if all chars in the word are alphabets 
   	   {
   	     nonalphabetFilter(word_store); //removes the non alphabetic character 
   	     makeLower(word_store); //converts all alphabets to lower case
   	     wordmapFunction(wordlist, &no_of_words_to_process, word_store); //stores words in map and assigns occurence count 
   	   }
   	 }

/*Function to sort the mapped words into descending order 
Parameters: 1: Void pointer to first element of the array that needs to be sorted 
2: size_t number of elements in the array that needs to be processed 
3: size_t size of elements in the array 
4: int (*comparator) (const void*, const void*): any type of sorting mechanism function(ascending/descending), return type for this function should be 
<0 for parameter a before parameter b
0 for if both parameters are equal 
>0 for parameter before parameter a */
   	 qsort((void *)wordlist, no_of_words_to_process, sizeof(map_t), wordSort );

//decides how many highest occuring words to accept  
   	 if(no_of_words_to_process < TOP_SORTED)
   	 {
   	   no_of_highest_repeating_words = no_of_words_to_process; 
   	 }
   	 else 
   	 {
   	   no_of_highest_repeating_words = TOP_SORTED; 
   	 }
		
       returnvalue = pthread_mutex_unlock(&lock);
       if (returnvalue)
       {
         printf("Error: Failed to unlock mutex\n"); 
       }
     }//valid mutex   
	}//valid file 
}


//Handler for SIGUSR2 signal
/* This handler prints the file stats calculated in the read thread and the 5 mapped highest recurring words from the file */  
static void printHandler(int signo)
{
  printf("Progress: In Print Thread Handler\n");
  int32_t returnvalue = 1; 
  returnvalue = pthread_mutex_lock(&lock); //locks the mutex
  if (returnvalue)
  {
    printf("Error: Failed to lock mutex\n"); 
  }
  else 
  {
    //prints out file stats
    printf("\nProgress: Printing File Stats\n"); 
    printf("Number of chars: %d\n", current.no_of_char);
    printf("Number of words: %d\n", current.no_of_words);
    printf("Number of lines: %d\n", current.no_of_lines);
    current.no_of_words = 0; //reinitializes values to 0
    current.no_of_lines = 0;   
    current.no_of_char = 0; 
    returnvalue = pthread_mutex_unlock(&lock);//unlocks the mutex
    //prints out the top 5 highest recurring words 
    printf("\nProgress: Printing Map of User Input\n"); 
    for (iterate = 0; iterate<no_of_highest_repeating_words; iterate++)
    {
     printf("Word: %s\tCount: %d\n", wordlist[iterate].word, wordlist[iterate].occurenceCount);
    }
    for (iterate = 0; iterate<no_of_highest_repeating_words; iterate++)
    {
     //wordlist[iterate].word = '\0'; 
     wordlist[iterate].occurenceCount = 0;
    }
    
    if(returnvalue)
    {
      printf("Error: Failed to unlock Mutex\n"); 
    }
  } 
}



/*--------------------Functions for Threads------------------------*/ 
//Function for Read Thread
//accepts sigusr1 as the unblocking signal and calls its handler 
void *readthreadFunction(void* arg)
{
  int32_t status; 
  while(1)
  {
    int32_t sig = 0; 
    sigset_t sigmask; 
    if (sigaddset(&sigmask, SIGUSR1)==-1) //unblocks acceptance of SIGUSR1 signal 
    {
      //opens all signals
      printf("Error: SIGUSR1 not valid Signal\n");
      pthread_exit(NULL); 
    }
    pthread_sigmask(SIG_UNBLOCK, &sigmask, (sigset_t *)0);
    //unblocks all signals
    sigwait(&sigmask, &sig); //wait here till USR1 is received
    if (sig == SIGUSR1)
    {
     if (killsig == 1) //check for exit value 
     {
       printf("Progress: Exiting Read Thread\n"); 
       break;
     }
     printf("Progress: Read Thread Initiated\nThread ID: %lu\n", pthread_self()); 
     readHandler(sig); //call handler 
    }
    printf("Progress: Returned from Read Thread Handler\n");
    status = pthread_kill(printthread, SIGUSR2); //USR2 = 31 //calls print thread by sending signal SIGUSR2
    if(status < 0)
    {
      printf("Error: SIGUSR2 failed to send\n"); 
    }
  }//end of while 
  pthread_exit(NULL); //exiting thread on ctrl+C 
}

//Function for print thread
//waits for SIGUSR2 to unblock and calls its handler to print file stats 
void *printthreadFunction(void* arg)
{
  while(1)
  {
    int32_t sig; 
    sigset_t sigmask; 
    sigemptyset(&sigmask); //clears all signals 
    if(sigaddset(&sigmask, SIGUSR2) == -1) //adss sigusr2 to the list of required signals 
    {
      printf("Error: SIGUSR2 signal Invalid\n");
      pthread_exit(NULL); 
    }
    pthread_sigmask(SIG_UNBLOCK, &sigmask, (sigset_t *)0); //Unblocks all signals 
    sigwait(&sigmask, &sig);//waits for sigusr2  
    if (sig == SIGUSR2)
    {
      if (killsig == 1)//checks for process exit function 
      {
        printf("Progress: Exiting Print Thread\n"); 
        break;
      }
      printf("Progress: Print Thread Initiated\nThread ID: %lu\n", pthread_self());
      printHandler(sig); //calls print thread signal
    }
    printf("Progress: Returned from Print Thread Handler\n"); 
  }
  pthread_exit(NULL);//exiting in case of ctrl+c 
}

/*---------------------------------MAIN-------------------------------*/
int32_t main(int argc, char* argv[])
{
  pid_t id = getpid(); 
  printf("Main Process ID: %d\n", id); 
  volatile int32_t returnvalue = 0; 
  fnew = fopen(argv[1], "w+"); //opens file to write
  if (fnew == NULL)
  {
    printf("Error: Couldnt Open File\n"); 
    return 0; 
  }
  else
  {//file initiated loop
    printf("Progress: File Initiated\n");
    
    //initializing mutex lock 
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
      printf("Error: Mutex Init Failed\n"); 
    }
    
    pthread_attr_t attr_obj; //thread attribute variable
    
    sigset_t signalmask; //sigset_t is a struct type to store signals defined in unix 
    
    struct sigaction whataction; //sigaction is a struct with members like handler, flags, mask etc
    
    returnvalue = sigemptyset(&signalmask); //initialises set to empty excluding all masks prototype "int sigemptyset(sigset_t *set)"
    if(returnvalue)
    {
      printf("Error: Failed to empty relevant signal set\n"); 
    }
    returnvalue = pthread_sigmask(SIG_BLOCK, &signalmask, (sigset_t*)0);
    if(returnvalue)
    {
      printf("Error: Failed to clock all signals\n"); 
    }//used to fetch or change the set of signals whose delivery is currently blocked for the caller. the parameters are 1. HOW: essenrially tells you how to change the sigmask- you can block/unblock/setmask - block combines the signals in the second parameter with old blocked signals and blocks them all, unblock removes the second parameters from the set of blocked signals and setmask assigns a group of second parameter to blocked signals (as old ones). 2/3 - sigset_t* setnew, sigset_t* setold - setold if non-null holds the old value of sigmask 

    /*set up signal handlers for all SIGUSR1 and SIGUSR2*/ 
    whataction.sa_flags = 0; //member of struct sigaction 
    whataction.sa_handler = readHandler; //assigns readhandler to the whataction member handler 
    returnvalue = sigaction(SIGUSR1, &whataction, (struct sigaction *)0); 
    if(returnvalue)
    {
      printf("Error: Failed to assign readhandler to SIGUSR1\n"); 
    }
    //int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact) - this function assigns a handler to a signum. second parameter contains the action/signal habdler initialisation. third paramter if non null contains the old sig handler initialization. signum can be anything except sigkill and sigstop  
   
     
    whataction.sa_handler = printHandler; //assigns print handler to the whataction member handler 
    returnvalue = sigaction(SIGUSR2, &whataction, (struct sigaction *)0); 
//initiates printhandler function for USR2 signal 
     if(returnvalue)
    {
      printf("Error: Failed to assign printhandler to SIGUSR2\n"); 
    }
/*Done setting signals and handlres*/ 

    returnvalue = pthread_attr_init(&attr_obj); //initialise attr object
    if(returnvalue)
    {
      printf("Error: Failed to initialize attribute to detachable\n"); 
    }
    pthread_attr_setdetachstate(&attr_obj, PTHREAD_CREATE_DETACHED);
    if(returnvalue)
    {
      printf("Error: Failed to set detachable state to detached\n"); 
    }
 //initialises the attribute object to changes it detachment state to detached. The thread neednt wait for pthread_join to release its resources when its done. detached is also used when we dont need to know the return status of the thread i.e you dont need to know if it has finished 
    char* filename = argv[1]; 
    
    returnvalue = pthread_create(&readthread,&attr_obj, readthreadFunction, &filename); //creates thread to read values from file 
    
    if (returnvalue == 0) 
    {
      printf("Read Thread TID: %u\n", (unsigned int)readthread); 
    }

    returnvalue = pthread_attr_init(&attr_obj); //initialise attr object
    if(returnvalue)
    {
      printf("Error: Failed to initialize attribute to joinable\n"); 
    }
    pthread_attr_setdetachstate(&attr_obj, PTHREAD_CREATE_JOINABLE); 
    if(returnvalue)
    {
      printf("Error: Failed to set detachable state to joinable\n"); 
    }
//initialises the attribute object to changes it detachment state to joinable. The thread must wait for pthread_join to release its resources when its done. detached is also used when we  need to know the return status of the thread i.e you need to know if it has finished
    
    returnvalue = pthread_create(&printthread,&attr_obj, printthreadFunction, &filename); //creates thread to print al file stats 
    if (returnvalue == 0) 
    {
      printf("Write Thread TID: %u\n", (unsigned int)printthread); 
    }
    if (signal(SIGINT, killHandler) == SIG_ERR) //checks for SIGINT 
    {
      printf("Error: Cant catch kill signal\n"); 
    }
    
    usleep(1000*1000); //godforsaken reasons 

    
    printf("Start inputting file content:\n"); 
    while(1)
    {
     while(killsig != 1)
     {
      char stringrep[1000];
      size_t length = 0;
      char* return_value = fgets(stringrep, 1000, stdin);//sets fgets to accept inputs from command line to write to file 
      char* stringp = stringrep; 
      length = strlen(stringrep);
     
      if(return_value != NULL)
      {
        pthread_mutex_lock(&lock);
        if (returnvalue)
        {
          printf("Error: Failed to lock mutex\n"); 
        } 
        fwrite(stringp, 1,length, fnew); //writes input to file 
        pthread_mutex_unlock(&lock);
        if (returnvalue)
        {
          printf("Error: Failed to unlock mutex\n"); 
        }
      }
      else 
      {
        printf("Sending USR1 to Read Thread\n");
        int32_t r = 10; 
        r = pthread_kill(readthread, SIGUSR1); //USR1 =30 //sends USR1 signal if user has finished giving input by hitting CTRL+D
      }
       
    }
  }
   
  }//file initiated loop
   return 0; 
}
