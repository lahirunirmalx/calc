/*
 * calc.c
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
 * 
 * 
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

GObject *textBox;
static double opOne;
static int opp;
static int dis;
static CalcPhase phase;


static void
remove_first_char(char *str)
{
	int len = (int)strlen(str);
	if (len > 0) {
		memmove(str, str + 1, (size_t)(len - 1));
		str[len - 1] = '\0';
	}
}

static bool is_empty(const char* str) {
  return *str == '\0';
}

static bool has_period(const char* str) {
  while (*str != '\0') {  
    if (*str == '.') { 
      return true;
    }
    str++;
  } 
  return false;
}

static bool has_negative(const char* str) {
  while (*str != '\0') {  
    if (*str == '-') { 
      return true;
    }
    str++;
  } 
  return false;
}

static void print_number(GtkWidget *widget, gpointer data)
{
	(void)data;

	if (dis == DIS_CLR) {
		/* new digit after a finished result: drop stale acc/op, see review */
		if (phase == CALC_NEED_LHS) {
			opOne = 0.0;
			opp = OPP_UND;
		}
		gtk_entry_set_text(GTK_ENTRY(textBox), "");
		dis = DIS_KEP;
	}
	{
		GtkButton *gtk_button = GTK_BUTTON(widget);
		const gchar *label_text = gtk_button_get_label(gtk_button);
		const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(textBox));
		gchar *new_text = g_strdup_printf("%s%s", current_text, label_text);

		gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
		g_free(new_text);
	}
}
static void clear_display(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	gtk_entry_set_text(GTK_ENTRY(textBox), "");
	opOne = 0.0;
	opp = OPP_UND;
	phase = CALC_NEED_LHS;
	dis = DIS_KEP;
}


	
static void add_dot(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	{
		const char *current_text = gtk_entry_get_text(GTK_ENTRY(textBox));
		gchar *new_text;

		if (!has_period(current_text)) {
			if (is_empty(current_text)) {
				new_text = g_strdup_printf("%s%s", "0", ".");
			} else {
				new_text = g_strdup_printf("%s%s", current_text, ".");
			}
			gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
			g_free(new_text);
		}
	}
}

static void togal_sign(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	{
		const char *current_text = gtk_entry_get_text(GTK_ENTRY(textBox));
		gchar *new_text;

		if (!is_empty(current_text)) {
			if (has_negative(current_text)) {
				new_text = g_strdup_printf("%s", current_text);
				remove_first_char(new_text);
			} else {
				new_text = g_strdup_printf("%s%s", "-", current_text);
			}
			gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
			g_free(new_text);
		}
	}
}

static void do_operation(GtkWidget *widget, gpointer data, int operation, bool is_equal)
{
	(void)widget;
	(void)data;
	{
		const char *current_text = gtk_entry_get_text(GTK_ENTRY(textBox));
		double rhs;

		opp = operation;
		if (phase == CALC_NEED_LHS) {
			if (calc_parse_entry(current_text, &opOne) != 0) {
				gtk_entry_set_text(GTK_ENTRY(textBox), "ERROR");
				return;
			}
			phase = CALC_NEED_RHS;
			gtk_entry_set_text(GTK_ENTRY(textBox), "");
		} else {
			if (calc_parse_entry(current_text, &rhs) != 0) {
				gtk_entry_set_text(GTK_ENTRY(textBox), "ERROR");
				return;
			}
			switch (operation) {
			case OPP_ADD:
				opOne += rhs;
				break;
			case OPP_SUB:
				opOne -= rhs;
				break;
			case OPP_MUL:
				opOne *= rhs;
				break;
			case OPP_DIV:
				if (rhs != 0.0) {
					opOne /= rhs;
				} else {
					gtk_entry_set_text(GTK_ENTRY(textBox), "014 32202");
					return;
				}
				break;
			default:
				break;
			}
			{
				gchar *new_text = g_strdup_printf("%.2lf", opOne);
				gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
				g_free(new_text);
			}
		}
		dis = DIS_CLR;
	}
	if (is_equal) {
		phase = CALC_NEED_LHS;
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
static void do_equl(GtkWidget *widget, gpointer data)
{
	(void)widget;
	(void)data;
	do_operation(NULL, NULL, opp, true);
}
int _init_ui(int   argc,  char *argv[]){
  GtkBuilder *builder;
  GObject *window;
  GObject *button;
  
  GError *error = NULL;

   opOne = 0.0;
   opp = OPP_UND;
   phase = CALC_NEED_LHS;

  gtk_init (&argc, &argv);

  /* Construct a GtkBuilder instance and load our UI description */
  builder = gtk_builder_new ();
  if (gtk_builder_add_from_file (builder, "builder.ui", &error) == 0)
    {
      g_printerr ("Error loading file: %s\n", error->message);
      g_clear_error (&error);
      return 1;
    }

  /* Connect signal handlers to the constructed widgets. */
  window = gtk_builder_get_object (builder, "window1");
  if (window == NULL) {
    g_printerr ("Missing object: window1\n");
    g_object_unref (builder);
    return 1;
  }
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
  textBox = gtk_builder_get_object (builder, "textDisplay");
  if (textBox == NULL) {
    g_printerr ("Missing object: textDisplay\n");
    g_object_unref (builder);
    return 1;
  }
  
  for (int i = 0; i < NUM_BUTTONS; i++) {
		char btnName[NUM_NAME_LENGTH];
		g_snprintf (btnName, sizeof (btnName), "btn%d", i);
		button = gtk_builder_get_object (builder, btnName);
		if (button == NULL) {
			g_printerr ("Missing widget: %s\n", btnName);
			g_object_unref (builder);
			return 1;
		}
		g_signal_connect (button, "clicked", G_CALLBACK (print_number), NULL);
	}

        button = gtk_builder_get_object (builder, "btnac" );
        g_signal_connect (button, "clicked", G_CALLBACK (clear_display), NULL);
        
         button = gtk_builder_get_object (builder, "btndot" );
         g_signal_connect (button, "clicked", G_CALLBACK (add_dot), NULL);
         
         button = gtk_builder_get_object (builder, "btnsign" );
         g_signal_connect (button, "clicked", G_CALLBACK (togal_sign), NULL);
         
         button = gtk_builder_get_object (builder, "btneql" );
         g_signal_connect (button, "clicked", G_CALLBACK (do_equl), NULL);
         
         button = gtk_builder_get_object (builder, "btnsum" );
         g_signal_connect (button, "clicked", G_CALLBACK (do_add), NULL);
         button = gtk_builder_get_object (builder, "btnmin" );
         g_signal_connect (button, "clicked", G_CALLBACK (do_sub), NULL);
  
         button = gtk_builder_get_object (builder, "btnmul" );
         g_signal_connect (button, "clicked", G_CALLBACK (do_mul), NULL);
  
         button = gtk_builder_get_object (builder, "btndiv" );
         g_signal_connect (button, "clicked", G_CALLBACK (do_div), NULL);

  g_object_unref (builder);
  builder = NULL;
  gtk_main ();

  return 0;
	}

int main(int   argc,      char *argv[])
{
	
	return  _init_ui(argc,argv);
}


