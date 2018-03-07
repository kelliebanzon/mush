#include "parseline.h"

int char_index(char *haystack, char *needle){
	char *c = strstr(haystack, needle);
	if (c == NULL){
		return -1;
	}
	else{
		return c-haystack;
	}
}

int strcount(char *haystack, char *needle){
	int count = 0;
	char *cp = NULL;
	cp = haystack-1;
	while ((cp = strstr(cp+1, needle)) != NULL){
		count++;
	}
	return count;
}

cmd empty_cmd(){
	cmd c = {0};
	/*int i = 0;
	c.line = NULL;
	c.argc = 0;
	for (; i < CMDARGS_LEN; i++){
		c.argv[i] = NULL;
	}*/
	c.input = STDIN_FILENO;
	c.output = STDOUT_FILENO;
	c.stage = -1;
	/*c.next = NULL;*/
	return c;
}

int check_line(char *l){
	int i = 0, len = strlen(l);
	for (; i < len; i++){
		if (isgraph(l[i])){
			return 0;
		}
	}
	return -1;
}

int set_pipes(cmd *c, int num_pipes){
	if (num_pipes == 0){
		return 0;
	}
	else{
		if (c->stage > 0){
			c->input = c->stage*(-1);
		}
		if (c->stage < num_pipes){
			c->output = (c->stage+2)*(-1);
		}
		return 0;
	}
}

int check_redirects(cmd *c){
	if (c->input != STDIN_FILENO && strchr(c->line, '<') != NULL){
		return -1;
	}
	if (c->output != STDOUT_FILENO && strchr(c->line, '>') != NULL){
		return -2;
	}
	if (strcount(c->line, "<") > 1){
		return -1;
	}
	if (strcount(c->line, ">") > 1){
		return -2;
	}
	return 0;
}
