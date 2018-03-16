#include "parseline.h"
#include "mush.h"

#define DEBUG 1
#define DEBUG2 0

int main(int argc, char *argv[]){

    char pipeline[CMDLINE_LEN] = {'\0'};
#if DEBUG2
    char buf[512] = {'\0'};
#endif
    cmd *cmd_list[PIPELINE_LEN] = {'\0'};
    pid_t parent = getpid();
    pid_t child;
    int one[2] = {0}, two[2] = {0};
    int i = 0, err, quit = 1, num_cmds = 0, num_children = 0;
    int status;
    int max = 4;

    while (quit){

#if !DEBUG
        printf("8-P ");
        fgets(pipeline, CMDLINE_LEN, stdin); /* TODO: tty nonsense? */
#endif
#if DEBUG
        /*strcpy(pipeline, "ls -l\n");*/
        /*strcpy(pipeline, "ls | sort < foo\n");*/
        strcpy(pipeline, "ls -tl | sort\n");
        /*strcpy(pipeline, "ls | more | sort | wc\n");*/
        /*strcpy(pipeline, "cd /home/kmbanzon/Documents\n");*/
#endif

        err = parse_pipeline(cmd_list, pipeline);
        if (err < 0){
            continue;
        }

#if DEBUG2
        if (err == 0){
            print_pipeline(cmd_list);
        }
#endif

        for (num_cmds = 0; cmd_list[num_cmds] != NULL; num_cmds++){
            /* do nothing */
        }

        if (pipe(one) < 0){
            perror("one pipe");
            exit(EXIT_FAILURE);
        }
        if (pipe(two) < 0){
            perror("two pipe");
            exit(EXIT_FAILURE);
        }
    for (i = 0; i < max; i++){
        if (!(child = fork())){
            /* child */
            if (i == 0){
                if ( -1 == dup2(one[WRITE],STDOUT_FILENO) ) {
                    perror("dup2");
                    exit(-1);
                }
            }
            else if (i < max-1){
                if ( -1 == dup2(one[READ],STDIN_FILENO) ) {
                    perror("dup2");
                    exit(-1);
                }
                if ( -1 == dup2(two[WRITE],STDOUT_FILENO) ) {
                    perror("dup2");
                    exit(-1);
                }
            }
            else if (i == max-1){
                if ( -1 == dup2(two[READ],STDIN_FILENO) ) {
                    perror("dup2");
                    exit(-1);
                }
            }

            /* clean up */
            close_pipes(one, two);
            switch(i){
                case 0:
                    execl("/bin/ls", "ls", NULL);
                    break;
                case 1:
                    execl("/bin/more", "more", NULL);
                    break;
                case 2:
                    execl("/bin/sort", "sort", "-r", NULL);
                    break;
                case 3:
                    execl("/bin/sort", "sort", NULL);
                    break;
            }
            perror(NULL);
            exit(-1);
        }
    }

    /* parent stuff */
    /* clean up */
    num_children++;
    close_pipes(one, two);
    while (num_children){
        int status;
        if (-1 == wait(&status)){
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
        }
    }
    exit(0);
 }
 return 0;
}
/*        for (i = 0; i < num_cmds; i++){
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
                
                if (cmd_list[i]->stage == 0){
                    if (set_input_fd(cmd_list[i]) < 0){
                        exit(EXIT_FAILURE);
                    }
                }
                else if (cmd_list[i]->stage == num_cmds-1){
                    if (set_output_fd(cmd_list[i]) < 0){
                        exit(EXIT_FAILURE);
                    }
                }
#if DEBUG
                fprintf(stderr, "STDIN: %d, STDOUT: %d\n", fileno(stdin), fileno(stdout));
#endif

                err = redirect_pipes(cmd_list[i], num_cmds, one, two);
#if DEBUG
                fprintf(stderr, "STDIN: %d, STDOUT: %d\n", fileno(stdin), fileno(stdout));
#endif
                if (err < 0){
                    exit(EXIT_FAILURE);
                }


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
                num_children++;
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
            }

        }

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
}*/
