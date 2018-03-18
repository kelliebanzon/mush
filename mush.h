#ifndef MUSHH
#define MUSHH

#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define READ 0
#define WRITE 1

/* the number of child processes running */
static int num_children;

/* if this command has a file input, open that file and populate
 * the input field with that file descriptor
 * returns 0 on success, -1 on failure */
int set_input_fd(cmd *c);

/* if this command has a file output, open that file and populate
 * the input field with that file descriptor
 * returns 0 on success, -1 on failure */
int set_output_fd(cmd *c);

/* handles the < and > redirects, if they exist, for a pipeline
 * with only one stage
 * returns 0 on success, -1 on failure */
int no_pipes(cmd *c);

/* for pipelines with two commands
 * handles piping and input/output redirection, if applicable
 * returns 0 on success, -1 on failure */
int one_pipe(cmd *c, int num_cmds, int *one);

/* for pipelines with three or more commands
 * handles piping and input/output redirection, if applicable
 * returns 0 on success, -1 on failure */
int redirect_pipes(cmd *c, int num_cmds, int *one, int *two);

/* closes both ends of the pipe */
void close_pipe(int *pipe);

/* runs the "cd" command
 * returns 0 on success, -1 on failure */
int run_cd(cmd *c);

/* runs the command
 * returns 0 on success, -1 on failure */
int run_cmd(cmd *c);

/* closes both ends of the pipe */
void close_pipe(int *pipe);

/* closes both ends of the pipes */
void close_pipes(int *one, int *two);

/* catches SIGINT's
 * waits for children to terminate, then prints the prompt to 
 * continue interactive mush mode */
void int_handler(int signum);


#endif
