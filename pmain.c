#include "parseline.h"

#define DEBUG 0


/* error message if command line length limit is exceeded */
const char *mcmdline_len = "command too long";
/* error message if pipeline has too many elements */
const char *mpipeline_len = "pipeline too deep";
/* error message if individual command exceeds arguments limit */
const char *mcmdargs_len = "too many arguments";
/* error message if pipeline has an empty stage */
const char *mempty_pipe = "invalid null command";
/* error message if  pipeline if a command has multiple redirects
 * or missing redirect file name for input */
const char *minput_redir = "bad input redirection";
/* error message if  pipeline if a command has multiple redirects
 * or missing redirect file name for input */
const char *moutput_redir = "bad output redirection";
/* error message if stage has both input redirect and pipe in */
const char *mambig_input = "ambiguous input";
/* error message if stage has both output redirect and pipe out */
const char *mambig_output = "ambiguous output";


int main(int argc, char *argv[]){

	int err = 0, index = 0, stage = 0, pipe_index = -1;
	int num_pipes = -1, i = 0;
	char cmdline[CMDLINE_LEN] = {'\0'};
	char *cp = NULL;
	cmd *cmd_list[PIPELINE_LEN] = {'\0'};
	cmd *temp_cmd = NULL;

#if DEBUG
	/*strcpy(cmdline, "ls <\n");*/
	/*strcpy(cmdline, "ls < a | more < file\n");*/
	/*strcpy(cmdline, \
	"This command has way more than the required ten arguments.");
	strcat(cmdline, " It really should make up its mind.\n");*/
	/*strcpy(cmdline, "cat < foo > bar\n");*/
	strcpy(cmdline, "ls < one | more | sort\n");
	/*strncpy(cmdline, "ls < one > two three four\n", 27);*/
	/*strcpy(cmdline, "ls | more\n");*/
	/*strcpy(cmdline, "ls < one two three |	| > more | sort\n");*/
#endif
#if !DEBUG
	printf("line: ");
	fgets(cmdline, CMDLINE_LEN, stdin);
#endif
	/* strip the newline */
	index = strcspn(cmdline, "\n");
	cmdline[index] = '\0';

	/* if the command line is too long, quit */
	if (strlen(cmdline) > CMDLINE_LEN){
		fprintf(stderr, "%s\n", mcmdline_len);
		exit(EXIT_FAILURE);
	}
	
	/* if the pipeline is too long, quit */
	num_pipes = strcount(cmdline, "|");
	if (num_pipes > PIPELINE_LEN){
		fprintf(stderr, "%s\n", mpipeline_len);
		exit(EXIT_FAILURE);
	}
	
	if (cmdline[0] == '|'){
		fprintf(stderr, "%s\n", mempty_pipe);
		exit(EXIT_FAILURE);
	}

	cp = cmdline;
	do{
		if (*cp == '|'){
			cp++;
		}
		pipe_index = char_index(cp, "|");
		/* handle each stage */
		cmd_list[i] = (cmd *)calloc(1, sizeof(cmd));
		if (cmd_list[i] == NULL){
			perror("cmd_list calloc");
			exit(EXIT_FAILURE);
		}
		*cmd_list[i] = empty_cmd();
		temp_cmd = cmd_list[i];

		strncpy(temp_cmd->line, cp, \
			(pipe_index == -1)? strlen(cp): pipe_index);
		temp_cmd->stage = stage;
		err = check_line(temp_cmd->line);
		if (err == -1){
			fprintf(stderr, "%s\n", mempty_pipe);
			exit(EXIT_FAILURE);
		}

		err = parse_args(temp_cmd);
		switch(err){
			case -1:
				fprintf(stderr, "%s\n", minput_redir);
				exit(EXIT_FAILURE);
			case -2:
				fprintf(stderr, "%s\n", moutput_redir);
				exit(EXIT_FAILURE);
			case -3:
				fprintf(stderr, "%s\n", mcmdargs_len);
				exit(EXIT_FAILURE);
		}
				
		err = set_pipes(temp_cmd, num_pipes);
		switch(err){
			case 1:
				fprintf(stderr, "%s\n", mambig_input);
				exit(EXIT_FAILURE);
			case 2:
				fprintf(stderr, "%s\n", mambig_output);
				exit(EXIT_FAILURE);
		}

		/*print_cmd(&temp_cmd);*/

		stage++;
		i++;
	} while ((cp = strstr(cp+1, "|")) != NULL);

	/* print all of the cmds stored */
	for (i = 0; cmd_list[i] != NULL; i++){
		print_cmd(cmd_list[i]);
		printf("\n");
		free(cmd_list[i]);
	}

	return 0;
}
