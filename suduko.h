#ifndef suduko_h

#include "struct.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

int toindex(int i) {
	return i - 1;
}
int toi(int index) {
	return index + 1;
}

int xytov(int x, int y, int degree) {
	int v = (x - 1) * degree + y;
	return v;
}

void vtoxy(int* x,int* y,int degree,int v) {
	*y = v % degree;
	if (*y == 0) {
		*y = degree;
	}
	*x = (v - *y) / degree + 1;
}

typedef struct suduko_t {
	int degree;
	char** space;
}suduko;

char** new_suduko_space(int degree) {
	int i; int j;
	char** space = (char**)malloc(sizeof(char*) * degree);
	for (i = 0; i < degree; i++) {
		space[i] = (char*)malloc(sizeof(char) * degree);
	}
	for (i = 0; i < degree; i++) {
		for (j = 0; j < degree; j++) {
			space[i][j] = -1;
		}
	}
	return space;
}

suduko* new_suduko() {
	suduko* s = (suduko*)malloc(sizeof(suduko));
	return s;
}
void suduko_set(suduko* s,int degree) {
	s->degree = degree;
	s->space = new_suduko_space(s->degree);
}

void destroy_suduko(suduko* su) {
	int i = 0;
	for (i = 0; i < su->degree; i++) {
		free(su->space[i]);
	}
	free(su->space);
	free(su);
}

void read_suduko(suduko* s,const char* filename) {
	FILE* fp;
	char line[256];
	size_t len = 0;

	fp = fopen(filename, "r");
	if (fp == NULL) printf("cannot open file"), exit(1);

	char* token;
	char x; char y; char v;

	while (fgets(line, sizeof(line), fp)) {
		if (line[0] == 'd') {
			int degree;
			sscanf(line, "d %d", &degree);
			suduko_set(s,degree);
			continue;
		}
		int i = 0;
		token = strtok(line, " ");
		x = atoi(token);
		token = strtok(NULL, " ");
		y = atoi(token);
		token = strtok(NULL, " ");
		v = atoi(token);
		s->space[toindex(x)][toindex(y)] = v;
	}
	fclose(fp);
}

void print_suduko(suduko* s) {
	int i = 0; int j = 0;
	for (i = 0; i < s->degree; i++) {
		for (j = 0; j < s->degree; j++) {
			printf("%d ", toi(i));
			printf("%d ", toi(j));
			printf("*%d* :", xytov(toi(i),toi(j),s->degree));
			printf("%d", s->space[i][j]);
			printf("\n");
		}
	}
}

void print_clause_suduko(clause* c,suduko* s) {
	int i; int x; int y; int v;
	for (i = 0; i < c->size; i++) {
		v = lit_val(c->lits[i]);
		vtoxy(&x, &y, s->degree, abs(v));
		if (v < 0) {
			printf("-%d%d ", x, y);
		} else {
			printf("%d%d ", x, y);
		}
	}
	printf("\n");
}

lit build_literal(int i,int j,int degree,bool pos) {
	int v; lit l;
	v = xytov(toi(i), toi(j),degree);
	if (!pos) {
		v = -1 * v;
	}
	l = tolit(v);
	return l;
}

//new unit literal
void new_unitclause(suduko* su,vecp* clauses) {
	int i; int j; lit l;
	clause* c;
	for (i = 0; i < su->degree; i++) {
		for (j = 0; j < su->degree; j++) {
			if (su->space[i][j] != -1) {

				if (su->space[i][j] == 0) {
					l = build_literal(i, j, su->degree,false);
					c = new_clause(&l, &l + 1);
					vecp_push(clauses, c);
				} else {
					l = build_literal(i, j, su->degree, true);
					c = new_clause(&l, &l + 1);
					vecp_push(clauses, c);
				}
			}
		}
	}
}

/*
clause* rule_1_addcluase(int i, int j, int degree) {
	int v; int l[3];
	v = xytov(toi(i), toi(j), degree);
	l[0] = tolit(v);
	v = xytov(toi(i + 1), toi(j + 1), degree);
	l[1] = tolit(v);
	v = xytov(toi(i + 2), toi(j + 2), degree);
	l[2] = tolit(v);
	c = new_clause(l, l + 3);
	clause* c = new_clause();
	return c;
}*/

