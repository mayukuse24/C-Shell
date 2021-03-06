#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<pwd.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

int launch_command(char** argv,int bground)
{
  pid_t pid;
  
  pid = fork();
  printf("%d  %d\n",getpgrp(),pid);
  
  if(pid==0)
    {
      if(bground == 1)
	setpgid(0,0);

      if(execvp(argv[0],argv) == -1)
	perror("Error : command not executed");
      exit(0);
    }
  else if(pid<0)
    {
      perror("Error : Fork not created");
    }
  else
    {
      if(bground == 1)
	tcsetpgrp(0,getpgrp());
      else
	wait();
      //printf("waited\n");
    }
      //printf("%d  %d\n",getpgid(pid),pid);
    
  return 1;
}
