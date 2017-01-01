#include<stdio.h>

#include<setjmp.h>

static jmp_buf env;

void func1()
{
  if(setjmp(env))
    printf("env is %d\n",env);
}

int main()
{
  int a,b,c;
  
  func1();

  printf("main program\n");

  longjmp(env,2);
  return 0;
}
  
