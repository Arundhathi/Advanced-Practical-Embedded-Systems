#include<stdio.h> 

int main()
{
FILE* LEDH = NULL; 
char* LEDP = "/sys/class/leds/beaglebone:green:usr3/brightness";
if(LEDH = fopen(LEDP, "r+"))
{
fwrite("1", 1, 1, LEDH);
fclose(LEDH); 
}
else
{
printf("galat file\n"); 
}

return 1; 
}
