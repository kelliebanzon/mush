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
	int num_pipes = -1;
	char cmdline[CMDLINE_LEN] = {'\0'};
	char *cp = NULL;
	cmd *cmd_list[PIPELINE_LEN] = {'\0'};
	cmd temp_cmd = {0};

#if DEBUG
	printf("%i\n", strcount("canta loupe", " "));
	printf("%i\n", strcount("can ta lo upe ", " "));
	printf("%i\n", strcount(" cantaloupe", " "));
	printf("%i\n", strcount(" cantaloupe ", " "));
	printf("%i\n", strcount("cantaloupe ", " "));
	printf("%i\n", strcount("cantaloupe", " "));
	printf("%i\n", strcount("", " "));
	printf("%i\n", strcount(" ", " "));
#endif

#if DEBUG
	/*strncpy(cmdline, "ls < one > two three four\n", 27);*/
	strcpy(cmdline, "ls < one two three | > more | sort\n");
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
		printf("%s\n", mcmdline_len);
		exit(EXIT_FAILURE);
	}
	
	/* if the pipeline is too long, quit */
	num_pipes = strcount(cmdline, "|");
	if (num_pipes > PIPELINE_LEN){
		printf("%s\n", mpipeline_len);
		exit(EXIT_FAILURE);
	}

	cp = cmdline;
	do{
		if (*cp == '|'){
			cp++;
		}
		pipe_index = char_index(cp, "|");
		/* handle each stage */
		temp_cmd = empty_cmd();
		strncpy(temp_cmd.line, cp, \
			(pipe_index == -1)? strlen(cmdline): pipe_index);
		temp_cmd.stage = stage;
		set_pipes(&temp_cmd, num_pipes);

		err = check_redirects(&temp_cmd);
		if (err == -1){
			printf("%s\n", minput_redir);
			exit(EXIT_FAILURE);
		}
		else if (err == -2){
			printf("%s\n", moutput_redir);
			exit(EXIT_FAILURE);
		}

		stage++;
	} while ((cp = strstr(cp+1, "|")) != NULL);



	return 0;
}