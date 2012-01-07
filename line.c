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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "oatmeal.h"

struct line_revbuf {
	struct line_revbuf *next;
	char **line;
	int length;
};

struct line_revbuf_list {
	FILE *file;
	struct line_revbuf *revbuf;
	struct line_revbuf_list *next;
};

static struct line_revbuf_list *_list = NULL;

void unload_line(FILE *stream, char **line, int length) {
	struct line_revbuf_list *l;
	struct line_revbuf *r;

	for (l = _list; l; l = l->next) {
		if (l->file == stream) {
			break;
		}
	}

	if (!l) {
		l = malloc(sizeof(struct line_revbuf_list));
		l->file = stream;
		l->revbuf = NULL;
		l->next = _list;
		_list = l;
	}

	r = malloc(sizeof(struct line_revbuf));
	r->next = l->revbuf;
	r->line = line;
	r->length = length;
	l->revbuf = r;
}

char **load_line(FILE *stream, int *length) {
	struct line_revbuf_list *l;
	struct line_revbuf *r;
	int linestr_length;
	int linestr_alloc;
	char *linestr;
	char **line;
	char *saveptr;
	char *elem;
	int c;
	int i;
	int tabcount;

	for (l = _list; l; l = l->next) {
		if (l->file == stream) {
			break;
		}
	}

	if (l && l->revbuf) {
		r = l->revbuf;
		l->revbuf = l->revbuf->next;
		*length = r->length;
		line = r->line;
		free(r);
		return line;
	}

	if (feof(stream)) {
		return NULL;
	}

	tabcount = 0;
	linestr_length = 0;
	linestr_alloc  = 16;
	linestr = malloc(sizeof(char) * linestr_alloc);
	c = getc(stream);

	if (c == EOF) {
		return NULL;
	}
	while (c == '\n') {
		c = getc(stream);
		if (c == EOF) return NULL;
	}
		
	while (c != '\n' && c != EOF) {
		linestr[linestr_length++] = (char) c;

		if (c == '\t') tabcount++;

		if (linestr_length >= linestr_alloc) {
			linestr_alloc *= 2;
			linestr = realloc(linestr, sizeof(char) * linestr_alloc);
		}

		c = getc(stream);
	}
	linestr[linestr_length] = '\0';

	line = malloc(sizeof(char*) * (tabcount + 1));
	line[0] = strdup(strtok_r(linestr, "\t", &saveptr));
	for (i = 1;; i++) {
		elem = strtok_r(NULL, "\t", &saveptr);
		if (!elem) break;
		line[i] = strdup(elem);
	}
	*length = i;

	free(linestr);

	return line;
}

void save_line(FILE *stream, char **line, int length) {
	int i;

	for (i = 0; i < length - 1; i++) {
		printf("%s\t", line[i]);
	}
	printf("%s\n", line[length - 1]);
}

void free_line(char **line, int length) {
	int i;

	for (i = 0; i < length; i++) {
		free(line[i]);
	}

	free(line);
}
