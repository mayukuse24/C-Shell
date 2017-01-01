#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<pwd.h>
#include<string.h>
#include<stdlib.h>
#include<signal.h>
#include<fcntl.h>

#define sc(n) scanf("%d",&n)

typedef struct dabba
{
  pid_t pid;
  int proc_state;
  int num;
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

void bg_pop(int val)
{
  node* root=bg_head;
  
  if(bg_head == NULL)
    {
      top=1;
      printf("Process list is empty\n");
    }
  else if(root->num == val)
    {
      bg_head = bg_head->next;
    }
  else
    {
      while(root->next && root->next->num != val)
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
      printf("[%d---%d] ",temp->num,temp->pid);
      temp=temp->next;
    }
}

int main()
{
  node *pstruc;
  int choice,del;
  
  while(1)
    {
      
      sc(choice);
      
      switch(choice)
	{	
	case 1 :
	  pstruc = (node*)malloc(sizeof(node));
	  sc(pstruc->pid);
	  sc(pstruc->proc_state);
	  bg_push(pstruc);
	  break;
	  
	case 2 :
	  sc(del);
	  bg_pop(del);
	  break;
	}
      print_link();
    }
  
  return 0;
}
