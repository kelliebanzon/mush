#include "parseline.h"
#include "mush.h"

void print_err(int err, char *msg){
    if (err < 0){
        perror(msg);
    }
}

int set_cmd_fds(cmd *c){
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
}

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
    /*a[0] = (char *)calloc(strlen("./mush")+1, sizeof(char));
    if (a[0] == NULL){
        perror("run_cd");
        return -1;
    }
    else{
        strncpy(a[0], "./mush", strlen("./mush"));
    }*/
    a[0] = "./mush";
    for (i = 1; i < c->argc; i++){
        /*a[i] = (char *)calloc(strlen(c->argv[i])+1, sizeof(char));
        if (a[i] == NULL){
            perror("run_cd");
            return -1;
        }
        else{
            strncpy(a[i], c->argv[i], strlen(c->argv[i]));
        }*/
        a[i] = c->argv[i];
    }
    /*a[i] = (char *)calloc(1, sizeof(char));
    if (a[i] == NULL){
        perror("run_cd");
        return -1;
    }*/
    a[i] = NULL;
    err = execvp(c->argv[0], a);
    return err;
}
