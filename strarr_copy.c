#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<pwd.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

char** copy_string_arr(char** argv)
{
  int a;
  char **newarr=malloc(100*sizeof(char*));
  for(a=0;argv[a][0] != '>' ;a++)
    {
      newarr[a]=argv[a];
    }
  newarr[a]= NULL;
  
  return newarr;
}

int main()
{

  char** argv = malloc(100*sizeof(char*));
  int a=0;
  argv[0] = "ls";
  argv[1] = "-la";
  argv[2] = ">";
  argv[3] = "shouldnotbeinnewarr";
  argv[4] = NULL;
  argv[5] = NULL;
 
  argv = copy_string_arr(argv); 

  for(a=0;argv[a];a++)
    {
      printf("%s ",argv[a]);
    }
  
  return 0;
}
