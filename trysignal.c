#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <sys/types.h>

void handler(int sig)
{
  pid_t pid;

  pid = wait(NULL);

  printf("Pid %d exited.\n", pid);
}

int main()
{
  pid_t pid;
  int status;
  char **argv;
  
  signal(SIGCHLD, handler);
  
  pid=fork();
  if(pid<0)
    printf("error\n");
  else if(pid == 0) //child  process
    {
      setpgid(0,0);
      argv[0] = "emacs";
      argv[1] = "\0";
      printf("child process\n");
      if(execvp(argv[0],argv) == -1)
	perror("Error : command not executed");
      
      exit(0);
    }
  else
    {
      
      tcsetpgrp(0,getpgrp());
      
      printf("parent process\n");
      while(1)
	{
	  sleep(10);
	}
    }
      
  return 0;
  
}
