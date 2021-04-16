#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define APP_NAME "rocketdb"

typedef enum {
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED
} meta_command_result;

typedef enum {
	STATEMENT_PREPARE_SUCCESS,
    STATEMENT_PREPARE_UNRECOGNIZED,
    STATEMENT_PREPARE_FAILED_MALLOC,
    STATEMENT_PREPARE_NAME_TOO_LARGE
} statement_prepare_result;

typedef enum {
	STATEMENT_CREATE,
	STATEMENT_READ,
	STATEMENT_UPDATE,
	STATEMENT_DELETE
} statement_type;

typedef struct {
	char* buffer;
	size_t buffer_length;
	ssize_t input_length;
} input_buffer;

typedef struct {
	statement_type type;
	char* database_name;
	char* group_name;
} statement_;

input_buffer* init_input_buffer();
void input_prompt(input_buffer* input);
void free_input_buffer(input_buffer* input);

meta_command_result execute_meta_command(input_buffer* input);
statement_prepare_result prepare_statement(input_buffer* input, statement_* statement);
void execute_statement(statement_* statement);

int compare(char* a, char* b, int n);

#endif