/*
 * calc_sdl.c — calculator UI using SDL2 (same state machine as calc.c)
 *
 * Copyright 2024 lahiru <lahiru@lahiru-Lenovo-V15-G2-ITL>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "calc_core.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN_W 400
#define WIN_H 540
#define PAD 12
#define DISP_H 72
#define BTN_H 64
#define BTN_W ((WIN_W - PAD * 2 - 9) / 4) /* 3 gaps of 3px */
#define GAP 3
#define DISP_MAX 10

#define OPP_ADD 1
#define OPP_DIV 2
#define OPP_SUB 3
#define OPP_MUL 4
#define OPP_UND 5

#define DIS_CLR 1
#define DIS_KEP 2

static char s_display[32];
static double opOne;
static int opp;
static int dis;
static CalcPhase phase;

static TTF_Font *s_font;
static TTF_Font *s_font_disp;

typedef enum {
	ACT_NONE = 0,
	ACT_NUM0,
	ACT_NUM1,
	ACT_NUM2,
	ACT_NUM3,
	ACT_NUM4,
	ACT_NUM5,
	ACT_NUM6,
	ACT_NUM7,
	ACT_NUM8,
	ACT_NUM9,
	ACT_DOT,
	ACT_SIGN,
	ACT_AC,
	ACT_ADD,
	ACT_SUB,
	ACT_MUL,
	ACT_DIV,
	ACT_EQ
} Action;

typedef struct {
	SDL_Rect r;
	Action act;
} Btn;

/* row1 AC +/- % /, row2 7–9*, row3 4–6-, row4 1–3+, row5 0(2w) . = */
#define MAX_BTNS 32
static Btn s_btns[MAX_BTNS];
static int s_nbtns;

static void remove_first_char(char *str)
{
	size_t len = strlen(str);
	if (len == 0) {
		return;
	}
	memmove(str, str + 1, len - 1);
	str[len - 1] = '\0';
}

static bool is_empty(const char *str)
{
	return *str == '\0';
}

static bool has_period(const char *str)
{
	for (; *str; str++) {
		if (*str == '.')
			return true;
	}
	return false;
}

static bool has_negative(const char *str)
{
	for (; *str; str++) {
		if (*str == '-')
			return true;
	}
	return false;
}

static void set_display(const char *t)
{
	strncpy(s_display, t, sizeof(s_display) - 1);
	s_display[sizeof(s_display) - 1] = '\0';
}

static void print_number(int digit)
{
	if (dis == DIS_CLR) {
		if (phase == CALC_NEED_LHS) {
			opOne = 0.0;
			opp = OPP_UND;
		}
		set_display("");
		dis = DIS_KEP;
	}
	{
		char dch[2] = { (char)('0' + digit), '\0' };
		char *new_text = (char *)malloc(strlen(s_display) + 2);
		if (!new_text)
			return;
		strcpy(new_text, s_display);
		strcat(new_text, dch);
		if (strlen(new_text) <= (size_t)DISP_MAX) {
			set_display(new_text);
		}
		free(new_text);
	}
}

static void clear_all(void)
{
	set_display("");
	opOne = 0.0;
	opp = OPP_UND;
	phase = CALC_NEED_LHS;
	dis = DIS_KEP;
}

static void add_dot(void)
{
	if (!has_period(s_display)) {
		if (is_empty(s_display)) {
			set_display("0.");
		} else {
			char *nt = (char *)malloc(strlen(s_display) + 2);
			if (!nt)
				return;
			strcpy(nt, s_display);
			strcat(nt, ".");
			if (strlen(nt) <= (size_t)DISP_MAX) {
				set_display(nt);
			}
			free(nt);
		}
	}
}

static void toggle_sign(void)
{
	if (is_empty(s_display))
		return;
	if (has_negative(s_display)) {
		size_t n = strlen(s_display);
		char *cpy = (char *)malloc(n + 1);
		if (!cpy) {
			return;
		}
		memcpy(cpy, s_display, n + 1);
		remove_first_char(cpy);
		set_display(cpy);
		free(cpy);
	} else {
		char *nt = (char *)malloc(strlen(s_display) + 2);
		if (!nt)
			return;
		nt[0] = '-';
		strcpy(nt + 1, s_display);
		if (strlen(nt) <= (size_t)DISP_MAX) {
			set_display(nt);
		}
		free(nt);
	}
}

static void do_operation(int operation, bool is_equal);
static void do_binary(int operation);

static void do_binary(int operation)
{
	do_operation(operation, false);
}

static void do_operation(int operation, bool is_equal)
{
	double rhs;

	opp = operation;
	if (phase == CALC_NEED_LHS) {
		if (calc_parse_entry(s_display, &opOne) != 0) {
			set_display("ERROR");
			return;
		}
		phase = CALC_NEED_RHS;
		set_display("");
	} else {
		if (calc_parse_entry(s_display, &rhs) != 0) {
			set_display("ERROR");
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
				set_display("014 32202");
				return;
			}
			break;
		default:
			break;
		}
		{
			char buf[64];
			snprintf(buf, sizeof(buf), "%.2f", opOne);
			set_display(buf);
		}
	}
	dis = DIS_CLR;
	if (is_equal) {
		phase = CALC_NEED_LHS;
	}
}

