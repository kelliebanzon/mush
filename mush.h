#ifndef MUSHH
#define MUSHH

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1

/* TODO: delete this */
/* if there is an error, prints the given error message */
void print_err(int err, char *msg);

/* populates the input and output file descriptors of the cmd struct
 * returns 0 on success, -1 on failure */
int set_cmd_fds(cmd *c);

/* returns the file descriptors for input and output of a command as
 * a list in the format {infile, outfile} TODO: */

/* runs the "cd" command
 * returns 0 on success, -1 on failure */
int run_cd(cmd *c);

/* runs the command
 * returns 0 on success, -1 on failure */
int run_cmd(cmd *c);




#endif
