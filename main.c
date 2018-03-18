#include "parseline.h"
#include "mush.h"

#define DEBUG 0
#define DEBUG2 0

/* the number of child processes running */
static int num_children = 0;

int main(int argc, char *argv[]){

    char pipeline[CMDLINE_LEN+1] = {'\0'};
    cmd *cmd_list[PIPELINE_LEN] = {'\0'};
    pid_t parent = getpid();
    pid_t child;
    int one[2] = {0}, two[2] = {0};
    int scriptfile, script_read;
    char *script_cur = NULL, *script_break = NULL;
    char scriptbuf[CMDLINE_LEN] = {'\0'};
    int i = 0, err, quit = 1, num_cmds = 0;
    int status, pipeline_len = 0;
    struct sigaction sa;
    sigset_t set, old;

    sa.sa_handler = int_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    sigemptyset(&set);
    sigemptyset(&old);
    sigaddset(&set, SIGINT);

    if (argc == 2){
        if ((scriptfile = open(argv[1], O_RDONLY)) < 0){
            perror("scriptfile open");
            exit(EXIT_FAILURE);
        }
        script_read = read(scriptfile, scriptbuf, CMDLINE_LEN);
        if (script_read < 0){
            perror("scriptfile read");
            exit(EXIT_FAILURE);
        }
        script_cur = scriptbuf;
        script_break = scriptbuf;
    }
    else if (argc != 1){
        fprintf(stderr, "usage: ./mush [scriptfile]\n");
        exit(EXIT_FAILURE);
    }

    while (quit){

        for (i = 0; i < pipeline_len; i++){
            pipeline[i] = '\0';
        }
        for (i = 0; i < num_cmds; i++){
            cmd_list[i] = NULL;
        }

        if (argc == 1){
            if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO)){
                printf("8-P ");
            }
            if (fgets(pipeline, CMDLINE_LEN, stdin) == NULL){
                if (feof(stdin)){
                    printf("\n");
                    break;
                }
                else{
                    continue;
                }
            }
        }
        else if (argc == 2){
            script_cur = script_break;
            if (*script_cur == '\0'){
                script_read = read(scriptfile, scriptbuf, CMDLINE_LEN);
                if (script_read < 0){
                    perror("scriptfile read");
                    exit(EXIT_FAILURE);
                }
                else if (script_read == 0){
                    break;
                }
            }

            script_break = strchr(script_cur, '\n');
            script_break++;
            strncpy(pipeline, script_cur, script_break-script_cur);

        }

        pipeline_len = strlen(pipeline);
        if (pipeline_len == 0){
            printf("\n");
            break;
        }
        else if (pipeline_len == 1 && *pipeline == '\n'){
            continue;
        }

        err = parse_pipeline(cmd_list, pipeline);
        if (err < 0){
            continue;
        }

        if (pipe(one) < 0){
            perror("one pipe");
            continue;
        }
        if (pipe(two) < 0){
            perror("two pipe");
            continue;
        }

        for (num_cmds = 0; cmd_list[num_cmds] != NULL; num_cmds++){
            /* do nothing */
        }

        for (i = 0; i < num_cmds; i++){
            if (strcmp(cmd_list[i]->argv[0], "cd") == 0){
                err = run_cd(cmd_list[i]);
                break;
            }

            else if ((strcmp(cmd_list[i]->argv[0], "exit") == 0) ||
                    (strcmp(cmd_list[i]->argv[0], "quit") == 0) ||
                    (strcmp(cmd_list[i]->argv[0], "q") == 0)){
                quit = 0;
                break;  
            }

            num_children++;

            /* blocks SIGINTs during the fork */
            sigprocmask(SIG_BLOCK, &set, &old);

            child = fork();
            if (child < 0){
                perror(cmd_list[i]->argv[0]);
                break;
            }

            if (child == 0){
                /* child */               
                
                /* unblock SIGINTs */
                sigprocmask(SIG_SETMASK, &old, NULL);

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

                /* unblock SIGINTs */
                sigprocmask(SIG_SETMASK, &old, NULL);
                if (num_cmds > 3 && i > 3){
                    one[0] = two[0];
                    one[1] = two[1];
                    if (pipe(two) < 0){
                        perror("pipe two");
                        break;
                    }
                }
            }

        } /* end for */

        close_pipe(one);
        close_pipe(two);
        while (num_children){
            wait(&status);
            if (WIFEXITED(status)){
                num_children--;
                status = WEXITSTATUS(status);
            }
            else{
                status = EXIT_FAILURE;
            }

            if (status == EXIT_FAILURE){
                break;
            }
        }

        fflush(stdout);

    } /* end while */

    for (i = 0; cmd_list[i] != NULL; i++){
        free(cmd_list[i]);
    }

    return 0;
}
