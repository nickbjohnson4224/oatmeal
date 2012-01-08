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

#ifndef __OATMEAL_H
#define __OATMEAL_H

/*****************************************************************************
 * Oatmeal data format
 *
 * Oatmeal processes data in a simple line-oriented texutal format. Its 
 * structure has three levels -- cluster, table, and row -- which map to the
 * relational database concepts of database, table, and row, respectively.
 *
 * A cluster contains a set of tables, each of which has a name within the
 * cluster. No two tables may have the same name. A cluster is a single file,
 * but two cluster files concatenated is a valid cluster file containing their
 * disjoint union, assuming there are no table name collisions.
 *
 * A table contains a schema and a potentially empty set of rows following
 * that schema. It is represented by a sequence of nonempty lines in the
 * cluster file (empty lines are ignored,) preceeded by a line containing the
 * name
 *
 * TODO finish documentation
 */

/* table operations *********************************************************/

struct table {
	int cols;      /* number of columns (schema entries) */
	int rows;      /* number of rows (data entries) */
	char **schema; /* list of column names */
	char ***data;  /* list of data rows following schema */
};

struct table *table_load(FILE *stream);
struct table *table_copy(struct table *t);
struct table *table_proj(struct table *t, char **proj);
int           table_save(FILE *stream, struct table *t);
void          table_free(struct table *t);

struct table *table_append(struct table *t, struct table *t2);
struct table *table_rename(struct table *t, char **schema);

/* cluster operations *******************************************************/

struct cluster {
	int tables;           /* number of tables in cluster */
	struct table **table; /* list of tables in cluster */
	char **name;          /* list of table names in cluster */
};

struct cluster *cluster_load(FILE *stream);
int             cluster_save(FILE *stream, struct cluster *c);
void            cluster_free(struct cluster *c);

/* common I/O operations ****************************************************/

void unload_line(FILE *stream, char **line, int length);
char **load_line(FILE *stream, int *length);
void   save_line(FILE *stream, char **line, int length);
void   free_line(char **line, int length);

void   line_reset(void);

/* script execution *********************************************************/

struct table   *run_script (char **script, struct table *input);
struct cluster *run_cscript(char **script, struct cluster *input);

#endif/*__OATMEAL_H*/
