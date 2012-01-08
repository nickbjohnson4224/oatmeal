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

struct cluster *cluster_load(FILE *stream) {
	struct cluster *cluster;
	struct table *table;
	char *name;
	char **line;
	int length;

	/* allocate cluster structure */
	cluster = malloc(sizeof(struct cluster));
	cluster->tables = 0;
	cluster->table  = NULL;
	cluster->name   = NULL;

	while (1) {

		/* read potential header line */
		line = load_line(stream, &length);

		if (!line) {
			return cluster;
		}

		if (!strcmp(line[0], "###")) {
			/* table header line */

			if (length != 2) {
				/* incorrect header line format */
				free_line(line, length);
				return NULL;
			}

			name  = line[1];
			table = table_load(stream);

			free(line[0]);
			free(line);

			if (!table) {
				/* invalid table format */
				return NULL;
			}

			cluster->tables++;
			cluster->table = realloc(cluster->table, sizeof(void*) * cluster->tables);
			cluster->name  = realloc(cluster->name,  sizeof(void*) * cluster->tables);
			cluster->table[cluster->tables-1] = table;
			cluster->name[cluster->tables-1] = name;
		}
		else {
		
			unload_line(stream, line, length);

			name  = strdup("table");
			table = table_load(stream);

			if (!table) {
				/* invalid table format */
				return NULL;
			}

			cluster->tables = 1;
			cluster->table = malloc(sizeof(void*));
			cluster->name  = malloc(sizeof(void*));
			cluster->table[0] = table;
			cluster->name[0] = name;
		}

	}
}

int cluster_save(FILE *stream, struct cluster *c) {
	struct table *table;
	size_t i, j;

	for (i = 0; i < c->tables; i++) {
		fprintf(stream, "###\t%s\n", c->name[i]);
		table_save(stream, c->table[i]);
	}

	return 0;
}

void cluster_free(struct cluster *c) {
	size_t i;

	if (!c) return;

	for (i = 0; i < c->tables; i++) {
		table_free(c->table[i]);
		free(c->name[i]);
	}

	free(c->table);
	free(c->name);
	free(c);
}
