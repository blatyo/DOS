/**************************
 * @author Allen Madsen (blatyo)
 *
 * This program mimics a dos shell in unix
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>

//Define bool for ease of use
typedef int bool;
#define false 0
#define true 1

bool parse(char *command, char **args, char *input, char *output);
bool mapCommands(char **args);
void execute(char **args, const bool runInBG, char *input, char *output);

//commands
char ls[]    = "ls";
char pwd[]   = "pwd";
char cp[]    = "cp";
char mv[]    = "mv";
char rm[]    = "rm";
char emacs[] = "emacs";
char cat[]   = "cat";

/**
 * This program mimics a dos shell in unix
 */
int main () {

  bool quit = false;
  
  while(!quit){
    char command[80];
  
    printf("dos > ");
    fgets(command, sizeof(command), stdin);
  
    char *args[80];
    char input[80];
    char output[80];
    bool runInBG = parse(command, args, input, output);
  
    if(mapCommands(args)){
      if(strcmp(*args, "quit") != 0){
        execute(args, runInBG, input, output);
      } else {
        quit = true;
      }
    } else {
      printf("Invalid command: %s\n", *args);
    }
  }
  return 0;
}

/**
 * This method parses a command entered into the interpreter and
 * returns its various parts.
 *
 * @param command
 *    the command to parse
 * @param args
 *    a place to store the parsed command
 * @param input
 *    a place to store the argument for redirecting input if any
 * @param output
 *    a place to store the argument for redirecting output if any
 *
 * @return if the process should run in the background
 */
bool parse(char *command, char **args, char *input, char *output){
  bool runInBG = false;
  char *out;
  char *in;
  *out = '\0';
  *in = '\0';
  while(*command != '\0'){
    if(*command == ' ' || *command == '\t' || *command == '\n'){
      *command++ = '\0';
    }else if(*command == '&'){
      *command++ = '\0';
      runInBG = true;
      break;
    }else if(*command == '<'){
      *command++ = '\0';
      while(*command == ' ' || *command == '\t'){
        *command++ = '\0';
      }
      in = command;
      while(*command != ' ' && *command != '\n' && *command != '\t' 
          && *command != '\0'){
        command++;
      }
    }else if(*command == '>'){
      *command++ = '\0';
      while(*command == ' ' || *command == '\t'){
        *command++ = '\0';
      }
      out = command;
      while(*command != ' ' && *command != '\t' && *command != '\n' 
          && *command != '\0'){
        command++;
      }
    }else{
      *args++ = command;
      while(*command != ' ' && *command != '\n' && *command != '\0'){
        command++;
      }
    }
  }

  strcpy(input, in);
  strcpy(output, out);

  return runInBG;
}

/**
 * This method attempts to map a dos command to a unix command
 *
 * @param args
 *      the parsed command entered by the user
 *
 * @return if the command was successfully mapped
 */
bool mapCommands(char **args){
  if(strcmp(*args, "dir") == 0){
    *args = ls;
  } else if(strcmp(*args, "whereami") == 0){
    *args = pwd;
  } else if(strcmp(*args, "copy") == 0){
    *args = cp;
  } else if(strcmp(*args, "rename") == 0){
    *args = mv;
  } else if(strcmp(*args, "delete") == 0){
    *args = rm;
  } else if(strcmp(*args, "edit") == 0){
    *args = emacs;
  } else if(strcmp(*args, "type") == 0){
    *args = cat;
  } else if(strcmp(*args, "quit") == 0){
    //leave as is
  } else {
    //bad command
    return false;
  }
  //good command
  return true;
}

/**
 * This method executes the command entered by the user and performs
 * any optional things they passed in as well.
 *
 * @param args
 *      the parsed command of the user
 * @param runInBG
 *      whether or not to run the process in the back ground
 * @param input
 *      place to redirect input
 * @param output
 *      place to redirect output
 */
void execute(char **args, const bool runInBG, char *input, char *output){
  pid_t pid = fork();
  
  if(pid == 0){
    if(*input != '\0'){
      if(freopen(input, "r", stdin) == NULL){
        printf("Redirect of input failed");
        exit(3);
      }
    }
    if(*output != '\0'){
      if(freopen(output, "w", stdout) == NULL){
        printf("Redirect of output failed");
        exit(3);
      }
    }
    if(execvp(*args, args) < 0){
      printf("Executing %s failed", *args);
      exit(1);
    }
  } else {
    if(!runInBG){
      waitpid(pid, NULL, 0);
    }
  }
}