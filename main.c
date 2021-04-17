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
    if(create_dirs(statement)) return;
    printf("db_name (%d): %s\n", statement->database_name_length, statement->database_name);
    printf("db_path (%d): %s\n", statement->database_path_length, statement->database_path);
    printf("gp_name (%d): %s\n", statement->group_name_length, statement->group_name);
    printf("gp_path (%d): %s\n", statement->group_path_length, statement->group_path);
    printf("data (%d): %s\n", statement->data_length, statement->data);
	switch(statement->type) {
	    case STATEMENT_CREATE:
            printf("[SUCCESS] CREATE\n");
            if(fwrite(statement->data, statement->data_length, 1, statement->group_fp) != 1) {
                printf("[ERROR] Failed to write to binary group file.\n");
                return;
            }
            if(fflush(statement->group_fp) != 0) {
                printf("[ERROR] Failed to flush binary group file buffer.\n");
                return;
            }
	        break;
		case STATEMENT_READ:
            printf("[SUCCESS] READ\n");
            char* buffer = malloc(5);
            if(fread(&buffer, 5, 1, statement->group_fp) != 1) {
                printf("[ERROR] Failed to read from binary group file.\n");
                return;
            }
            printf("read: %s\n", buffer);
			break;
		case STATEMENT_UPDATE:
            printf("[SUCCESS] UPDATE\n");
			break;
        case STATEMENT_DELETE:
            printf("[SUCCESS] DELETE\n");
            break;
	}
}

statement_prepare_result prepare_statement(input_buffer* input, statement_* statement) {
    char* db_name = malloc(33);
    char* gp_name = malloc(33);
    char* cmd_name = malloc(33);
    int data_allocated = 5;
    char* data = malloc(data_allocated);
    if(db_name == NULL || gp_name == NULL || cmd_name == NULL || data == NULL) {
        if(db_name != NULL) free(db_name);
        if(gp_name != NULL) free(gp_name);
        if(cmd_name != NULL) free(cmd_name);
        if(data != NULL) free(data);
        return STATEMENT_PREPARE_FAILED_MALLOC;
    }
    int db_name_i = 0, gp_name_i = 0, cmd_name_i = 0, data_i = 0;
    int sig_db = 0, sig_gp = 0, sig_cmd = 0, sig_data = 0;
    char* c;
    for(c = input->buffer; *c != '\0'; c++) {
        if(sig_db && sig_gp && sig_cmd && !sig_data) {
            if(data_i > data_allocated) {
                data_allocated *= 2;
                data = realloc(data, data_allocated);
                if(data == NULL) return STATEMENT_PREPARE_FAILED_MALLOC;
            }
            if(*c == ')') {
                sig_data = 1;
                data[data_i] = '\0';
                data_i++;
                continue;
            }
            data[data_i] = *c;
            data_i++;
        }
        if(sig_db && sig_gp && !sig_cmd) {
            if(*c == '(') {
                sig_cmd = 1;
                cmd_name[cmd_name_i] = '\0';
                cmd_name_i++;
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
                db_name_i++;
                continue;
            } else if(!sig_gp) {
                sig_gp = 1;
                gp_name[gp_name_i] = '\0';
                gp_name_i++;
                continue;
            } else {
                free(db_name);
                free(gp_name);
                free(cmd_name);
                free(data);
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
    if(!sig_db || !sig_gp || !sig_cmd || !sig_data) {
        free(db_name);
        free(gp_name);
        free(cmd_name);
        free(data);
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
        statement->database_name_length = db_name_i;
        memcpy(statement->database_name, db_name, db_name_i);

        statement->group_name = malloc(gp_name_i);
        statement->group_name_length = gp_name_i;
        memcpy(statement->group_name, gp_name, gp_name_i);

        statement->data = malloc(data_i);
        statement->data_length = data_i;
        memcpy(statement->data, data, data_i);

        free(db_name);
        free(gp_name);
        free(cmd_name);
        free(data);
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

int create_dirs(statement_* statement) {
    if((mkdir("./databases", S_IRWXU | S_IRGRP | S_IROTH)) == -1 && errno != EEXIST) {
        printf("[ERROR] Unable to create './databases' directory.\n");
        return 1;
    }
    char* prefix = "./databases/";
    int prefix_length = 12;
    char* gp_ext = ".rgp";
    int gp_ext_length = 4;

    int db_path_length = prefix_length + (statement->database_name_length - 1) + 1;
    char* db_path = malloc(db_path_length);
    if(db_path == NULL) {
        printf("[ERROR] Unable to allocate memory for database path string (errno = %s).\n", strerror(errno));
        return 1;
    }
    strcpy(db_path, prefix);
    strcat(db_path, statement->database_name);

    int gp_path_length = prefix_length + (statement->database_name_length - 1) + 1 + (statement->group_name_length - 1) + gp_ext_length + 1;
    char* gp_path = malloc(gp_path_length);
    if(gp_path == NULL) {
        printf("[ERROR] Unable to allocate memory for group path string.\n");
        return 1;
    }
    strcpy(gp_path, prefix);
    strcat(gp_path, statement->database_name);
    strcat(gp_path, "/");
    strcat(gp_path, statement->group_name);
    strcat(gp_path, gp_ext);

    if((mkdir(db_path, S_IRWXU | S_IRGRP | S_IROTH)) == -1 && errno != EEXIST) {
        printf("[ERROR] Unable to create '%s' directory (errno = %s).\n", db_path, strerror(errno));
        return 1;
    }

    if((statement->group_fp = fopen(gp_path, "wb")) == NULL) {
        printf("[ERROR] Unable to open group file (%s) in binary mode.\n", gp_path);
        return 1;
    }

    statement->database_path = malloc(db_path_length);
    if(statement->database_path == NULL) {
        printf("[ERROR] Unable to allocate memory for statement database path.\n");
        return 1;
    }
    strcpy(statement->database_path, db_path);
    statement->database_path_length = db_path_length;

    statement->group_path = malloc(gp_path_length);
    if(statement->group_path == NULL) {
        printf("[ERROR] Unable to allocate memory for statement group path.\n");
        return 1;
    }
    strcpy(statement->group_path, gp_path);
    statement->group_path_length = gp_path_length;

    free(db_path);
    free(gp_path);
    return 0;
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