#include "main.h"


int main() {
	input_buffer* input = init_input_buffer();
	for(;;) {
		input_prompt(input);
		if(input->buffer[0] == '.') {
			switch(execute_meta_command(input)) {
				case META_COMMAND_SUCCESS:
					continue;
				case META_COMMAND_UNRECOGNIZED:
					printf("[ERROR] Meta command not recognized.\n");
					continue;
			}
		}

		statement_ stmt;
		switch(prepare_statement(input, &stmt)) {
			case STATEMENT_PREPARE_SUCCESS:
				break;
			case STATEMENT_PREPARE_UNRECOGNIZED:
				printf("[ERROR] Command not recognized.\n");
				continue;
		    case STATEMENT_PREPARE_FAILED_MALLOC:
		        printf("[ERROR] Failed to allocate memory for statement preparation.\n");
                continue;
            case STATEMENT_PREPARE_NAME_TOO_LARGE:
                printf("[ERROR] One of the fields is too long of a string.\n");
                continue;
		}
		execute_statement(&stmt);
	}
	free_input_buffer(input);
	return 0;
}

void execute_statement(statement_* statement) {
    printf("db_name: %s\n", statement->database_name);
    printf("gp_name: %s\n", statement->group_name);
	switch(statement->type) {
	    case STATEMENT_CREATE:
            printf("Logic for create.\n");
	        break;
		case STATEMENT_READ:
			printf("Logic for read.\n");
			break;
		case STATEMENT_UPDATE:
			printf("Logic for update.\n");
			break;
        case STATEMENT_DELETE:
            printf("Logic for delete.\n");
            break;
	}
}

statement_prepare_result prepare_statement(input_buffer* input, statement_* statement) {
    char* db_name = malloc(33);
    char* gp_name = malloc(33);
    char* cmd_name = malloc(33);
    if(db_name == NULL || gp_name == NULL || cmd_name == NULL) {
        free(db_name);
        free(gp_name);
        free(cmd_name);
        return STATEMENT_PREPARE_FAILED_MALLOC;
    }
    int db_name_i = 0, gp_name_i = 0, cmd_name_i = 0;
    int sig_db = 0, sig_gp = 0, sig_cmd = 0;
    char* c;
    for(c = input->buffer; *c != '\0'; c++) {
        if(sig_db && sig_gp && !sig_cmd) {
            if(*c == '(') {
                sig_cmd = 1;
                cmd_name[cmd_name_i] = '\0';
                continue;
            }
            if(cmd_name_i > 32) return STATEMENT_PREPARE_NAME_TOO_LARGE;
            cmd_name[cmd_name_i] = *c;
            cmd_name_i++;
        }
        if(*c == '.') {
            if(!sig_db) {
                sig_db = 1;
                db_name[db_name_i] = '\0';
                continue;
            } else if(!sig_gp) {
                sig_gp = 1;
                gp_name[gp_name_i] = '\0';
                continue;
            } else {
                free(db_name);
                free(gp_name);
                free(cmd_name);
                return STATEMENT_PREPARE_UNRECOGNIZED;
            }
        }
        if(!sig_db) {
            if(db_name_i > 32) return STATEMENT_PREPARE_NAME_TOO_LARGE;
            db_name[db_name_i] = *c;
            db_name_i++;
            continue;
        }
        if(!sig_gp) {
            if(gp_name_i > 32) return STATEMENT_PREPARE_NAME_TOO_LARGE;
            gp_name[gp_name_i] = *c;
            gp_name_i++;
        }
    }
    if(!sig_db || !sig_gp || !sig_cmd) {
        free(db_name);
        free(gp_name);
        free(cmd_name);
        return STATEMENT_PREPARE_UNRECOGNIZED;
    }
    int cmd_valid = 0;
    if(compare(cmd_name, "create", 0)) {
        statement->type = STATEMENT_CREATE;
        cmd_valid = 1;
    } else if(compare(cmd_name, "read", 0)) {
        statement->type = STATEMENT_READ;
        cmd_valid = 1;
    } else if(compare(cmd_name, "update", 0)) {
        statement->type = STATEMENT_UPDATE;
        cmd_valid = 1;
    } else if(compare(cmd_name, "delete", 0)) {
        statement->type = STATEMENT_DELETE;
        cmd_valid = 1;
    }
    if(cmd_valid) {
        statement->database_name = malloc(db_name_i);
        memcpy(statement->database_name, db_name, db_name_i + 1);
        statement->group_name = malloc(gp_name_i);
        memcpy(statement->group_name, gp_name, gp_name_i + 1);
        free(db_name);
        free(gp_name);
        free(cmd_name);
        return STATEMENT_PREPARE_SUCCESS;
    }
    free(db_name);
    free(gp_name);
    free(cmd_name);
    return STATEMENT_PREPARE_UNRECOGNIZED;
}

meta_command_result execute_meta_command(input_buffer* input) {
	if(compare(input->buffer, ".exit", 0)) {
		printf("[SUCCESS] Goodbye!\n");
		free_input_buffer(input);
		exit(EXIT_SUCCESS);
	} else if(compare(input->buffer, ".list", 0)) {
	    //printf("[SUCCESS] Found 1 database(s):\n");
	    //printf("%10s%s\n", " ", "test");
	    return META_COMMAND_SUCCESS;
    } else {
		return META_COMMAND_UNRECOGNIZED;
	}
}

void input_prompt(input_buffer* input) {
	printf("%s > ", APP_NAME);
	ssize_t bytes_read = getline(&(input->buffer), &(input->buffer_length), stdin);
	if(bytes_read < 0) {
		printf("[ERROR] Failed to read input from command line.\n");
		exit(EXIT_FAILURE);
	}
	input->input_length = bytes_read - 1;
	input->buffer[input->input_length] = '\0';
}

input_buffer* init_input_buffer() {
	input_buffer* input = (input_buffer* ) malloc(sizeof(input_buffer));
	if(input == NULL) {
		printf("[ERROR] Unable to allocate memory for input buffer.\n");
		exit(EXIT_FAILURE);
	}
	input->buffer = NULL;
	input->buffer_length = 0;
	input->input_length = 0;
	return input;
}

int compare(char* a, char* b, int n) {
	if(n > 0) return (strncmp(a, b, n) == 0);
	return (strcmp(a, b) == 0);
}

void free_input_buffer(input_buffer* input) {
	free(input->buffer);
	free(input);
}