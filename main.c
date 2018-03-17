#include "parseline.h"
#include "mush.h"

#define DEBUG 0
#define DEBUG2 0

int main(int argc, char *argv[]){

    char pipeline[CMDLINE_LEN] = {'\0'};
    cmd *cmd_list[PIPELINE_LEN] = {'\0'};
    pid_t parent = getpid();
    pid_t child;
    int one[2] = {0}, two[2] = {0};
    int i = 0, err, quit = 1, num_cmds = 0, num_children = 0;
    int status;
#if DEBUG
    int while_count = -1;
#endif

    while (quit){

#if !DEBUG
        printf("8-P ");
        fgets(pipeline, CMDLINE_LEN, stdin); /* TODO: tty nonsense? */
#endif
#if DEBUG
        /*if (while_count == 0){
            strcpy(pipeline, "cat README | wc\n");
        }
        else if (while_count == 1){
            strcpy(pipeline, "ls -tl | sort\n");
        }
        else if (while_count == 2){
            strcpy(pipeline, "cat README | sort\n");
        }
        else{
            strcpy(pipeline, "ls -l\n");
        }*/
        if (while_count == 0){
            strcpy(pipeline, "ls | sort < foo\n");
        }
        else if (while_count == 1){
            strcpy(pipeline, "ls -tl | sort | wc\n");
        }
        else if (while_count == 2){
            strcpy(pipeline, "ls | more | sort | wc\n");
        }
        else{
            strcpy(pipeline, "ls -l\n");
        }
        /*strcpy(pipeline, "cd /home/kmbanzon/Documents\n");*/
#endif

        err = parse_pipeline(cmd_list, pipeline);
        if (err < 0){
            continue;
        }

        if (pipe(one) < 0){
            perror("one pipe");
            exit(EXIT_FAILURE);
        }
        if (pipe(two) < 0){
            perror("two pipe");
            exit(EXIT_FAILURE);
        }

#if DEBUG2
        if (err == 0){
            print_pipeline(cmd_list);
        }
        printf("parent process: %d\n", parent);
#endif

        for (num_cmds = 0; cmd_list[num_cmds] != NULL; num_cmds++){
            /* do nothing */
        }

        for (i = 0; i < num_cmds; i++){
            if (strcmp(cmd_list[i]->argv[0], "cd") == 0){
                err = run_cd(cmd_list[i]);
                if (err < 0){
                    break;
                }
            }

            else if ((strcmp(cmd_list[i]->argv[0], "exit") == 0) ||
                    (strcmp(cmd_list[i]->argv[0], "quit") == 0) ||
                    (strcmp(cmd_list[i]->argv[0], "q") == 0)){
                quit = 0;
                break;  
            }

            child = fork();
            if (child < 0){
                perror(cmd_list[i]->argv[0]);
                break;
            }

            if (child == 0){
                /* child */               

                if (cmd_list[i]->stage == 0){
                    if (set_input_fd(cmd_list[i]) < 0){
                        exit(EXIT_FAILURE);
                    }
                }
                if (cmd_list[i]->stage == num_cmds-1){
                    if (set_output_fd(cmd_list[i]) < 0){
                        exit(EXIT_FAILURE);
                    }
                }

                if (num_cmds == 1){
                    err = no_pipes(cmd_list[i]);
                }
                else if (num_cmds == 2){
                    err = one_pipe(cmd_list[i], num_cmds, one);
                }
                else{
                    err = redirect_pipes(cmd_list[i], num_cmds, one, two);
                }
                if (err < 0){
                    exit(EXIT_FAILURE);
                }
                close_pipe(one);
                close_pipe(two);

                err = run_cmd(cmd_list[i]);
                if (err < 0){
                    perror(cmd_list[i]->argv[0]);
                    exit(EXIT_FAILURE);
                }
                else{
                    exit(EXIT_SUCCESS);
                }
            }

            else if (getpid() == parent){
                /* parent */               
#if DEBUG2
                printf("%s: just spawned child process: %d\n", cmd_list[i]->argv[0], child);
#endif
                num_children++;
                if (num_cmds > 3){
                    one[0] = two[0];
                    one[1] = two[1];
                }
            }

        } /* end for */

        close_pipe(one);
        close_pipe(two);
        while (num_children){
            if (wait(&status) == -1){
                perror("wait");
            }
            else{
                num_children--;
                if (WIFEXITED(status)){
                    status = WEXITSTATUS(status);
                }
                else{
                    status = EXIT_FAILURE;
                }

                if (status == EXIT_FAILURE){
                    break;
                }
            }
        }

        for (i = 0; i < strlen(pipeline); i++){
            pipeline[i] = '\0';
        }

#if DEBUG
        /*quit = 0;*/
        if (while_count < 2){
            while_count++;
        }
        else{
            quit = 0;
        }
#endif

    } /* end while */


    return 0;
}
