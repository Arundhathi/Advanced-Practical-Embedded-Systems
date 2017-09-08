#/*
#CODE: Build Kernel Script - from any directory other than linux directory 
#LANGUAGE: Bash Scritping
#AUTHOR: Arundhathi Swami
#COURSE/PROJECT: Advanced Practical Embedded Systems
#DATE STARTED: Sept 7 2017
#LAST DATE MODIFIED: Sept 8 2017
#*/





#!/bin/bash 
#shebang line implies bash executable can be found in bin folder


echo Kernel Build Process Initiated 
cd #moves to home directory
cd Downloads/linux-4.12.10  #moves to right directory 
sudo apt-get install git #installs git libraries 
sudo apt-get install libssl-dev #installs libsl-dev package for libraries 
make oldconfig #creates .config file 
sudo make -j4 #builds kernel files while running two threads per processor 
sudo make modules_install -j4 #builds all modules 
sudo make install -j4 
sudo update-grub #updates grub to include newly built kernel 
cd

