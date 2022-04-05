/**
 * @file    simple_shell.c
 * @brief   This program executes the simple shell.
 *          This program provides executing user commands,
 *          input/output redirection, background execution,
 *          and communication via pipes.
 * @author  Seunghyun Kim
 */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))  /* Return the larger value. */

/** Status codes of commandline execution for ExecuteShellCommand() function. */
#define EXEC_NORMAL 0
#define EXEC_REDIRECT_RIGHT 1
#define EXEC_REDIRECT_LEFT 2
#define EXEC_PIPE 3

#define MAX_LINE 80  /** The maximum length command. */
#define PARSE_ERROR -1  /** Return value of parse error. */

int ParseCommand(char* commandline, char* args[]);
int PrepareVariables(int* args_size, char* args[], int* background,int* should_run);
int ExecuteShellCommand(int args_size, char* args[]);

/**
 * @brief This is the main function of myshell.c.
 * @param void Does not need any parameters.
 * @return Execution success status.
 */
int main(void) {
  char* args[MAX_LINE/2 + 1];  /** Command line arguments. */
  char commandline[MAX_LINE];  /** Entered commandline. */
  int should_run = 1,  /** Flag to determine when to exit program. */
      background,  /** Whether the commandline contains &. */
      pstatus,
      args_size;
  pid_t pid;

  while (should_run) {
    fputs("sshell$ ", stdout);  /* Print simple shell prompt. */
    fflush(stdout);

    /* Get commandline from standard input. */
    fgets(commandline, MAX_LINE, stdin);
    args_size = ParseCommand(commandline, args);  /* Parse commandline. */

    /**
     * Set main() function's variables.
     * PrepareVariables() returns 0 if args has invalid command
     */ 
    if(PrepareVariables(&args_size, args, &background, &should_run) == 0) {
      continue;
    }

    pid = fork();  /* Create a child process. */

    if(pid < 0) {  /* Process forking error. */
      exit(1);
    } else if(pid == 0) {  /* Child process. */
      ExecuteShellCommand(args_size, args);
      exit(0);
    } else {  /* Parent process. */
      /**
       * If commandline contains "&" token then
       * the child process will be executing in background
       * and the parent process does not wait for child process.
       */
      if (background == 0) {  /* Wait for child process. */
        waitpid(pid, &pstatus, 0);  
      } else {
        /* Background execution. WNOHANG option : Does not wait any child */
        waitpid(pid, &pstatus, WNOHANG);
      }
    }
  }

  return 0;
}

/**
 * @brief This function is that parses a string into tokens.
 *        String is divided by spaces, returns, tabs.
 * @param commandline Command string to parsing.
 * @param args        Array for tokens.
 * @return The number of tokens normally parsed. (return -1 in error)
 */
int ParseCommand(char* commandline, char* args[]) {
  int args_size = 0;  /* The number of tokens */
  char delimiters[] = {' ', '\n', '\t'};  /* Delimiters for dividing tokens. */
  char* token;

  if(strlen(commandline) < 0) {  /* Commandline input error */
    return PARSE_ERROR;
  }

  /* 
   * Divide commandline by delimiters until there is no string left.
   * strtok() function returns the token that are divided by the delimiters.
   * strtok() function returns the NULL value when string can't be parsed.
   * The leftover string can be used by strtok(NULL, delimiters);
   */
  token = strtok(commandline,delimiters);
  args[args_size++] = token;

  while(token != NULL) {
    token = strtok(NULL, delimiters);
    args[args_size++] = token;
  }

  return MAX(PARSE_ERROR, args_size - 1);
}

/**
 * @brief This function is that sets the main function's variables.
 * @param args_size Pointer of variable that contains the number of arguments.
 * @param args  Array of the command arguments
 * @param background  Pointer of variable
 *                    that shows whether the commandline contains &.
 * @param should_run  Pointer of variable
 *                    that flags to determine when to exit program.
 * @return  Whether the simple shell continues to run.
 *          (return 0 when shell needs restarting)
 */
