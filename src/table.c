/*
 * Copyright (C) 2012 Nick Johnson <nickbjohnson4224 at gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "oatmeal.h"

struct table *table_load(FILE *stream) {
	struct table *table;
	char **line;
	int length;
	
	/* allocate table structure */
	table = malloc(sizeof(struct table));
	table->schema = NULL;
	table->data   = NULL;
	table->rows   = 0;

	/* read schema line */
	line = load_line(stream, &length);

	if (!line) {
		free(table);
		return NULL;
	}

	table->schema = line;
	table->cols = length;

	while (1) {
		
		/* read row */
		line = load_line(stream, &length);
		
		if (!line) {
			break;
		}

		if (!strcmp(line[0], "###")) {
			unload_line(stream, line, length);
			break;
		}

		table->rows++;
		table->data = realloc(table->data, sizeof(char**) * table->rows);
		table->data[table->rows-1] = line;
	}

	return table;
}

struct table *table_proj(struct table *table, char **proj) {
	struct table *table1;
	int *projmap;
	int length;
	int i, j;

	for (length = 0; proj[length]; length++);
	projmap = malloc(sizeof(int) * length);

	for (i = 0; i < length; i++) {
		projmap[i] = -1;
		for (j = 0; j < table->cols; j++) {
			if (!strcmp(proj[i], table->schema[j])) {
				projmap[i] = j;
			}
		}
		
		if (projmap[i] == -1) {
			fprintf(stderr, "error: projection: column \"%s\" not found\n", proj[i]);
			return NULL;
		}
	}

	table1 = malloc(sizeof(struct table));
	table1->rows = table->rows;
	table1->cols = length;
	table1->schema = malloc(sizeof(char*) * length);
	table1->data = malloc(sizeof(char**) * table->rows);
	
	for (i = 0; i < length; i++) {
		table1->schema[i] = strdup(proj[i]);
	}

	for (i = 0; i < table->rows; i++) {
		table1->data[i] = malloc(sizeof(char*) * length);
		for (j = 0; j < length; j++) {
			table1->data[i][j] = strdup(table->data[i][projmap[j]]);
		}
	}

	free(projmap);
	return table1;
}

struct table *table_rename(struct table *table, char **schema) {
	struct table *table1;
	int length;
	int i;

	for (length = 0; schema[length]; length++);

	table1 = table_copy(table);

	for (i = 0; i < length && i < table1->cols; i++) {
		free(table1->schema[i]);
		table1->schema[i] = strdup(schema[i]);
	}

	return table1;
}

int table_save(FILE *stream, struct table *table) {
	int i;

	save_line(stream, table->schema, table->cols);

	for (i = 0; i < table->rows; i++) {
		save_line(stream, table->data[i], table->cols);
	}

	return 0;
}

void table_free(struct table *table) {
	int i;

	free_line(table->schema, table->cols);

	for (i = 0; i < table->rows; i++) {
		free_line(table->data[i], table->cols);
	}

	free(table->data);
	free(table);
}

struct table *table_copy(struct table *table) {
	struct table *table1;
	int i, j;

	table1 = malloc(sizeof(struct table));
	table1->rows = table->rows;
	table1->cols = table->cols;
	table1->schema = malloc(sizeof(char*) * table1->cols);
	table1->data = malloc(sizeof(char**) * table1->rows);

	for (i = 0; i < table->cols; i++) {
		table1->schema[i] = strdup(table->schema[i]);
	}

	for (i = 0; i < table->rows; i++) {
		table1->data[i] = malloc(sizeof(char*) * table->cols);
		for (j = 0; j < table->cols; j++) {
			table1->data[i][j] = strdup(table->data[i][j]);
		}
	}

	return table1;
}

struct table *table_append(struct table *table1, struct table *table2) {
	struct table *table3;
	int i, j;

	if (table1->rows != table2->rows) {
		fprintf(stderr, "error: append: row dimension mismatch\n");
		return NULL;
	}

	table3 = malloc(sizeof(struct table));
	table3->rows = table1->rows;
	table3->cols = table1->cols + table2->cols;
	table3->schema = malloc(sizeof(char*) * table3->cols);
	table3->data = malloc(sizeof(char**) * table3->rows);

	for (i = 0; i < table1->cols; i++) {
		table3->schema[i] = strdup(table1->schema[i]);
	}
	for (i = 0; i < table2->cols; i++) {
		table3->schema[i + table1->cols] = strdup(table2->schema[i]);
	}

	for (i = 0; i < table3->rows; i++) {
		table3->data[i] = malloc(sizeof(char*) * table3->cols);

		for (j = 0; j < table1->cols; j++) {
			table3->data[i][j] = strdup(table1->data[i][j]);
		}
		for (j = 0; j < table2->cols; j++) {
			table3->data[i][j + table1->cols] = strdup(table2->data[i][j]);
		}
	}

	return table3;
}
