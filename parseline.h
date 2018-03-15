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

/* error message if command line length limit is exceeded */
#define MCMDLINE_LEN "command too long"
/* error message if pipeline has too many elements */
#define MPIPELINE_LEN "pipeline too deep"
/* error message if individual command exceeds arguments limit */
#define MCMDARGS_LEN "too many arguments"
/* error message if pipeline has an empty stage */
#define MEMPTY_PIPE "invalid null command"
/* error message if  pipeline if a command has multiple redirects
 * or missing redirect file name for input */
#define MINPUT_REDIR "bad input redirection"
/* error message if  pipeline if a command has multiple redirects
 * or missing redirect file name for input */
#define MOUTPUT_REDIR "bad output redirection"
/* error message if stage has both input redirect and pipe in */
#define MAMBIG_INPUT "ambiguous input"
/* error message if stage has both output redirect and pipe out */
#define MAMBIG_OUTPUT "ambiguous output"


typedef struct command{
	/* the full line of text associated with this command,
	 * including spaces */
	char line[CMDLINE_LEN];
	/* the number of keywords associated with this command
	 * keywords do not include symbols or redirections */
	int argc;
	/* the keywords */
	char argv[CMDARGS_LEN][CMDLINE_LEN];
	/* where to send the input
	 * - if < 0: indicates a pipe, where the pipe number is the
	 *   		 absolute value + 1 (to account for the fact that
	 *   		 the pipe of stage 0 cannot be 0, else if would
	 *   		 overlap with stdin)
	 * - if = 0, 1, or 2: indicates stdin, stdout, stderr
	 * - if > 2: indicates a file descriptor */
	int input;
	/* if input is a file, this is the name of the file
	 * otherwise, remains NULL */
	char input_name[CMDLINE_LEN];
	/* where to send the output 
	 * - if < 0: indicates a pipe, where the pipe number is the
	 *   		 absolute value + 1 (to account for the fact that
	 *   		 the pipe of stage 0 cannot be 0, else if would
	 *   		 overlap with stdin)
	 * - if = 0, 1, or 2: indicates stdin, stdout, stderr
	 * - if > 2: indicates a file descriptor */
	int output;
	/* if output is a file, this is the name of the file
	 * otherwise, remains NULL */
	char output_name[CMDLINE_LEN];
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

/* separates the arguments using the spaces between them
 * stores the char pointers in argv
 * redirection arguments are handled here (instead
 * of parsing them into the char array and then handling
 * them) due to the technique of initializing the char
 * array with the maximum argc value
 * returns
 * - 0 on success
 * - -1 if input redirection lacks a destination
 * - -2 if output redirection lacks a destination
 * - -3 if too many arguments are found */
int parse_args(cmd *c);

/* returns the appropriate error values for malformatted redirects 
 * returns -1 for bad input redirection 
 * returns -2 for bad output redirection
 * returns 0 if everything is fine */
int bad_redirect(char *c);

/* sets the input/output of a command to the given file
 * if inout is 0: fname is stored to input_name
 * if inout is 1: fname is stored to output_name
 * later: will actually open the file and put the appropriate file
 * descriptor in the command input attribute
 * for now, simply saves the name of the file to input_name/output_name
 * sets command input/ouput field to -1 
 * returns 0 on success
 * returns -1 if there are too many input redirections
 * returns -2 if there are too many output redirections */
int set_inoutname(cmd *c, char *direct, char *fname);

/* sets the input and output attributes of a command based on the pipes 
 * if there is a pipe preceding the command, set the input to that pipe
 * if there is a pipe following the command, set the output to that pipe
 * otherwise, leave the input/output as the default values
 * returns 0 on success
 * returns 1 if there is a input pipe and an input redirection
 * returns 2 if there is an output pipe and an output redirection */
int set_pipes(cmd *c, int num_pipes);

/* prints out the contents of the command struct 
 * returns 0 on success, -1 on failure */
int print_cmd(cmd *c);

/* returns a formatted string describing the command's output
 * returns NULL on error
 * buf is a buffer to return
 * type is:
 * - 0 for input
 * - 1 for output
 * returns one of:
 * - "original stdin" or "original stdout"
 * - "pipe from stage #" or "pipe to stage #"
 * - file name */
char *format_inout(cmd *c, char *buf, int type);

/* returns a list of the arguments passed to a command,
 * each separated by commas and quotation marks */
char *format_argv(cmd *c, char *buf);

/* given a pipeline, parses and stores each stage into cmd_list
 * returns 0 on success
 * returns -1 on error */
int parse_pipeline(cmd **cmd_list, char *pipeline);

/* prints the contents of all the commands in the pipeline
 * formatted as the output of asgn05 (parseline) was */
void print_pipeline(cmd **cmd_list);

#endif
