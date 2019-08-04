/* SPDX-License-Identifier: LGPL-3.0-or-later */

#include <reg.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* reg_gperf.h is generated by gperf */
#include <reg_gperf.h>

static char* x_strchr(const char *s, int c) {
	char* n = NULL;

	if ((n = strchr(s, c))) {
		*(n++) = '\0';
		return n;
	}

	return n;
}

static int reg_from_pair(struct reg* reg, char* key, unsigned long int value) {
	assert(value < 0xFFFF);

	/* in_word_set() is generated by gperf */
	struct reg_decode* rd = in_word_set(key, strlen(key));
	if (!rd) {
		return -1;
	}

	reg->addr[rd->reg_name] = value;

	return 0;
}

static int reg_from_line(struct reg* reg, char* line) {
	const char sep[] = " ";
	char* key = line;
	char* value = NULL;

	if (line[0] == '\0' || line[0] == '#')
		return 0;

	if (!(value = strchr(line, '=')))
		return -1;
	*(value++) = '\0';

	key = strtok(key, sep);
	assert(key);

	reg_from_pair(reg, key, strtoul(value, NULL, 16));

	return 0;
}

static int reg_validate(struct reg* reg) {
	size_t i = 0;

	for (i = 0; i < REG_MAX; ++i) {
		if (reg->addr[i] == 0xffff) {
			reg->error_str = "Missed register address";
			return -1;
		}
	}

	return 0;
}

static int reg_init_from_map(struct reg* reg, char* map) {
	char* c = map;
	char* n = NULL;

	do {
		n = x_strchr(c, '\n');
		if (reg_from_line(reg, c)) {
			reg->error_str = "Can not parse register map";

			return -1;
		}
		c = n;
	} while (n);

	return reg_validate(reg);
}

int reg_init(struct reg* reg, char* map) {
	memset(reg, 0xff, sizeof(struct reg));
	reg->error_str = NULL;

	return reg_init_from_map(reg, map);
}
