#include "parseline.h"
#include "mush.h"

int set_input_fd(cmd *c){
    if (*c->input_name != '\0'){
        c->input = open(c->input_name, O_RDONLY);
        if (c->input < 0){
            fprintf(stderr, "\"%s\" input: ", c->argv[0]);
            perror(NULL);
            return -1;
        }
    }
    return 0;
}

int set_output_fd(cmd *c){
    if (*c->output_name != '\0'){
        c->output = open(c->output_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (c->output < 0){
            fprintf(stderr, "\"%s\" output: ", c->argv[0]);
            perror(NULL);
            return -1;
        }
    }
    return 0;
}

int no_pipes(cmd *c){
    if (c->input > 2){
        if (dup2(c->input, STDIN_FILENO) < 0){
            perror("no_pipes input dup2");
            return -1;
        }
    }
    if (c->output > 2){
        if (dup2(c->output, STDOUT_FILENO) < 0){
            perror("no_pipes output dup2");
            return -1;
        }
    }
    return 0;
}

int redirect_pipes(cmd *c, int num_cmds, int *one, int *two){
    if (c->stage == 0){
        if (dup2(c->input, one[READ]) < 0){
            perror("dup2 1.1");
            return -1;
        }
        if (dup2(one[WRITE], STDOUT_FILENO) < 0){
            perror("dup2 1.2");
            return -1;
        }
    }
    else if (c->stage == num_cmds-1){
        if (dup2(two[READ], STDIN_FILENO) < 0){
            perror("dup2 2.1");
            return -1;
        }
        if (dup2(c->output, two[WRITE]) < 0){
            perror("dup2 2.2");
            return -1;
        }
    }
    else{
        if (dup2(one[READ], STDIN_FILENO) < 0){
            perror("dup2 3.1");
            return -1;
        }
        if (dup2(two[WRITE], STDOUT_FILENO) < 0){
            perror("dup2 3.2");
            return -1;
        }
    }
    return 0;
}

void close_pipe(int *pipe){
    close(pipe[READ]);
    close(pipe[WRITE]);
}

/*int set_cmd_fds(cmd *c){
    if (*c->input_name != '\0'){
        c->input = open(c->input_name, O_WRONLY, O_CREAT | O_TRUNC, 0446);
        if (c->input < 0){
            fprintf(stderr, "\"%s\" input: ", c->argv[0]);
            perror(NULL);
            return -1;
        }
    }
    if (*c->output_name != '\0'){
        c->output = open(c->output_name, O_WRONLY, O_CREAT | O_TRUNC, 0446);
        if (c->output < 0){
            fprintf(stderr, "\"%s\" output: ", c->argv[0]);
            perror(NULL);
            return -1;
        }
    }
    return 0;
}*/

/* TODO: can't handle ~/ paths */
int run_cd(cmd *c){
    int err;
    if (c->stage != 0){
        fprintf(stderr, "cd: cannot change directories in a pipeline\n");
        return -1;
    }
    if (c->argc != 2){
        fprintf(stderr, "usage: cd <path>\n");
        return -1;
    }
    err = chdir(c->argv[1]);
    if (err < 0){
        perror("cd");
        return -1;
    }
    return 0;
}

int run_cmd(cmd *c){
    char *a[CMDARGS_LEN+2];
    int err, i;
    a[0] = "./mush";
    for (i = 1; i < c->argc; i++){
        a[i] = c->argv[i];
    }
    a[i] = NULL;
    err = execvp(c->argv[0], a);
    return err;
}

void close_pipes(int *one, int *two){
    close(one[READ]);
    close(one[WRITE]);
    close(two[READ]);
    close(two[WRITE]);
}
