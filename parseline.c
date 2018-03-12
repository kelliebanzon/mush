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
	c.input = STDIN_FILENO;
	c.output = STDOUT_FILENO;
	c.stage = -1;
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

int parse_args(cmd *c){
	int j = 0, err = 0;
	char *curr = c->line, *next = NULL, *direct = NULL;
	/* the ASCII decimal codes of the whitespace characters */
	char whitespace[] = " 	\n\r";
	next = curr+1;
	/* note: c->line+1 is a safe assignment because check_line already
	 * verified that c->line is not empty */
	while (curr != NULL && *curr != '\0'){
		if (c->argc >= 10){
			return -3;
		}
		if (!isspace(*curr)){
			next = strpbrk(curr, whitespace);
			if (next == NULL){
				if (*curr == '<' || *curr == '>'){
					return bad_redirect(curr);
				}
				else{
					strcpy(c->argv[c->argc++], curr);
					return 0;
				}
			}
			/* note: must check redirection arguments now, instead
			 * of parsing them into the char array and then handling
			 * them, due to the technique of initializing the char
			 * array with the maximum argc value */
			else if (*curr == '>' || *curr == '<'){
				direct = curr;
				curr = next+1;
				next = NULL;
				for (j = 0; j < strlen(curr); j++){
					if (isspace(curr[j])){
						next = curr + j;
						break;
					}
				}
				if (next && (*next == '>' || *next == '<')){
					return bad_redirect(curr);
				}
				else{
					char f[CMDLINE_LEN];
					if (next){
						strncpy(f, curr, next-curr);
					} /* TODO: double check maths */
					else{
						strncpy(f, curr, strlen(curr));
					}
					if (*direct == '<'){
						err = set_inoutname(c, 0, f);
						if (err < 0){
							return err;
						}
					}
					else if (*direct == '>'){
						err = set_inoutname(c, 1, f);
						if (err < 0){
							return err;
						}
					}
					if (next){
						curr = next+1;
						next = NULL;
					}
					else{
						return 0;
					}
				}
			}
			else{
				strncpy(c->argv[c->argc++], curr, next-curr);
				curr = next+1;
				next = NULL;
			}
		}
		else{
			curr++;
		}
	}
	return 0;
}

int bad_redirect(char *c){
	if (*c == '<'){
		return -1;
	}
	else if (*c == '>'){
		return -2;
	}
	else{
		return 0;
	}
}

int set_inoutname(cmd *c, int inout, char *fname){
	if (fname == NULL){
		fprintf(stderr, "%s\n", \
		"set_inoutname: given file name is NULL");
		return -1;
	}
	if (inout == 0){
		if (c->input != STDIN_FILENO){
			return -1;
		}
		else{
			strncpy(c->input_name, fname, strlen(fname));
			c->input = -1;
		}
	}
	else if (inout == 1){
		if (c->output != STDOUT_FILENO){
			return -2;
		}
		else{
			strncpy(c->output_name, fname, strlen(fname));
			c->output = -1;
		}
	}
	else{
		fprintf(stderr, "%s\n", "set_inoutname: inout not recognized");
		return -1;
	}
	return 0;
}

int set_pipes(cmd *c, int num_pipes){
	if (num_pipes == 0){
		return 0;
	}
	else{
		if (c->stage > 0){
			if (c->input_name[0] != '\0'){
				return 1;
			}
			else{
				c->input = c->stage*(-1);
			}
		}
		if (c->stage < num_pipes){
			if (c->output_name[0] != '\0'){
				return 2;
			}
			else{
				c->output = (c->stage+2)*(-1);
			}
		}
		return 0;
	}
}

int print_cmd(cmd *c){
	char buf[CMDLINE_LEN];
	printf("\n--------\n");
	printf("Stage %i: \"%s\"\n", c->stage, c->line);
	printf("--------\n");
	printf("%10s: %s\n", "input", format_inout(c, buf, 0));
	printf("%10s: %s\n", "output", format_inout(c, buf, 1));
	printf("%10s: %i\n", "argc", c->argc);
	printf("%10s: %s\n", "argv", format_argv(c, buf));
	return 0;
}

char *format_inout(cmd *c, char *buf, int type){
	char temp[CMDLINE_LEN] = {'\0'};
	if (type == 0){
		if (c->input_name[0] != '\0'){
			sprintf(temp, "%s", c->input_name);
		}
		else if (c->input == STDIN_FILENO){
			sprintf(temp, "original stdin");
		}
		else if (c->input < 0){
			sprintf(temp, "pipe from stage %i", (c->input*-1)-1);
		}
	}
	else if (type == 1){
		if (c->output_name[0] != '\0'){
			sprintf(temp, "%s", c->output_name);
		}
		else if (c->output == STDOUT_FILENO){
			sprintf(temp, "original stdout");
		}
		else if (c->output < 0){
			sprintf(temp, "pipe to stage %i", (c->output*-1)-1);
		}
	}
	else{
		fprintf(stderr, "format_inout: unrecognized type\n");
		return NULL;
	}
	strncpy(buf, temp, strlen(temp)+1);
	return buf;
}

char *format_argv(cmd *c, char *buf){
	int i = 0;
	char temp[CMDLINE_LEN] = {'\0'};
	for (i = 0; i < c->argc; i++){
		sprintf(temp + strlen(temp), "\"%s\",", c->argv[i]);
	}
	/* strip the trailing comma */
	temp[strlen(temp)-1] = '\0';
	strncpy(buf, temp, strlen(temp)+1);
	return buf;
}
