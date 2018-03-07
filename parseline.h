#ifndef PARSELINEH
#define PARSELINEH

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>


/* the maximum length of the command line (in bytes) */
#define CMDLINE_LEN 512
/* the maximum length of the pipeline (in bytes) */
#define PIPELINE_LEN 10
/* the maximum number of arguments to any one command (in bytes) */
#define CMDARGS_LEN 10


typedef struct command{
	/* the full line of text associated with this command,
	 * including spaces */
	char line[CMDLINE_LEN];
	/* the number of keywords associated with this command
	 * keywords do not include symbols or redirections */
	int argc;
	/* the keywords */
	char *argv[CMDARGS_LEN];
	/* where to send the input
	 * - if < 0: indicates a pipe, where the pipe number is the
	 *   		 absolute value + 1 (to account for the fact that
	 *   		 the pipe of stage 0 cannot be 0, else if would
	 *   		 overlap with stdin)
	 * - if = 0, 1, or 2: indicates stdin, stdout, stderr
	 * - if > 2: indicates a file descriptor */
	int input;
	/* where to send the output 
	 * - if < 0: indicates a pipe, where the pipe number is the
	 *   		 absolute value + 1 (to account for the fact that
	 *   		 the pipe of stage 0 cannot be 0, else if would
	 *   		 overlap with stdin)
	 * - if = 0, 1, or 2: indicates stdin, stdout, stderr
	 * - if > 2: indicates a file descriptor */
	int output;
	/* the stage this command is associated with */
	int stage;
	/* the next command in the pipeline */
	struct command *next;
}(cmd);


/* returns the index of the first occurrence of needle in haystack
 * replicates behavior of strstr, but returns int index instead of char*
 * if no space is found, returns -1 */
int char_index(char *haystack, char *needle);

/* returns the number of occurrences of a given char in a string */
int strcount(char *haystack, char *needle);

/* initializes and returns an empty cmd
 * default empty values are:
 * - line: NULL
 * - argc: 0
 * - every index of **argv is a nul char
 * - input: stdin
 * - output: stdout
 * - stage: -1
 * - next: NULL
 * Note: Some of these values are deliberately inaccurate "empty" values
 * (e.g. stage is null), whereas some are potentially accurate
 * "default" values (e.g. input is stdin). This is done for ease of
 * execution here; all values will be properly set later. */
cmd empty_cmd();

/* checks that a line contains a valid command
 * invalid commands include: spaces, new lines, empty strings, tabs,
 * carriage returns, and other space characters
 * returns 0 for valid lines, -1 for invalid ones */
int check_line(char *l);

/* sets the input and output attributes of a command based on the pipes 
 * if there is a pipe preceding the command, set the input to that pipe
 * if there is a pipe following the command, set the output to that pipe
 * otherwise, leave the input/output as the default values
 * returns 0 on success, -1 on error */
int set_pipes(cmd *c, int num_pipes);

/* checks that input and output redirections are valid
 * allows < > redirects if pipes are not being used
 * returns 0 if all redirections are valid
 * returns -1 if input redirection is invalid
 * returns -2 if output redirection is invalid */
int check_redirects(cmd *c);

#endif
