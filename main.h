#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define APP_NAME "rocket"

typedef enum {
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED
} meta_command_result;

typedef enum {
	STATEMENT_PREPARE_SUCCESS,
	STATEMENT_PREPARE_UNRECOGNIZED,
	STATEMENT_INVALID_DATABASE
} statement_prepare_result;

typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT
} statement_type;

typedef enum {
	DATABASE_CONNECTION_VALID,
	DATABASE_CONNECTION_INVALID
} database_connection_status;

typedef struct {
	char* buffer;
	size_t buffer_length;
	ssize_t input_length;
} input_buffer;

typedef struct {
	char* name;
	char* file_name;
	FILE* file;
	database_connection_status status;
} database_connection_;

typedef struct {
	statement_type type;
} statement_;

input_buffer* init_input_buffer();
void input_prompt(input_buffer* input);
void free_input_buffer(input_buffer* input);

meta_command_result execute_meta_command(input_buffer* input);
statement_prepare_result prepare_statement(database_connection_* database_connection, input_buffer* input, statement_* statement);
void execute_statement(statement_* statement);

int compare(char* a, char* b, int n);

#endif