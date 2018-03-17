#ifndef MUSHH
#define MUSHH

#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1

int fileno(FILE *stream);

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

int one_pipe(cmd *c, int num_cmds, int *one);

int redirect_pipes(cmd *c, int num_cmds, int *one, int *two);

void close_pipe(int *pipe);

/* populates the input and output file descriptors of the cmd struct
 * returns 0 on success, -1 on failure */
/*int set_cmd_fds(cmd *c);*/

/* returns the file descriptors for input and output of a command as
 * a list in the format {infile, outfile} TODO: */

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




#endif