static void do_equal(void)
{
	do_operation(opp, true);
}

static void add_btn(int x, int y, int w, int h, Action a)
{
	if (s_nbtns >= MAX_BTNS) {
		return;
	}
	s_btns[s_nbtns].r.x = x;
	s_btns[s_nbtns].r.y = y;
	s_btns[s_nbtns].r.w = w;
	s_btns[s_nbtns].r.h = h;
	s_btns[s_nbtns].act = a;
	s_nbtns++;
}

static void build_layout(void)
{
	int y0 = PAD + DISP_H + GAP;
	int x0 = PAD;

	s_nbtns = 0;
	/* % matches Glade (btnmod); same as original GTK, not wired to logic */
	add_btn(x0, y0, BTN_W, BTN_H, ACT_AC);
	add_btn(x0 + BTN_W + GAP, y0, BTN_W, BTN_H, ACT_SIGN);
	add_btn(x0 + 2 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_NONE);
	add_btn(x0 + 3 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_DIV);
	y0 += BTN_H + GAP;
	add_btn(x0, y0, BTN_W, BTN_H, ACT_NUM7);
	add_btn(x0 + BTN_W + GAP, y0, BTN_W, BTN_H, ACT_NUM8);
	add_btn(x0 + 2 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_NUM9);
	add_btn(x0 + 3 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_MUL);
	y0 += BTN_H + GAP;
	add_btn(x0, y0, BTN_W, BTN_H, ACT_NUM4);
	add_btn(x0 + BTN_W + GAP, y0, BTN_W, BTN_H, ACT_NUM5);
	add_btn(x0 + 2 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_NUM6);
	add_btn(x0 + 3 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_SUB);
	y0 += BTN_H + GAP;
	add_btn(x0, y0, BTN_W, BTN_H, ACT_NUM1);
	add_btn(x0 + BTN_W + GAP, y0, BTN_W, BTN_H, ACT_NUM2);
	add_btn(x0 + 2 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_NUM3);
	add_btn(x0 + 3 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_ADD);
	y0 += BTN_H + GAP;
	add_btn(x0, y0, 2 * BTN_W + GAP, BTN_H, ACT_NUM0);
	add_btn(x0 + 2 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_DOT);
	add_btn(x0 + 3 * (BTN_W + GAP), y0, BTN_W, BTN_H, ACT_EQ);
}

static const char *btn_label(int i)
{
	static const char *const lab[] = {
		"AC", "+/-", "%", "/",
		"7", "8", "9", "*",
		"4", "5", "6", "-",
		"1", "2", "3", "+",
		"0", ".", "="
	};

	if (i < 0 || i >= s_nbtns || i >= (int)(sizeof(lab) / sizeof(lab[0]))) {
		return "";
	}
	return lab[i];
}

static void dispatch(Action a)
{
	switch (a) {
	case ACT_NUM0:
		print_number(0);
		break;
	case ACT_NUM1:
		print_number(1);
		break;
	case ACT_NUM2:
		print_number(2);
		break;
	case ACT_NUM3:
		print_number(3);
		break;
	case ACT_NUM4:
		print_number(4);
		break;
	case ACT_NUM5:
		print_number(5);
		break;
	case ACT_NUM6:
		print_number(6);
		break;
	case ACT_NUM7:
		print_number(7);
		break;
	case ACT_NUM8:
		print_number(8);
		break;
	case ACT_NUM9:
		print_number(9);
		break;
	case ACT_DOT:
		add_dot();
		break;
	case ACT_SIGN:
		toggle_sign();
		break;
	case ACT_AC:
		clear_all();
		break;
	case ACT_ADD:
		do_binary(OPP_ADD);
		break;
	case ACT_SUB:
		do_binary(OPP_SUB);
		break;
	case ACT_MUL:
		do_binary(OPP_MUL);
		break;
	case ACT_DIV:
		do_binary(OPP_DIV);
		break;
	case ACT_EQ:
		do_equal();
		break;
	default:
		break;
	}
}

static int hit_test(int mx, int my)
{
	int i;

	for (i = 0; i < s_nbtns; i++) {
		if (mx >= s_btns[i].r.x && mx < s_btns[i].r.x + s_btns[i].r.w
		    && my >= s_btns[i].r.y
		    && my < s_btns[i].r.y + s_btns[i].r.h) {
			return i;
		}
	}
	return -1;
}

static void open_fonts(void)
{
	const char *paths[] = {
		"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
		"/usr/share/fonts/TTF/DejaVuSans.ttf",
		"/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
		NULL
	};
	int p;

	s_font = NULL;
	s_font_disp = NULL;
	for (p = 0; paths[p]; p++) {
		s_font = TTF_OpenFont(paths[p], 22);
		s_font_disp = TTF_OpenFont(paths[p], 32);
		if (s_font && s_font_disp) {
			return;
		}
		if (s_font) {
			TTF_CloseFont(s_font);
			s_font = NULL;
		}
		if (s_font_disp) {
			TTF_CloseFont(s_font_disp);
			s_font_disp = NULL;
		}
	}
}

