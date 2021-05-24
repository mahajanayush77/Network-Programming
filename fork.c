#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>

int main(){

    int pid_1 = fork(); // creates 1 child process
    int pid_2 = fork(); // creates 2 child processes
    // total child processes are 3

    if(pid_1 == 0 && pid_2!=0){
      printf("Child Process created through first fork command; PID1: %d, PID2: %d \n",pid_1,pid_2);
    }
    else if(pid_2 == 0)
      printf("Child Process created through second fork command PID1: %d, PID2: %d \n",pid_1,pid_2);
    
    else
      printf("Parent Process PID1: %d, PID2: %d \n",pid_1,pid_2);
    return 0;
}