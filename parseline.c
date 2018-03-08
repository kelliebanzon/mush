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
	int j = 0, arg = 0, err = 0;
	char *curr = c->line, *next = NULL, *direct = NULL;
	/* the ASCII decimal codes of the whitespace characters */
	char whitespace[] = " 	\n\r";
	next = curr+1;
	/* note: c->line+1 is a safe assignment because check_line already
	 * verified that c->line is not empty */
	while (curr != NULL && *curr != '\0'){
		if (arg >= 10){
			return -3;
		}
		if (!isspace(*curr)){
			next = strpbrk(curr, whitespace);
			if (next == NULL){
				strcpy(c->argv[c->argc++], curr); /* TODO: this will probably throw a SIGSEGV */
				return 0; /* TODO: check */
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
				if (next == NULL || *next == '>' || *next == '<'){
					if (*next == '<'){
						return -1;
					}
					else if (*next == '>'){
						return -2;
					}
				}
				else{
					char fname[CMDLINE_LEN];
					strncpy(fname, curr, next-curr);
					if (*direct == '<'){
						err = set_inoutname(c, 0, fname);
						if (err < 0){
							return err;
						}
					}
					else if (*direct == '>'){
						err = set_inoutname(c, 1, fname);
						if (err < 0){
							return err;
						}
					}
					curr = next+1;
					next = NULL;
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

int set_inoutname(cmd *c, int inout, char *fname){
	if (fname == NULL){
		fprintf(stderr, "%s\n", "set_inoutname: given file name is NULL");
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

/*int set_inout(cmd *c, int inout, int where, int type){
	int *c_inout;
	switch(inout){
		case 0:
			c_inout = &(c->input);
			break;
		case 1:
			c_inout = &(c->output);
			break;
		default:
			fprintf(stderr, "%s\n", \
			"set_inout: destination not recognized");
			return -1;
	}
	if (type < 0){
		if (inout == 0){
			c->input = where*(-1);
		}
		else if (inout == 1){
			c->outpu*/

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

int check_redirects(cmd *c){
	if (c->input != STDIN_FILENO && strchr(c->line, '<') != NULL){
		return 1;
	}
	if (c->output != STDOUT_FILENO && strchr(c->line, '>') != NULL){
		return 2;
	}
	if (strcount(c->line, "<") > 1){
		return -1;
	}
	if (strcount(c->line, ">") > 1){
		return -2;
	}
	return 0;
}

int print_cmd(cmd *c){
	printf("\n--------\n");
	printf("Stage %i: \"%s\"\n", c->stage, c->line);
	printf("--------\n");
	printf("%10s: %s\n", "input", "temp TODO");
	printf("%10s: %s\n", "output", "temp TODO");
	printf("%10s: %i\n", "argc", c->argc);
	printf("%10s: %s\n", "argv", "temp TODO");
	return 0;
}
