/*
 * calc2.c
 *
 * Copyright 2024 lahiru <lahiru@lahiru-Lenovo-V15-G2-ITL>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "calc_core.h"

#include <stdio.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include <string.h>

#define NUM_BUTTONS  10
#define NUM_NAME_LENGTH  40

#define OPP_ADD 1
#define OPP_DIV 2
#define OPP_SUB 3
#define OPP_MUL 4
#define OPP_UND 5

#define DIS_CLR 1
#define DIS_KEP 2

typedef struct {
	GObject *textBox;
	double opOne;
	int opp;
	int dis;
	CalcPhase phase;
} CalculatorState;

static CalculatorState calcState;

static void remove_first_char(char *str)
{
	size_t len = strlen(str);
	if (len == 0) {
		return;
	}
	memmove(str, str + 1, len - 1);
	str[len - 1] = '\0';
}

static bool is_empty(const char* str)
{
	return *str == '\0';
}

static bool has_period(const char* str)
{
	return strchr(str, '.') != NULL;
}

static bool has_negative(const char* str)
{
	return strchr(str, '-') != NULL;
}

static void set_entry_text(const char *text)
{
	gtk_entry_set_text(GTK_ENTRY(calcState.textBox), text);
}

static const gchar *get_entry_text(void)
{
	return gtk_entry_get_text(GTK_ENTRY(calcState.textBox));
}

static void clear_state(void)
{
	set_entry_text("");
	calcState.opOne = 0.0;
	calcState.opp = OPP_UND;
	calcState.phase = CALC_NEED_LHS;
	calcState.dis = DIS_KEP;
}

static void on_clear_all(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	clear_state();
}

static void print_number(GtkWidget *widget, gpointer data)
{
	(void)data;

	if (calcState.dis == DIS_CLR) {
		if (calcState.phase == CALC_NEED_LHS) {
			calcState.opOne = 0.0;
			calcState.opp = OPP_UND;
		}
		set_entry_text("");
		calcState.dis = DIS_KEP;
	}
	{
		const gchar *label_text = gtk_button_get_label(GTK_BUTTON(widget));
		const gchar *current_text = get_entry_text();
		gchar *new_text = g_strdup_printf("%s%s", current_text, label_text);

		set_entry_text(new_text);
		g_free(new_text);
	}
}

static void add_dot(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	{
		const char *current_text = get_entry_text();
		gchar *new_text = NULL;

		if (!has_period(current_text)) {
			if (is_empty(current_text)) {
				new_text = g_strdup_printf("0.");
			} else {
				new_text = g_strdup_printf("%s.", current_text);
			}
			set_entry_text(new_text);
			g_free(new_text);
		}
	}
}

static void toggle_sign(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	{
		const char *current_text = get_entry_text();
		gchar *new_text = NULL;

		if (!is_empty(current_text)) {
			if (has_negative(current_text)) {
				new_text = g_strdup(current_text);
				remove_first_char(new_text);
			} else {
				new_text = g_strdup_printf("-%s", current_text);
			}

			set_entry_text(new_text);
			g_free(new_text);
		}
	}
}

static void do_operation(GtkWidget *widget, gpointer data, int operation, bool is_equal)
{
	(void)widget;
	(void)data;
	{
		const char *current_text = get_entry_text();
		double rhs;

		calcState.opp = operation;
		if (calcState.phase == CALC_NEED_LHS) {
			if (calc_parse_entry(current_text, &calcState.opOne) != 0) {
				set_entry_text("ERROR");
				return;
			}
			calcState.phase = CALC_NEED_RHS;
			set_entry_text("");
		} else {
			if (calc_parse_entry(current_text, &rhs) != 0) {
				set_entry_text("ERROR");
				return;
			}
			switch (operation) {
			case OPP_ADD:
				calcState.opOne += rhs;
				break;
			case OPP_SUB:
				calcState.opOne -= rhs;
				break;
			case OPP_MUL:
				calcState.opOne *= rhs;
				break;
			case OPP_DIV:
				if (rhs != 0.0) {
					calcState.opOne /= rhs;
				} else {
					set_entry_text("ERROR");
					return;
				}
				break;
			default:
				break;
			}
			{
				gchar *new_text = g_strdup_printf("%.2lf", calcState.opOne);
				set_entry_text(new_text);
				g_free(new_text);
			}
		}
		calcState.dis = DIS_CLR;
	}
	if (is_equal) {
		calcState.phase = CALC_NEED_LHS;
	}
}

static void do_binary_op(int operation)
{
	do_operation(NULL, NULL, operation, false);
}

static void do_add(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	do_binary_op(OPP_ADD);
}

static void do_sub(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	do_binary_op(OPP_SUB);
}

static void do_mul(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	do_binary_op(OPP_MUL);
}

static void do_div(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	do_binary_op(OPP_DIV);
}

static void do_equal(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	do_operation(NULL, NULL, calcState.opp, true);
}

static void init_ui(int argc, char *argv[])
{
	GtkBuilder *builder;
	GObject *window;
	GObject *button;
	GError *error = NULL;
	int i;

	calcState.opOne = 0.0;
	calcState.opp = OPP_UND;
	calcState.phase = CALC_NEED_LHS;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();

	if (gtk_builder_add_from_file(builder, "builder.ui", &error) == 0) {
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		g_object_unref(builder);
		return;
	}

	window = gtk_builder_get_object(builder, "window1");
	if (window == NULL) {
		g_printerr("Missing object: window1\n");
		g_object_unref(builder);
		return;
	}
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	calcState.textBox = gtk_builder_get_object(builder, "textDisplay");
	if (calcState.textBox == NULL) {
		g_printerr("Missing object: textDisplay\n");
		g_object_unref(builder);
		return;
	}

	for (i = 0; i < NUM_BUTTONS; i++) {
		char btnName[NUM_NAME_LENGTH];
		g_snprintf(btnName, sizeof(btnName), "btn%d", i);
		button = gtk_builder_get_object(builder, btnName);
		if (button == NULL) {
			g_printerr("Missing object: %s\n", btnName);
			g_object_unref(builder);
			return;
		}
		g_signal_connect(button, "clicked", G_CALLBACK(print_number), NULL);
	}

	button = gtk_builder_get_object(builder, "btnac");
	if (button == NULL) { g_printerr("Missing: btnac\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(on_clear_all), NULL);

	button = gtk_builder_get_object(builder, "btndot");
	if (button == NULL) { g_printerr("Missing: btndot\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(add_dot), NULL);

	button = gtk_builder_get_object(builder, "btnsign");
	if (button == NULL) { g_printerr("Missing: btnsign\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(toggle_sign), NULL);

	button = gtk_builder_get_object(builder, "btneql");
	if (button == NULL) { g_printerr("Missing: btneql\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(do_equal), NULL);

	button = gtk_builder_get_object(builder, "btnsum");
	if (button == NULL) { g_printerr("Missing: btnsum\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(do_add), NULL);

	button = gtk_builder_get_object(builder, "btnmin");
	if (button == NULL) { g_printerr("Missing: btnmin\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(do_sub), NULL);

	button = gtk_builder_get_object(builder, "btnmul");
	if (button == NULL) { g_printerr("Missing: btnmul\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(do_mul), NULL);

	button = gtk_builder_get_object(builder, "btndiv");
	if (button == NULL) { g_printerr("Missing: btndiv\n"); g_object_unref(builder); return; }
	g_signal_connect(button, "clicked", G_CALLBACK(do_div), NULL);

	g_object_unref(builder);
	gtk_main();
}

int main(int argc, char *argv[])
{
	init_ui(argc, argv);
	return 0;
}
