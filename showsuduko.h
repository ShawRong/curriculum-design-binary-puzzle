#ifndef showsuduko_h
#define showsuduko_h
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include "suduko.h"

void gotoxy(int x, int y) {
	++x, ++y;
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void color(int b) {
	HANDLE hConsole = GetStdHandle((STD_OUTPUT_HANDLE));
	SetConsoleTextAttribute(hConsole, b);
}

void setwhite() {
	HANDLE hOut;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void new_mark(int io,int jo,int degree) {
	int i;
	for (i = 0; i < degree; i++) {
		gotoxy(io + i * 4 + 2,jo - 1);
		printf("%d", i + 1);
	}
	for (i = 0; i < degree; i++) {
		gotoxy(io - 1, jo + i * 2 + 1);
		printf("%d", i + 1);
	}
}

void new_block(int i,int j,int io,int jo) {
	gotoxy(io + i * 4, jo + j * 2);
	printf("+---+");
	gotoxy(io + i * 4, jo + j * 2 + 1);
	printf("|");
	gotoxy(io + 4 + i * 4, jo + j * 2 + 1);
	printf("|");
	gotoxy(io + i * 4, jo + j * 2 + 2);
	printf("+---+");
}
void new_num(int i, int j, int io, int jo,int num) {
	gotoxy(io + j * 4 + 2,jo + i * 2 + 1);
	printf("%d", num);
}

void Show(suduko* s) {
	int io = 4; int jo = 11;
	int i; int j;
	color(7);
	new_mark(io, jo, s->degree);
	for (i = 0; i < s->degree; i++) {
		for (j = 0; j < s->degree; j++) {
			new_block(i,j,io,jo);
		}
	}
	color(6);
	for (i = 0; i < s->degree; i++) {
		for (j = 0; j < s->degree ; j++) {
			if (s->space[i][j] != -1) {
				new_num(i, j, io, jo, s->space[i][j]);
			}
		}
	}

	gotoxy(io + s->degree * 4 + 2,jo + 4);
	printf("Welcome");
	gotoxy(io + s->degree * 4 + 2, jo + 5);
	printf("input 0/0/1, which means to create another one");
	gotoxy(io + s->degree * 4 + 2, jo + 6);
	printf("input 1/2/0, which means that put 0 at col 2 & row 1");
	gotoxy(io + s->degree * 4 + 2, jo + 7);
	printf("if you input 0/0/0, you will see auto solving and exit");
	gotoxy(io + s->degree * 4 + 2, jo + 8);
	setwhite();
}

#endif
