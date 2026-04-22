#include "calc_core.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>

int
calc_parse_entry(const char *str, double *out)
{
	char *end;
	double v;

	if (!out) {
		return -1;
	}
	if (!str) {
		return -1;
	}
	while (*str && isspace((unsigned char) *str)) {
		str++;
	}
	if (*str == '\0') {
		*out = 0.0;
		return 0;
	}
	errno = 0;
	v = strtod(str, &end);
	if (end == str) {
		return -1;
	}
	if (errno == ERANGE) {
		return -1;
	}
	if (!isfinite(v)) {
		return -1;
	}
	while (*end) {
		if (!isspace((unsigned char) *end)) {
			return -1;
		}
		end++;
	}
	*out = v;
	return 0;
}
