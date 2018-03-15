#include "parseline.h"
#include "mush.h"

#define DEBUG 0
#define DEBUG2 1

int main(int argc, char *argv[]){

	char pipeline[CMDLINE_LEN] = {'\0'};
#if DEBUG2
	char buf[512] = {'\0'};
#endif
	cmd *cmd_list[PIPELINE_LEN] = {'\0'};
	int old[2] = {0}, new[2] = {0};
	int i = 0, err, exit = 1, num_cmds = 0;

	while (exit){

#if !DEBUG
		printf("8-P ");
		fgets(pipeline, CMDLINE_LEN, stdin); /* TODO: tty nonsense? */
#endif
#if DEBUG
		/*strcpy(pipeline, "ls < in | more | sort | wc\n");*/
		strcpy(pipeline, "cd /home/kmbanzon/Documents\n");
#endif

		err = parse_pipeline(cmd_list, pipeline);
		if (err < 0){
			continue;
		}

#if DEBUG
		if (err == 0){
			print_pipeline(cmd_list);
		}
#endif

		for (num_cmds = 0; cmd_list[num_cmds] != NULL; num_cmds++){
			/* do nothing */
		}
		for (i = 0; i < num_cmds; i++){
			if (strcmp(cmd_list[i]->argv[0], "cd") == 0){
#if DEBUG2
				printf("%s\n", getcwd(buf, 512));
#endif
				err = run_cd(cmd_list[i]); /* TODO: */
				if (err < 0){
					continue;
				}
#if DEBUG2
				printf("%s\n", getcwd(buf, 512));
#endif
			}
		} /* TODO: */

#if DEBUG
		for (i = 0; i < strlen(pipeline); i++){
			pipeline[i] = '\0';
		}
#endif

#if DEBUG
		exit = 0;
#endif

	}






	return 0;
}
