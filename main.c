#include "main.h"


int main() {
	database_connection_ database_connection;
	database_connection.status = DATABASE_CONNECTION_INVALID;
	input_buffer* input = init_input_buffer();
	for(;;) {
		input_prompt(input);
		if(input->buffer[0] == '.') {
			switch(execute_meta_command(input)) {
				case META_COMMAND_SUCCESS:
					printf("meta success!\n");
					continue;
				case META_COMMAND_UNRECOGNIZED:
					printf("[ERROR] Command not recognized.\n");
					continue;
			}
		}

		statement_ stmt;
		switch(prepare_statement(&database_connection, input, &stmt)) {
			case STATEMENT_PREPARE_SUCCESS:
				break;
			case STATEMENT_PREPARE_UNRECOGNIZED:
				printf("[ERROR] Command not recognized.\n");
				continue;
			case STATEMENT_INVALID_DATABASE:
				printf("[ERROR] No database selected.\n");
				continue;
		}
		execute_statement(&stmt);
	}
	free_input_buffer(input);
	return 0;
}

void execute_statement(statement_* statement) {
	switch(statement->type) {
		case STATEMENT_INSERT:
			printf("Logic for insert.\n");
			break;
		case STATEMENT_SELECT:
			printf("Logic for select.\n");
			break;
	}
}

statement_prepare_result prepare_statement(database_connection_* database_connection, input_buffer* input, statement_* statement) {
	if(database_connection->status != DATABASE_CONNECTION_VALID) return STATEMENT_INVALID_DATABASE;
	if(compare(input->buffer, "INSERT", 6)) {
		statement->type = STATEMENT_INSERT;
		return STATEMENT_PREPARE_SUCCESS;
	}
	if(compare(input->buffer, "SELECT", 6)) {
		statement->type = STATEMENT_SELECT;
		return STATEMENT_PREPARE_SUCCESS;
	}
	return STATEMENT_PREPARE_UNRECOGNIZED;
}

meta_command_result execute_meta_command(input_buffer* input) {
	if(compare(input->buffer, ".exit", 0)) {
		printf("goodbye!\n");
		free_input_buffer(input);
		exit(EXIT_SUCCESS);
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