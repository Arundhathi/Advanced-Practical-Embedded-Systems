#/*
#CODE: Python Script called by systeminformation.sh to write System Information to a text file named System_Configuration_Information.txt
#LANGUAGE: Python 
#AUTHOR: Arundhathi Swami
#COURSE/PROJECT: Advanced Practical Embedded Systems
#DATE STARTED: Sept 3 2017
#LAST DATE MODIFIED: Sept 5 2017
#*/

#operating system type 
#os distribution 
#kernel version 
#information on kernel build
#kernel gcc version build 
#kernel build time 
#system arch information 

import platform 
import sys 
import os 
import sys 
from subprocess import check_output 

#------------------------------------------------------------------------------------#
os_info = open('/proc/version') #executes cat /proc/version command 
file_info = os_info.read() #reads command generated info into file_info  
contents = file_info.split(' ') #thereby converting it into an array of strings


p_architecture = platform.architecture() #uses platform library to get processor architecture 
architecture = p_architecture[0]+' '+p_architecture[1]
#print architecture

p_endianness = sys.byteorder+ ' Endian'#uses sys library to determine processors endianness
#print p_endianness 


operating_system_type = contents[0] #extracts os information from /proc/version command 
#print operating_system_type

os_distribution = platform.dist() #uses platform library to determine distribution of os in use 
distribution = os_distribution[0] +' '+ contents[9]
#print distribution

kernel_build_version = contents[2] #extracts kernel version from /proc/version 
#print kernel_build_version

kernel_build_info = platform.uname() #extracts kernel build date and time 
timeinfo = kernel_build_info[3]
#print timeinfo 

gcc_version = contents[4]+ ' ' +contents[5]+ ' ' +contents[6] #extracts gcc version from /proc/version
#print gcc_version

system_info_file = open('System_Configuration_Information.txt', 'w')
system_info_file.write('\n Processor Architecture Details: ' + architecture)
system_info_file.write('\n Processor Endianness: '+p_endianness)
system_info_file.write('\n Operating System In Use: '+operating_system_type)
system_info_file.write('\n Operating System Distribution In Use: '+distribution)
system_info_file.write('\n Kernel Version: '+kernel_build_version)
system_info_file.write('\n Kernel Build Time: '+timeinfo)
system_info_file.write('\n GCC Tool Chain Version : '+gcc_version) 
system_info_file.close()


