#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<pwd.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

typedef struct dabba
{
  pid_t pid;
  int proc_state;
  int num;
  char name[100];
  struct dabba *next;
}node;

node* bg_head;
int top=1;

void bg_push(node *temp)
{
  node *root = bg_head;
  if(bg_head == NULL)
    {
      temp->num = top++;
      bg_head = temp;
    }
  else
    {
      int count=2; //place of root->next
      while(root->next)
	{
	  root=root->next;
	}
      temp->num = top++;
      root->next=temp;
    }
}

void bg_pop(pid_t pid)
{
  node* root=bg_head;
  
  if(bg_head == NULL)
    {
      top=1;
      printf("Process list is empty\n");
    }
  else if(root->pid == pid)
    {
      bg_head = bg_head->next;
    }
  else
    {
      while(root->next && root->next->pid != pid)
	{
	  root=root->next;
	}
      if(root->next == NULL)
	printf("Process with number doesnt exist\n");
      else
	{
	  node* p2=root->next;
	  root->next = p2->next;
	}
    }

  if(bg_head == NULL)
    top=1;
}

void print_link()
{
  node *temp=bg_head;
  while(temp)
    {
      printf("[%d] %s [%d]\n",temp->num,temp->name,temp->pid);
      temp=temp->next;
    }
}

void call_cd(char* arg)
{
  if(chdir(arg) == -1)
    perror("Error ");
}

void call_pwd()
{
  char* cwd;

  if(getcwd(cwd,100*sizeof(cwd)) == NULL)
    perror("Error ");
  else
    printf("%s\n",cwd);
}

void call_echo(char** argv)
{
  int a,b;
  int eEflag=-1;
  int nflag = 0;
  char output[1000] = {{'\0'}};

  for(b=1;argv[b] && argv[b][0]=='-' ;b++)
    {
      if(argv[b] && argv[b][0]=='-')
  	{
  	  for(a=0;argv[b][a] && argv[b][a] != ' ';a++)
  	    {
  	      if(argv[b][a] == 'n')
  		{
  		  nflag = 1;
  		}

  	      /* if(argv[b][a] == 'e') */
  	      /* 	{ */
  	      /* 	  eEflag = 1; */
  	      /* 	} */
  	      /* else if(argv[b][a] == 'E') */
  	      /* 	{ */
  	      /* 	  eEflag = -1; */
  	      /* 	} */
  	    }
  	}
    }

  for(a=b;argv[a];a++)
    {
      strcat(output,argv[a]);
      strcat(output," ");
    }
  output[strlen(output)-1] = '\0';

   /* if(eEflag == 1) */
  /*   { */
  /*   } */

  if(nflag == 1)
    {
      printf("%s",output);
      fflush(stdout);
   }
  else
    printf("%s\n",output);

}

void check_home(char pwd[],int len)
{
  char temp[1000];
  temp[0] = '~';
  int a=len;
  int b=1;
  while(pwd[a]) 
    { 
      temp[b] = pwd[a];
      b++; 
      a++; 
    }
  temp[b] = '\0';
  strcpy(pwd,temp);
}

char* input_command(char hpath[],int len)
{
  size_t buffer_size=1000;
  int val;
  char host[100];
  char pwd[1005];
  char *command=NULL;

  struct passwd *user = getpwuid(getuid());
  gethostname(host,sizeof(host));
  if(getcwd(pwd,sizeof(pwd))==NULL)
    perror("error : path not found");

  //printf("hpath is %s\n",hpath);
  //printf("pwd is %s\n",pwd);
  
  if(strncmp(pwd,hpath,len) == 0)
    {
      check_home(pwd,len);
    }

  printf("<%s@%s:%s-->",user->pw_name,host,pwd);
  fflush(stdout);

  val=getdelim(&command,&buffer_size,'\n',stdin);
  command[val-1] = '\0';

  return command;
}

char** split_command(char *command,char* delim)
{
  int a=0;
  char *part = strtok(command,delim);
  char **argv = malloc(100 * sizeof(char*));

  while(part != NULL)
    {
      argv[a] = part;
      a++;
      part = strtok(NULL,delim);
    }
  argv[a] = NULL;

  return argv;
}

int check_command(char** argv,int* rflag,int* pflag)
{
  int a;
  for(a=0;argv[a];a++)
    {
      if(strcmp(argv[a],">")==0 || strcmp(argv[a],"<")==0  || strcmp(argv[a],">>")==0 )
	*rflag = 1;

      if(strcmp(argv[a],"|")==0)
	*pflag = 1;
    }
  return a;
}

int val_to_pid(int val)
{
  node* root = bg_head;
  while(root != NULL && root->num != val)
    {
      root=root->next;
    }
  if(root)
    return root->pid;
  else
    return -1;
}

void handler(int sig)
{

  int stat;
  pid_t pid = waitpid(-1,&stat,WNOHANG);
  
  if(pid>0)
    {
      bg_pop(pid);
      printf("Pid %d exited.\n", pid);
    }
}

int launch_command(char** argv,int bground)
{
  pid_t pid;
  
  pid = fork();
  //printf("%d  %d\n",getpgrp(),pid);
  
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
	{
	  tcsetpgrp(0,getpgrp());
	  node *pstruc = (node*)malloc(sizeof(node));
	  strcpy(pstruc->name,argv[0]);
	  pstruc->pid = pid;
	  pstruc->proc_state = 0;
	  bg_push(pstruc);
	}
      else
	{
	  waitpid(pid,NULL,0);
	}
      //printf("waited\n");
    }
      //printf("%d  %d\n",getpgid(pid),pid);
    
  return 1;
}