void rule_1(suduko* su, vecp* clauses) {
	int i; int j; lit l[3];
	clause* c;
	// in row
	for (i = 0; i < su->degree; i++) {
		for (j = 0; j < su->degree - 2; j++) {
			//no 3 1
			l[0] = build_literal(i, j, su->degree, true);
			l[1] = build_literal(i, j + 1, su->degree, true);
			l[2] = build_literal(i, j + 2, su->degree, true);
			c = new_clause(l,l + 3);
			vecp_push(clauses, c);
			//no 3 0
			l[0] = build_literal(i, j, su->degree, false);
			l[1] = build_literal(i, j + 1, su->degree, false);
			l[2] = build_literal(i, j + 2, su->degree, false);
			c = new_clause(l, l + 3);
			vecp_push(clauses, c);
		}
	}
	//in col
	for (j = 0; j < su->degree; j++) {
		for (i = 0; i < su->degree - 2; i++) {
			//no 3 1
			l[0] = build_literal(i, j, su->degree, true);
			l[1] = build_literal(i + 1, j, su->degree, true);
			l[2] = build_literal(i + 2, j, su->degree, true);
			c = new_clause(l, l + 3);
			vecp_push(clauses, c);
			//no 3 0
			l[0] = build_literal(i, j, su->degree, false);
			l[1] = build_literal(i + 1, j, su->degree, false);
			l[2] = build_literal(i + 2, j, su->degree, false);
			c = new_clause(l, l + 3);
			vecp_push(clauses, c);
		}
	}
}
//rule 2
//from n select m


typedef struct info_t {
	bool row;  //true = row flase = col
	int xory;
	int n;
	int m;
	int* process;
	int* data;
}INFO;

INFO* new_INFO(int n,int m) {
	INFO* info = (INFO*)malloc(sizeof(INFO));
	int i;
	info->row = true;
	info->xory = 0;
	info->n = n;
	info->m = m;
	info->data = (int*)malloc(sizeof(int) * (info->n));
	info->process = (int*)malloc(sizeof(int) * info->m);
	for (i = 0; i < n; i++) {
		info->data[i] = i + 1;
	}
	return info;
}
void destroy_INFO(INFO* info) {
	free(info->process);
	free(info->data);
	free(info);
}

bool check(int t,INFO* info) {
	int i = 0;
	for (i = 0; i < info->m; i++) {
		if (t == info->process[i]) {
			return true;
		}
	}
	return false;
}

void combination(int start,int picked,vecp* clauses,INFO* info) {
	int i; clause* c; int t;
	if (picked == info->m) {
		vecl lits_pos; vecl_new(&lits_pos);
		vecl lits_neg; vecl_new(&lits_neg);
		for (t = 0; t < info->n; t++) {
			if (check(info->data[t], info)) {
				continue;
			}
			if (info->row) {
				vecl_push(&lits_pos,build_literal(info->xory, info->data[t]-1,info->n,true));
				vecl_push(&lits_neg, build_literal(info->xory, info->data[t]-1, info->n, false));
			} else {
				vecl_push(&lits_pos, build_literal(info->data[t]-1, info->xory, info->n, true));
				vecl_push(&lits_neg, build_literal(info->data[t]-1, info->xory, info->n, false));
			}
		}
		c = new_clause(vecl_begin(&lits_pos), vecl_begin(&lits_pos) + vecl_size(&lits_pos));
		vecp_push(clauses, c);
		c = new_clause(vecl_begin(&lits_neg), vecl_begin(&lits_neg) + vecl_size(&lits_neg));
		vecp_push(clauses, c);
		vecl_delete(&lits_pos);
		vecl_delete(&lits_neg);
		return;
	}
	int max = info->n - info->m + picked;

	for (i = start; i <= max; i++) {
		info->process[picked] = info->data[i];
		combination(i + 1, picked + 1,clauses,info);
	}
}

void rule_2(suduko* su, vecp* clauses) {
	INFO* info = new_INFO(su->degree, su->degree / 2 - 1);
	int i = 0;
	for (i = 0; i < su->degree; i++) {
		info->row = true;
		info->xory = i;
		combination(0, 0, clauses, info);
	}
	for (i = 0; i < su->degree; i++) {
		info->row = false;
		info->xory = i;
		combination(0, 0, clauses, info);
	}
	destroy_INFO(info);
}


void sudukotosat(suduko* su,vecp* clauses) {
	// new unit clause
	new_unitclause(su, clauses);
	//rules
	rule_1(su, clauses);
	rule_2(su, clauses);
}

/*==========================================================
file:
	(x y v)
	1 1 1
	2 2 0
	......
==========================================================*/
#define suduko_h
#endif
