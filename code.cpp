#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <grp.h>
#include "tokenizer.h"
#include<fcntl.h> 

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

int cmd_exit(struct tokens *tokens);
int cmd_help(struct tokens *tokens);
int cmd_saurbh(struct tokens *tokens);
int cmd_id(struct tokens *tokens);
//int cmd_id_g(struct tokens *tokens);
// int cmd_id_u(struct tokens *tokens);
//int cmd_id_G(struct tokens *tokens);
/* Built-in command functions take token array and return int */
typedef int cmd_fun_t(struct tokens *tokens);

/* Built-in command struct and lookup table */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_exit, "exit", "exit the command shell"},
  
  {cmd_id, "id", "clone of id command in linux\nid -g, print group id\nid -u, print user id\nid -G, print group list"},
  
};

/* Prints a helpful description for the given command */
int cmd_help(unused struct tokens *tokens) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
  return 1;
}
int cmd_saurbh(unused struct tokens *tokens) {
 // for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++){
  //	if(strcmp(cmd_table[i].cmd,"saurbh")==0)
  		printf("%s - %s\n", cmd_table[2].cmd, cmd_table[2].doc);
  //}
    
  return 1;
}
int cmd_id(unused struct tokens *tokens) {
	size_t as = tokens_get_length(tokens);
	struct group *group;
	char *temp = tokens_get_token(tokens,as-1);
	
	if(as==1){
		
  //		printf("%s - %s\n", cmd_table[3].cmd, cmd_table[3].doc);
  		int u_id=getuid();
  		int g_id=getgid();
  		gid_t groupslist[100];
  		int num_grps=getgroups(100,groupslist);
  		printf("gid is = %d\n",g_id );
  		printf("uid is = %d\n",u_id );
  		printf("groups=" );
  		for (int i = 0; i <num_grps ; ++i)
  		{
  			group = getgrgid(groupslist[i]);
  			printf(" %d (%s)",groupslist[i],group->gr_name );
  		}
  		printf("\n");		
	}
	else if(strcmp(temp,"-g")==0){
		int g_id=getgid();
		printf("gid is =%d\n",g_id );
	}
	else if(strcmp(temp,"-u")==0){
		int u_id=getuid();
		printf("uid is =%d\n",u_id );
	}
	else if(strcmp(temp,"-G")==0){
		//struct group *group;
		gid_t groupslist[100];
  		int num_grps=getgroups(100,groupslist);
  		for (int i = 0; i <num_grps ; ++i)
  		{
  			group = getgrgid(groupslist[i]);
  			printf("%d ",groupslist[i] );
  		}
  		printf("\n");	
	}
	else if(as>2){
		printf("id: cannot print :only: of more than one choice\n");
	}
	else{
		cmd_help(tokens);
	}
  return 1;
}

/* Exits this shell */
int cmd_exit(unused struct tokens *tokens) {
  exit(0);
}

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[]) {
  for (unsigned int i = 0; i < sizeof(cmd_table) / sizeof(fun_desc_t); i++)
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
      return i;
  return -1;
}

/* Intialization procedures for this shell */
void init_shell() {
  /* Our shell is connected to standard input. */
  shell_terminal = STDIN_FILENO;

  /* Check if we are running interactively */
  shell_is_interactive = isatty(shell_terminal);

  if (shell_is_interactive) {
    /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
     * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
     * foreground, we'll receive a SIGCONT. */
    while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
      kill(-shell_pgid, SIGTTIN);

    /* Saves the shell's process id */
    shell_pgid = getpid();

    /* Take control of the terminal */
    tcsetpgrp(shell_terminal, shell_pgid);

    /* Save the current termios to a variable, so it can be restored later. */
    tcgetattr(shell_terminal, &shell_tmodes);
  }
}

int main(unused int argc, unused char *argv[]) {
  init_shell();

  static char line[4096];
  int line_num = 0;

  /* Please only print shell prompts when standard input is not a tty */
  if (shell_is_interactive)
    fprintf(stdout, "%d: ", line_num);

  while (fgets(line, 4096, stdin)) {
    /* Split our line into words. */
    struct tokens *tokens = tokenize(line);

    /* Find which built-in function to run. */
    int fundex = lookup(tokens_get_token(tokens, 0));

    if (fundex >= 0) {
      cmd_table[fundex].fun(tokens);
    } else {

    int pid=fork();
	//int status;
	if(pid==0){
		char *ar[100];
    	ar[0]=tokens_get_token(tokens,0);
    	int i=1;
    	for(i=1;i<tokens_get_length(tokens);i++){
    		ar[i]=tokens_get_token(tokens,i);
    	}
    	//printf("%s\n",ar[1] );
    	ar[i]=NULL;
    	if(ar[1]!=NULL){
    		//printf("asa\n");
    		char *file_name=ar[2];
    		//execv(ar[0],ar);
    		int file_number=open(file_name,O_CREAT | O_WRONLY);
    		//printf("%d\n",file_number );
    		dup2(file_number,1);
    		execv(ar[0],ar);
    		//printf("asas\n");
    		close(file_number);

    	}
    	else{
    		execv(ar[0],ar);	
    	}
    	//printf("%s\n",ar[2] );
    	
	} else {
		wait(&pid);
		//printf("%u\n", getpid());

		//printf("This is parent, child has exited\n");
	}


      /* REPLACE this to run commands as programs. */
    	
     // fprintf(stdout, "This shell doesn't know how to run programs.\n");
    }

    if (shell_is_interactive)
      /* Please only print shell prompts when standard input is not a tty */
      fprintf(stdout, "%d: ", ++line_num);

    /* Clean up memory */
    tokens_destroy(tokens);
  }

  return 0;
}
