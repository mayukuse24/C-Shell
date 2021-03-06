void pipe_out(char **argv)
{
  int pf[2];
  int prev_flag=0;
  pid_t pid;
  char line[1000];
  char **arg_split;
  int a=0;
  
  while(argv[a])
    {
      arg_split = split_command(argv[a]," \t\r");
      /* printf("%s\n",arg_split[0]); */
      /* printf("%s\n",arg_split[1]); */
      /* printf("%d\n",prev_flag); */
      
      if(pipe(pf)<0)
	perror("Pipe command error ");
  
      pid=fork();
      if(pid<0)
	perror("Fork error ");
      else if(pid==0) //child
	{
	  dup2(prev_flag,0);

	  if(argv[a+1] != NULL)
	     dup2(pf[1],1);

	  close(pf[0]);
	  if(execvp(arg_split[0],arg_split)==-1)
	    perror("pipe exec error ");
	  _exit(0);
	}
      else //parent
	{
	  wait(NULL);
	  close(pf[1]);
	  prev_flag = pf[0];
	}
      a++;
    }
}
