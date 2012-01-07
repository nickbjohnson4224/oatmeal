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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>

#include "oatmeal.h"

static bool matches_list(char **list, const char *string);

/*****************************************************************************
 * oatmeal - relational data processing pipeline
 *
 * oat SCRIPT
 * oat -b FILE
 * oat [OPTIONS] -e SCRIPT
 *
 * OPTIONS
 *
 * -a
 *   Append script output to source table before output. Script output and
 *   source table must have the same number of rows.
 *
 * -t TABLE ...
 *   Only apply operation to the given list of tables in the cluster.
 *
 * -j [COUNT]
 *   Run up to COUNT parallel subprocesses (this does not cause splitting
 *   unless explicitly specified.)
 *
 * -c
 *   Give set of tables to the script in cluster format.
 *
 * -p COLUMN ...
 *   Project only the given list of columns to the script.
 *
 * -w FILE
 *   Write a copy of the output cluster to FILE.
 *
 * -r COLUMN ...
 *   Rename the columns of the script output schema to the given list of
 *   columns. The number of given columns must match the number of columns in
 *   the script output schema.
 *
 * -o COLUMN ...
 *   Project only the given list of columns in the output.
 */

int main(int argc, char **argv) {

	/* data */
	struct cluster *cluster = NULL; /* cluster being processed */
	struct table *source    = NULL; /* source table in cluster */
	struct table *output    = NULL; /* output table from script */
	struct table *saved     = NULL; /* saved source table in cluster */

	/* append argument */
	bool a_flag = false;

	/* table select argument */
	bool t_flag = false;
	char **table_list = NULL;
	
	/* script execute argument */
	bool e_flag = false;
	char **script_argv = NULL;

	/* project argument */
	bool p_flag = false;
	char **proj_schema = NULL;

	/* output project argument */
	bool o_flag = false;
	char **oproj_schema = NULL;

	size_t i, j;

	/* parse command line arguments */
	if (argc < 2) {
		fprintf(stderr, "%s: script argument required\n", argv[0]);
		return 1;
	}

	if (argv[1][0] != '-') {
		/* simple script format (no options) */
		e_flag = true;
		script_argv = &argv[1];
	}
	else for (i = 1; i < argc;) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'a': /* append option */
				a_flag = true;

				i += 1;
				break;
				
			case 't': /* table option */
				t_flag = true;

				/* allocate table list */
				for (j = 0; argv[i+j+1] && argv[i+j+1][0] != '-'; j++);
				table_list = malloc(sizeof(char*) * (j + 1));

				/* create table list */
				for (j = 0; argv[i+j+1] && argv[i+j+1][0] != '-'; j++) {
					table_list[j] = argv[i+j+1];
				}
				table_list[j] = NULL;
				
				i += j + 1;
				break;

			case 'p': /* project option */
				p_flag = true;

				/* allocate projection schema */
				for (j = 0; argv[i+j+1] && argv[i+j+1][0] != '-'; j++);
				proj_schema = malloc(sizeof(char*) * (j + 1));

				/* create table list */
				for (j = 0; argv[i+j+1] && argv[i+j+1][0] != '-'; j++) {
					proj_schema[j] = argv[i+j+1];
				}
				proj_schema[j] = NULL;
				
				i += j + 1;
				break;

			case 'o': /* output project option */
				o_flag = true;

				/* allocate projection schema */
				for (j = 0; argv[i+j+1] && argv[i+j+1][0] != '-'; j++);
				oproj_schema = malloc(sizeof(char*) * (j + 1));

				/* create table list */
				for (j = 0; argv[i+j+1] && argv[i+j+1][0] != '-'; j++) {
					oproj_schema[j] = argv[i+j+1];
				}
				oproj_schema[j] = NULL;
				
				i += j + 1;
				break;

			case 'e': /* script execute option */
				e_flag = true;
				script_argv = &argv[i+1];

				i = argc;
				break;

			default: /* unknown option */
				fprintf(stderr, "%s: unhandled option %s\n", argv[0], argv[i]);
				return 1;
			}
		}
	}

	/* read cluster from stdin */
	cluster = cluster_load(stdin);

	if (!cluster) {
		fprintf(stderr, "%s: cluster load error from standard input\n", argv[0]);
		return 1;
	}

	/* iterate over requested tables */
	for (i = 0; i < cluster->tables; i++) {
		if (!t_flag || matches_list(table_list, cluster->name[i])) {

			/* get source table */
			source = cluster->table[i];
			output = NULL;
			
			/* prepare for append */
			if (a_flag) {
				saved = table_copy(source);
			}

			/* apply input projection */
			if (p_flag) {
				output = table_proj(source, proj_schema);
				table_free(source);
				source = output;
			}

			/* TODO */
			/* apply script */

			/* apply append */
			if (a_flag) {
				output = table_append(saved, source);
				table_free(saved);
				table_free(source);
				source = output;
			}

			/* apply output projection */
			if (o_flag) {
				output = table_proj(source, oproj_schema);
				table_free(source);
				source = output;
			}
			
			if (!output) {
				output = source;
			}

			/* save output table */
			cluster->table[i] = output;
		}
	}

	/* write cluster to stdout */
	cluster_save(stdout, cluster);
}

static bool matches_list(char **list, const char *string) {
	size_t i;
	
	for (i = 0; list[i]; i++) {
		if (!strcmp(list[i], string)) {
			return true;
		}
	}

	return false;
}