int PrepareVariables(int* args_size, char* args[], int* background, int* should_run) {
  /**
   *  Whether the simple shell continues to run.
   *  handle = 0 when shell needs restarting.
   */
  int handle = 0;

  (*should_run) = 0;  
  (*background) = 0;

  if (*args_size == PARSE_ERROR ||  /* Unexpected parsing error. */
      *args_size == 0  /* Nothing inputed */) {
    /* Ignore this commandline and continue the simple shell */
    (*should_run) = 1;
  } else if (strcmp(args[0], "exit") == 0) {
    /* Stop simple shell program. */
  } else if (strcmp(args[(*args_size) - 1], "&") == 0) {
    /*
     * If args contains the "&", set the value of background to 1.
     * And set the value of args where contains "&" to NULL.
     * Then, execvp() function will not use "&" as parameter.
     */
    handle = 1;
    (*should_run) = 1;
    (*background) = 1;

    /* Here are changes of args and args_size */
    (*args_size)--;
    args[*args_size] = NULL;
  } else {
    handle = 1;
    (*should_run) = 1;
  }

  return handle;  /* return 0 in restarting, return 1 in continue program */
}

/**
 * @brief This function is that analyzes and executes command arguments.
 * @param args_size The number of arguments
 * @param args Array of the command arguments
 * @return Execution success status.
 */ 
int ExecuteShellCommand(int args_size, char* args[]) {
  int pivot = 0,  /** Index of pipe or redirection symbol */
      /** 
       * Type of this execution.
       * EXEC_NORMAL(0): Only one command
       * EXEC_REDIRECT_RIGHT(1): Right redirection
       * EXEC_REDIRECT_LEFT(2): Left redirection
       * EXEC_PIPE(3): Pipe
       */
      exe_type = EXEC_NORMAL;

  /* Check pivot index and execution type by for loop */
  for (pivot = 0; pivot < args_size && exe_type == 0; pivot++) {
    if (strcmp(args[pivot], ">") == 0) {
      exe_type = EXEC_REDIRECT_RIGHT;
    } else if (strcmp(args[pivot], "<") == 0) {
      exe_type = EXEC_REDIRECT_LEFT;
    } else if (strcmp(args[pivot], "|") == 0) {
      exe_type = EXEC_PIPE;
    }
  }

  /* Divide the two commands (files) by entering NULL in the pivot. */
  if (exe_type != EXEC_NORMAL) {
    args[--pivot] = NULL;
  }

  /* Execution commandline according to type. */
  switch(exe_type) {
    case EXEC_NORMAL:
      execvp(args[0], args);
      break;
    case EXEC_REDIRECT_RIGHT:
      {
        /* File descripter of the file. (if not exists the file, create it.) */
        int fd = open(args[pivot + 1], O_RDWR | O_CREAT, 0666);

        if (fd == -1) {  /* Open file failed error */
          exit(1);
        }

        /* Write output of command to file. */
        dup2(fd, 1);
        execvp(args[0], args);

        exit(0);
        break;
      }
    case EXEC_REDIRECT_LEFT:
      {
        /* File descripter of the file. This file is read-only. */
        int fd = open(args[pivot + 1], O_RDONLY);

        if (fd == -1) {  /* Open file failed error */
          exit(1);
        }

        /* Write file contents to standard input */
        dup2(fd, 0);
        execvp(args[0], args);

        exit(0);
        break;
      }
    case EXEC_PIPE:
      {
        /* The pipes. fd[0] : read-end, fd[1] : write-end */
        int fd[2] = {};

        if (pipe(fd) == -1) {  /* Pipeline failed error */
          exit(1);
        }

        pid_t pid = fork();

        // ls | less
        // stdin -> ls  -> pipe -> less -> stdout
        // ls : stdout -> pipe
        // less : stdin -> pipe (fd[0])
        
        if (pid < 0) {  /* Process forking error. */
          exit(1);
        } else if(pid == 0) {
          /* Child process opens write-end and closes read-end. ls */
          dup2(fd[1], 1); // 1 = stdout -> fd[1]
          close(fd[0]);
          execvp(args[0], &args[0]);  // ls 실행
        } else {
          /* Parent process opens read-end and closes write-end less */
          dup2(fd[0], 0); // 0 = stdin 대신에 fd[0] (read)
          close(fd[1]);
          execvp(args[pivot + 1], &args[pivot + 1]); // less
        }

        exit(0);
        break;
      }
    default:
      /* Occur exe_type error */
      return 1;
  }

  return 0;
}


