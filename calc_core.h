/* Shared calculator state and parsing for calc.c / calc2.c / calc_sdl.c */

#ifndef CALC_CORE_H
#define CALC_CORE_H

typedef enum {
	/* After AC or =; next op reads display as the left-hand operand, or
	 * the user is about to type a new number (see print_number reset). */
	CALC_NEED_LHS = 0,
	/* A left-hand value is in opOne; the display is the right operand (or
	 * will be, after the user types). */
	CALC_NEED_RHS
} CalcPhase;

/* Parse a single calculator entry. Rejects non-numeric garbage, strtod
 * over/underflow (ERANGE), and non-finite results. */
int calc_parse_entry(const char *str, double *out);

#endif
