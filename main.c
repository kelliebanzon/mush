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
	pid_t parent = getpid();
	pid_t child;
	int old[2] = {0}, new[2] = {0};
	int i = 0, err, quit = 1, num_cmds = 0;
	int flag = 0;

	while (quit){

#if !DEBUG
		printf("8-P ");
		fgets(pipeline, CMDLINE_LEN, stdin); /* TODO: tty nonsense? */
#endif
#if DEBUG
		strcpy(pipeline, "ls -l\n");
		/*strcpy(pipeline, "ls | more | sort | wc\n");*/
		/*strcpy(pipeline, "cd /home/kmbanzon/Documents\n");*/
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
				err = run_cd(cmd_list[i]); /* TODO: can't handle ~/ paths */
				if (err < 0){
					flag = 1;
				}
#if DEBUG2
				printf("%s\n", getcwd(buf, 512));
#endif
			}
			else{
				child = fork();
				if (child < 0){
					perror(cmd_list[i]->argv[0]);
					flag = 1;
					continue;
				}

				if (getpid() == parent){
					wait(NULL);
				}

				else if (child == 0){
					err = run_cmd(cmd_list[i]);
					if (err < 0){
						perror(cmd_list[i]->argv[0]);
						exit(EXIT_FAILURE);
					}
				}

				/*if (err < 0){
					flag = 1;
					continue;
				}*/
			}
			if (flag){
				continue;
			}
		} /* TODO: */

#if DEBUG
		for (i = 0; i < strlen(pipeline); i++){
			pipeline[i] = '\0';
		}
#endif

#if DEBUG
		quit = 0;
#endif

	}






	return 0;
}
