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

#include <unistd.h>

#include "oatmeal.h"

struct table *run_script(char **script, struct table *input) {
	struct table *output;
	char buffer[100];
	int ipipe[2];
	int opipe[2];
	pid_t child;
	FILE *instream, *outstream;

	if (pipe(ipipe) < 0 || pipe(opipe) < 0) {
		fprintf(stderr, "pipe failed");
		return NULL;
	}

	child = fork();
	if (child) {
		/* parent process */
		close(ipipe[0]);
		close(opipe[1]);

		instream = fdopen(ipipe[1], "w");
		table_save(instream, input);
		fclose(instream);

		outstream = fdopen(opipe[0], "r");
		output = table_load(outstream);
		fclose(outstream);

		return output;
	}
	else {
		/* child process */
		close(ipipe[1]);
		close(opipe[0]);

		dup2(ipipe[0], 0);
		dup2(opipe[1], 1);

		close(ipipe[0]);
		close(opipe[1]);

		if (script[0][0] == '/') {
			execvp(script[0], script);
		}
		else {
			strncpy(buffer, "./", 100);
			strncat(buffer, script[0], 100);
			execvp(buffer, script);
		}

		return NULL;
	}
}

struct cluster *run_cscript(char **script, struct cluster *input);