void check_redirection(char** argv,int* p,int* q)
{
  int a;
  for(a=0;argv[a];a++)
    {
      if(argv[a][0] == '>')
	*q = a+1;
      if(argv[a][0] == '<')
	*p = a+1;
    }
}

char** copy_string_arr(char** argv)
{
  int a;
  char **newarr=malloc(100*sizeof(char*));
  for(a=0;argv[a][0] != '>' && argv[a][0] != '<';a++)
    {
      newarr[a]=argv[a];
    }
  newarr[a]= NULL;
  
  return newarr;
}

int redirect(char** argv,int flag)
{
  pid_t pid;
  
  pid = fork();
  
  if(pid==0)
    {
      int out=0,in=0;
      int fout,fin;
      char** command=malloc(100*sizeof(char*));
      
      check_redirection(argv,&in,&out);
      command = copy_string_arr(argv);
      
      if(out>0)
	{
	  fout = open(argv[out], O_WRONLY | O_CREAT, S_IRWXU);
	  dup2(fout,1);
	}
      
      if(in>0)
	{
	  fin = open(argv[in], O_RDONLY, S_IRWXU);
	  dup2(fin,0);
	}
      
      //close(fin);
      //close(fout);
      
      if(execvp(command[0],command) == -1)
	perror("Error : command not executed");
      
      exit(0);
    }
  else if(pid<0)
    {
      perror("Error : Fork not created");
    }
  else
    {
      wait(NULL);
    }

  return 1;
}

int find_redir(char** argv,int* pos)
{
  int a;
  for(a=0;argv[a];a++)
    {
      if(strcmp(argv[a],">")==0)
	{
	  *pos=a+1;
	  return 1;
	}
      
      if(strcmp(argv[a],"<")==0)
	{
	  *pos = a+1;
	  return 2;
	}
    }

  *pos = -1;
  return 0;
}

void pipe_out(char **argv)
{
  int pf[2],fpos,finpointer,foutpointer;
  int prev_flag=0,temp;
  pid_t pid;
  char **arg_split;
  int a=0;
  printf("pipe function\n");
  while(argv[a])
    {
      arg_split = split_command(argv[a]," ");

      
      if(find_redir(arg_split,&fpos)==2)
	{
	  finpointer = open(arg_split[fpos], O_RDONLY, S_IRWXU);
	  prev_flag = -1;
	  strcpy(arg_split[fpos-1],arg_split[fpos]);
	  arg_split[fpos]=NULL;
	}

      if(find_redir(arg_split,&fpos)==1)
	{
	  foutpointer = open(arg_split[fpos], O_WRONLY | O_CREAT, S_IRWXU);
	  temp = -2;
	  arg_split[fpos-1]=NULL;
	  arg_split[fpos]=NULL;

	}

      printf("Checking loop run\n");
      if(pipe(pf)<0)
	perror("Pipe command error ");
  
      pid=fork();
      if(pid<0)
	perror("Fork error ");
      else if(pid==0) //child
	{
	  if(prev_flag!=-1)
	    dup2(prev_flag,0);
	  else
	    {
	      dup2(finpointer,0);
	      close(finpointer);
	      prev_flag=0;
	    }

	  //printf("Before entering\n");
	  if(argv[a+1] != NULL)
	    {
	      //printf("After entering\n");	  
	      dup2(pf[1],1);
	    }
	   else if(temp==-2)
	    {
	      dup2(foutpointer,1);
	      close(foutpointer);
	    }
	  
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

void quit()
{
  _exit(0);
}

int main()
{
  char *com = NULL;
  char *pipe_com = malloc(sizeof(char*));
  char **argv,**arg_split;
  char **multicom;
  int a,b,bg=0;
  char HOME[1000];
  int homelen,lastele;
  int redir_flag,pipe_flag,re_pi_flag;
  
  signal(SIGCHLD, handler);
  
  if(getcwd(HOME,sizeof(HOME))==NULL)
    perror("error : path not found");
  homelen=strlen(HOME);

  while(1)
    {
      com = input_command(HOME,homelen);
      multicom = split_command(com,";\t");
      
      for(a=0;multicom[a];a++)
	{
	  
	  strcpy(pipe_com,multicom[a]);
	  //printf("%s\n",pipe_com);
	  com = multicom[a];
	  argv = split_command(com," \t");
	  
	  if(argv[0])
	    {
	      bg=redir_flag=pipe_flag=0;
	      re_pi_flag= -1;
	      lastele = check_command(argv,&redir_flag,&pipe_flag)-1;
	      if(strcmp(argv[lastele],"&") == 0)
		{
		  bg=1;
		  argv[lastele] = NULL;
		}

	      if(strcmp(argv[0],"cd") == 0)
		{
		  call_cd(argv[1]);
		}
	      else if(strcmp(argv[0],"pwd") == 0)
		{
		  call_pwd();
		}
	      else if(strcmp(argv[0],"echo") == 0)
		{
		  call_echo(argv);
		}
	      else if(strcmp(argv[0],"quit") == 0)
		{
		  quit();
		}
	      else if(strcmp(argv[0],"jobs") == 0)
		{
		  print_link();
		}
	      else if(bg==1)
		{
		  if (launch_command(argv,1) != 1)
		    perror("Bg Error ");
		}
	      else if(pipe_flag == 1)
		{
		  argv = split_command(pipe_com,"|");
		  pipe_out(argv);
		  		  
		}
	      else if(redir_flag == 1)
		{
		  if(redirect(argv,redir_flag) != 1)
		    perror("Redir Error ");
		}
	      else
		{
		  if (launch_command(argv,0) != 1)
		    perror("Error ");
		}
	    }   
	}
    }

  return 0;
}
