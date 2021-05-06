/*************************************************************************
	> File Name: ncurses.h
	> Author: 
	> Mail: 
	> Created Time: Sun 23 Aug 2020 11:27:21 AM CST
 ************************************************************************/

#ifndef _SETSCREEN_H
#define _SETSCREEN_H
#include<head.h>
#define MSG_WIDTH 100
#define MSG_HEIGHT 25
#define HELP_WIDTH 15
#define INPUT_HEIGHT 5

WINDOW *create_newwin(int width, int height, int starty, int startx);
void gotoxy(int x, int y);
void gotoxy_putc(int x, int y, char c);
void gotoxy_puts(int x, int y, char *s);
void w_gotoxy_putc(WINDOW *win, int x, int y, char c);
void w_gotoxy_puts(WINDOW *win, int x, int y, char *s);
void init_ui();
void print_message(WINDOW *win, struct ChatMsg *msg, int type);

#endif