static void draw_ui(SDL_Renderer *ren)
{
	SDL_Rect darea;
	SDL_Color bg = { 45, 45, 50, 255 };
	SDL_Color key_bg = { 70, 70, 75, 255 };
	SDL_Color key_hi = { 100, 100, 110, 255 };
	SDL_Color fg = { 240, 240, 240, 255 };
	SDL_Surface *surf;
	SDL_Texture *tex;
	SDL_Rect tr;
	int i;

	SDL_SetRenderDrawColor(ren, bg.r, bg.g, bg.b, 255);
	SDL_RenderClear(ren);

	darea.x = PAD;
	darea.y = PAD;
	darea.w = WIN_W - 2 * PAD;
	darea.h = DISP_H;
	SDL_SetRenderDrawColor(ren, 30, 30, 35, 255);
	SDL_RenderFillRect(ren, &darea);

	if (s_font_disp && s_display[0]) {
		surf = TTF_RenderUTF8_Blended(s_font_disp, s_display, fg);
	} else if (s_font_disp) {
		surf = TTF_RenderUTF8_Blended(s_font_disp, "0", (SDL_Color){ 120, 120, 120, 255 });
	} else {
		surf = NULL;
	}
	if (surf) {
		tex = SDL_CreateTextureFromSurface(ren, surf);
		if (tex) {
			tr.w = surf->w;
			tr.h = surf->h;
			tr.x = darea.x + darea.w - tr.w - 8;
			tr.y = darea.y + (darea.h - tr.h) / 2;
			SDL_RenderCopy(ren, tex, NULL, &tr);
			SDL_DestroyTexture(tex);
		}
		SDL_FreeSurface(surf);
	}

	for (i = 0; i < s_nbtns; i++) {
		SDL_Rect *r = &s_btns[i].r;
		SDL_SetRenderDrawColor(ren, key_bg.r, key_bg.g, key_bg.b, 255);
		SDL_RenderFillRect(ren, r);
		SDL_SetRenderDrawColor(ren, key_hi.r, key_hi.g, key_hi.b, 255);
		SDL_RenderDrawRect(ren, r);
		if (s_font) {
			const char *l = btn_label(i);
			if (l && l[0]) {
				surf = TTF_RenderUTF8_Blended(s_font, l, fg);
				if (surf) {
					tex = SDL_CreateTextureFromSurface(ren, surf);
					if (tex) {
						tr.w = surf->w;
						tr.h = surf->h;
						tr.x = r->x + (r->w - tr.w) / 2;
						tr.y = r->y + (r->h - tr.h) / 2;
						SDL_RenderCopy(ren, tex, NULL, &tr);
						SDL_DestroyTexture(tex);
					}
					SDL_FreeSurface(surf);
				}
			}
		}
	}
	SDL_RenderPresent(ren);
}

int main(int argc, char *argv[])
{
	SDL_Window *win;
	SDL_Renderer *ren;
	int running;
	SDL_Event e;

	(void)argc;
	(void)argv;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 1;
	}
	if (TTF_Init() < 0) {
		fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
		SDL_Quit();
		return 1;
	}

	open_fonts();
	if (!s_font || !s_font_disp) {
		fprintf(
			stderr,
			"No font found (install DejaVu or set paths). TTF: %s\n",
			TTF_GetError()
		);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(
		    WIN_W, WIN_H, SDL_WINDOW_SHOWN, &win, &ren) != 0) {
		fprintf(
			stderr, "CreateWindow: %s\n", SDL_GetError()
		);
		goto out_fonts;
	}
	SDL_SetWindowTitle(win, "Calc (SDL2)");

	s_display[0] = '\0';
	opOne = 0.0;
	opp = OPP_UND;
	phase = CALC_NEED_LHS;
	dis = DIS_KEP;

	build_layout();

	running = 1;
	draw_ui(ren);
	while (running) {
		if (SDL_WaitEvent(&e) == 0) {
			fprintf(stderr, "SDL_WaitEvent: %s\n", SDL_GetError());
			break;
		}
		switch (e.type) {
		case SDL_QUIT:
			running = 0;
			break;
		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_EXPOSED) {
				draw_ui(ren);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT) {
				int idx = hit_test(e.button.x, e.button.y);
				if (idx >= 0 && s_btns[idx].act != ACT_NONE) {
					dispatch(s_btns[idx].act);
				}
				draw_ui(ren);
			}
			break;
		default:
			break;
		}
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
out_fonts:
	if (s_font) {
		TTF_CloseFont(s_font);
	}
	if (s_font_disp) {
		TTF_CloseFont(s_font_disp);
	}
	TTF_Quit();
	SDL_Quit();
	return 0;
}
