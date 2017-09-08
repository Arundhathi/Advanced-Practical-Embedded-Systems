#/*
#CODE: Prints out system Information to the screen and to a file named System_Configuration_Information.txt
#LANGUAGE: Bash Script
#AUTHOR: Arundhathi Swami
#COURSE/PROJECT: Advanced Practical Embedded Systems
#DATE STARTED: Sept 7 2017
#LAST DATE MODIFIED: Sept 8 2017
#*/

#!/bin/bash //shebang line implies bash executable can be found in bin folder 
#existential crisis 
#operating system type 
#os distribution 
#kernel version 
#information on kernel build
#kernel gcc version build 
#kernel build time 
#system arch information  

/usr/bin/python systeminformation.py #executes python file to create text file with all required system information

echo "Name of User is:" #prints out crrent sessions user name 
whoami
echo

echo "Processor Architecture:"
lscpu | grep Architecture #pulls out architecture (x86/ARM etc) from lscpu output 
echo

echo "Processor Architecture Endianess:"
lscpu | grep Byte #pulls out endianness information from lscpu output 
echo

echo "Dominant Operating System:"
uname #obtains current running operating system 
echo

echo "Operating System Distribution:"
lsb_release -a | grep Description #extracts and pipes OS Distribution from lsb_release command output contained in Description header 
echo 

echo "Kernel Version:"
uname -r #outputs kernel version 
echo

echo "Kernel Build Data:"
uname -v #kernel build data like time and date of installation 
echo

echo "GCC Version:"
gcc --version | grep gcc #displays currently installed gcc toolchain version 

echo "Other Available Kernels:"
dpkg --list | grep linux-image #displays all available kernels in use 
echo


