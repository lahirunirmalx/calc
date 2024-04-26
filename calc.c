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


#include <stdio.h>
#include <stdbool.h> 
#include <gtk/gtk.h> 
#include <string.h>
#include <math.h>

#define NUM_BUTTONS  10 
#define NUM_NAME_LENGTH  40
#define NUM_VALUE_LENGTH  10 

#define OPP_ADD 1
#define OPP_DIV 2 
#define OPP_SUB 3 
#define OPP_MUL 4 
#define OPP_UND 5 

#define DIS_CLR 1
#define DIS_KEP 2

GObject *textBox;
double opOne;
double opTwo;
int    opp;
int    dis;


static void remove_first_char(char *str) {
  int len = strlen(str);
  if (len > 0) { 
    memmove(str, str + 1, len - 1); 
    str[len - 1] = '\0';
  }
}


static int string_to_decimal(const char *str, double *decimal_value) {
  double value = 0.0;
  int decimal_place = 0;
  bool is_negative = false;

  if (*str == '-') {
    is_negative = true;
    str++;  // Skip the minus sign
  }

  while (*str != '\0') {
    if (*str >= '0' && *str <= '9') {
      value = value * 10 + (*str - '0');
    } else if (*str == '.' && decimal_place == 0) {
      decimal_place = 1;
    } else {
      return -1;  // Invalid character encountered
    }
    str++;
  }

  if (decimal_place > 0) {
    value /= pow(10, decimal_place);
  }

  *decimal_value = is_negative ? -value : value;
  return 0;
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

static void print_number (GtkWidget *widget, gpointer   data) {
	 if(dis == DIS_CLR){
		 gtk_entry_set_text(GTK_ENTRY(textBox), "");
		 dis = DIS_KEP;
		 }
    GtkButton *gtk_button = GTK_BUTTON(widget);  
    const gchar *label_text = gtk_button_get_label(gtk_button);
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(textBox));
    gchar *new_text = g_strdup_printf("%s%s", current_text,label_text);
    
    g_print("Button label: %s\n", label_text);
    gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
    g_free(new_text);
}
static void clear_display (GtkWidget *widget, gpointer   data) {
	gtk_entry_set_text(GTK_ENTRY(textBox), "");
	opOne = 0.0;
    opTwo = 0.0;
    opp =OPP_UND;
    dis = DIS_KEP;
	g_print("display vlaue cleared \n");
	}


	
static void add_dot (GtkWidget *widget, gpointer   data) {
	const char *current_text = gtk_entry_get_text(GTK_ENTRY(textBox)); 
	gchar *new_text;
	 if(!has_period(current_text)){ 
		if (is_empty(current_text)) { 
           new_text = g_strdup_printf("%s%s", "0","."); 

       } else { 
           new_text = g_strdup_printf("%s%s", current_text,".");
      
       }
       gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
       g_free(new_text);
	 }  
	}	

static void togal_sign (GtkWidget *widget, gpointer   data) {
	const char *current_text = gtk_entry_get_text(GTK_ENTRY(textBox)); 
	gchar *new_text;
	 if(!is_empty(current_text)){ 
		if (has_negative(current_text)) { 
           new_text = g_strdup_printf("%s", current_text); 
           remove_first_char(new_text);

       } else { 
           new_text = g_strdup_printf("%s%s", "-",current_text);
      
       }
       gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
       g_free(new_text);
	 }  
	}

  static void do_operation(GtkWidget *widget, gpointer data, int operation) {
  opp = operation;
  const char *current_text = gtk_entry_get_text(GTK_ENTRY(textBox));

  if (opOne == 0.0) {
    string_to_decimal(current_text, &opOne);
    gtk_entry_set_text(GTK_ENTRY(textBox), "");
  } else {
    double opTwo;
    string_to_decimal(current_text, &opTwo);

    switch (operation) {
      case OPP_ADD:
        opOne += opTwo;
        break;
      case OPP_SUB:
        opOne -= opTwo;
        break;
      case OPP_MUL:
        opOne *= opTwo;
        break;
      case OPP_DIV:
        if (opTwo != 0.0) {
          opOne /= opTwo;
        } else {
           gtk_entry_set_text(GTK_ENTRY(textBox), "014 32202");
          return;
        }
        break;
    }

    gchar *new_text = g_strdup_printf("%.2lf", opOne);
    gtk_entry_set_text(GTK_ENTRY(textBox), new_text);
    g_free(new_text);
  }

  dis = DIS_CLR;
}
static void do_add(GtkWidget *widget, gpointer data) {
  do_operation(widget, data, OPP_ADD);
}

static void do_sub(GtkWidget *widget, gpointer data) {
  do_operation(widget, data, OPP_SUB);
}

static void do_mul(GtkWidget *widget, gpointer data) {
  do_operation(widget, data, OPP_MUL);
}

static void do_div(GtkWidget *widget, gpointer data) {
  do_operation(widget, data, OPP_DIV);
}

static void do_equl (GtkWidget *widget, gpointer   data) {
 do_operation(widget, data, opp);
 }
int _init_ui(int   argc,  char *argv[]){
  GtkBuilder *builder;
  GObject *window;
  GObject *button;
  
  GError *error = NULL;

   opOne = 0.0;
   opTwo = 0.0;
   opp =OPP_UND;

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
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
   textBox = gtk_builder_get_object (builder, "textDisplay");
  
  for (int i = 0;i < NUM_BUTTONS;i ++){
	    char btnName[NUM_NAME_LENGTH] = "btn";
	    char btnNum[NUM_VALUE_LENGTH];
	    sprintf(btnNum,"%d",i);
	    strcat(btnName,btnNum);
	    button = gtk_builder_get_object (builder, btnName );
        g_signal_connect (button, "clicked", G_CALLBACK (print_number), btnNum);

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
  
  
  gtk_main ();

  return 0;
	}

int main(int   argc,      char *argv[])
{
	
	return  _init_ui(argc,argv);
}


