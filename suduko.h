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

void special_restoxy(int* x, int* y, int degree, int res) {
	int i, j;
	for (i = degree - 1; i > 0; i--) {
		j = res - (i - 1) * (2 * degree - i) / 2;
		if (j > 0) {
			*y = j + i;
			*x = i;
			return;
		}
	}
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

int tosequential(int x,int y,int degree) {
	int res;
	res = (-1 * (x * x) + (2 * degree - 1) * x + 2 * y - 2 * degree)/2;
	return res;
}
void breakup(int num,int* remain) {
	int i = 0;
	while (num) {
		remain[i] = num % 10;
		num /= 10;
		i++;
	}
}

int arraytonum_5(int* a) {
	int i = 4;
	int num = 0;
	while (a[i] != -1 && i >= 0) {
		num *= 10;
		num += a[i];
		i--;
	}
	return num;
}

int new_addition(int a,int b,int c,int d,int e) {  //no exist = -1
	int array[5]; int index; int ans;
	for (index = 0; index < 5; index++) {
		array[index] = -1;
	}
	array[4] = a;
	array[3] = b ;
	array[2] = c;
	array[1] = d;
	array[0] = e;
	ans = arraytonum_5(array);
	return ans;
}

lit additional_bool(int input, int degree) {
	bool row = true; int remain[5]; int order_3; int order_2; int order_1;
	int v; int i; lit l;
	//initial
	for (i = 0; i < 5; i++) {
		remain[i] = 0;
	}

	breakup(input,remain);
	if (remain[3] == 0) {
		if (remain[2] == 2) row = false;
		order_3 = tosequential(remain[1], remain[0],degree);
		order_2 = degree + 1;
		order_1 = 1;
	} else if(remain[4] == 0){
		if (remain[3] == 2) row = false;
		order_3 = tosequential(remain[2], remain[1], degree);
		order_2 = remain[0];
		order_1 = 3;
	} else {
		if (remain[4] == 2) row = false;
		order_3 = tosequential(remain[3], remain[2], degree);
		order_2 = remain[1];
		if (remain[0] == 1) {
			order_1 = 1;
		}else if (remain[0] == 0) {
			order_1 = 2;
		}
	}
	v = order_1 + (order_2 - 1) * 3 + (order_3 - 1) * (3 * degree + 1);
	if (!row) {
		int max = (3 * degree + 1) * (tosequential(degree - 1, degree, degree));
		v += max;
	}
	v += degree * degree;
	l = tolit(v);
	return l;
}
/*=============================================================
example: 1 5 8 1 1
order_1: the unit's place
		1--> first ->1
		0--> second ->2
		_--> third ->3
		if _ _ -> 1
order_2: the ten's place
		degree--> biggest ->d
		1--> smallest ->1
		_ --> d+1
order_3: the hundred's place and the thousand's position
		using tosqential function to order
		1,2 --> samllest
		d-1,d --> biggest

the order = order_1 + (order_2-1)*3 + (order_3-1)*(3*d+1)

==============================================================*/

//the biggest degree that system supported is 8

void addclauses_inrule3(suduko* su, vecp* clauses, int row,int i,int j,int t,int m) { // m = 0 or 1 //row = 1 or 2
	vecl lits; lit l; clause* c;
	//15711-1
	vecl_new(&lits);
	l = build_literal(i, t, su->degree, true);
	if (m == 0) l = lit_neg(l);
	vecl_push(&lits, l);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, m), su->degree);
	l = lit_neg(l);
	vecl_push(&lits, l);
	c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
	vecp_push(clauses, c);
	vecl_delete(&lits);
	//15711-2
	vecl_new(&lits);
	l = build_literal(j, t, su->degree, true);
	if (m == 0) l = lit_neg(l);
	vecl_push(&lits, l);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, m), su->degree);
	l = lit_neg(l);
	vecl_push(&lits, l);
	c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
	vecp_push(clauses, c);
	vecl_delete(&lits);
	//15711-3
	vecl_new(&lits);
	l = build_literal(i, t, su->degree, true);
	if(m == 1) l = lit_neg(l);
	vecl_push(&lits, l);
	l = build_literal(j, t, su->degree, true);
	if (m == 1) l = lit_neg(l);
	vecl_push(&lits, l);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, m), su->degree);
	vecl_push(&lits, l);
	c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
	vecp_push(clauses, c);
	vecl_delete(&lits);
}
void addclauses_inrule3_(suduko* su,vecp* clauses, int row,int i,int j,int t) {
	lit l; vecl lits; clause* c;
	//1
	vecl_new(&lits);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, 1), su->degree);
	l = lit_neg(l);
	vecl_push(&lits, l);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, -1), su->degree);
	vecl_push(&lits, l);
	c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
	vecp_push(clauses, c);
	vecl_delete(&lits);
	//2
	vecl_new(&lits);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, 0), su->degree);
	l = lit_neg(l);
	vecl_push(&lits, l);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, -1), su->degree);
	vecl_push(&lits, l);
	c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
	vecp_push(clauses, c);
	vecl_delete(&lits);
	//3
	vecl_new(&lits);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, 1), su->degree);
	vecl_push(&lits, l);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, 0), su->degree);
	vecl_push(&lits, l);
	l = additional_bool(new_addition(row, i + 1, j + 1, t + 1, -1), su->degree);
	l = lit_neg(l);
	vecl_push(&lits, l);
	c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
	vecp_push(clauses, c);
	vecl_delete(&lits);
}

void addclauses_inrule3__(suduko* su,vecp* clauses, int row,int i,int j) {
	vecl lits; int index = 0; lit save = additional_bool(new_addition(row, i + 1, j + 1, -1, -1), su->degree);
	lit temp; clause* c;
	//1
	vecl_new(&lits);
	for (index = 0; index < su->degree; index++) {
		temp = additional_bool(new_addition(row, i + 1, j + 1, index + 1, -1),su->degree);
		temp = lit_neg(temp);
		vecl_push(&lits,temp);
	}
	temp = lit_neg(save);
	vecl_push(&lits, temp);
	c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
	vecp_push(clauses, c);
	vecl_delete(&lits);
	//else
	for (index = 0; index < su->degree; index++) {
		vecl_new(&lits);
		temp = additional_bool(new_addition(row, i + 1, j + 1, index + 1, -1),su->degree);
		vecl_push(&lits, temp);
		vecl_push(&lits, save);
		c = new_clause(vecl_begin(&lits), vecl_begin(&lits) + vecl_size(&lits));
		vecp_push(clauses,c);
		vecl_delete(&lits);
	}
}

void rule_3(suduko* su,vecp* clauses) {
	int i; int j; int t; clause* c;
	//row
	for (i = 0; i < su->degree; i++) {
		for (j = i + 1; j < su->degree; j++) {
			for (t = 0; t < su->degree; t++) {
				//15711
				addclauses_inrule3(su, clauses,1, i, j, t, 1);
				addclauses_inrule3(su, clauses,1, i, j, t, 0);
				addclauses_inrule3_(su, clauses,1, i, j, t);
			}
			addclauses_inrule3__(su, clauses, 1, i, j);
		}
	}
	//col
	for (i = 0; i < su->degree; i++) {
		for (j = i + 1; j < su->degree; j++) {
			for (t = 0; t < su->degree; t++) {
				//25711
				addclauses_inrule3(su, clauses, 2, i, j, t, 1);
				addclauses_inrule3(su, clauses, 2, i, j, t, 0);
				addclauses_inrule3_(su, clauses, 2, i, j, t);
			}
			addclauses_inrule3__(su, clauses, 2, i, j);
		}
	}
}


void sudukotosat(suduko* su,solver* s) {
	s->numofvar = lit_val(additional_bool(new_addition(2,su->degree-1,su->degree,-1,-1), su->degree));
	
	// new unit clause
	new_unitclause(su, &s->clauses);
	//rules
	rule_1(su, &s->clauses);
	rule_2(su, &s->clauses);
	rule_3(su, &s->clauses);
	s->numofclause = vecp_size(&s->clauses);
	s->tail = vecp_size(&s->clauses);
}



int trans_fromorder(int order_1,int order_2,int order_3,bool row,suduko* su) {
	int array[5]; int i;
	for (i = 0; i < 5; i++) {
		array[i] = -1;
	}
	if (row) {
		array[4] = 1;
	} else {
		array[4] = 2;
	}
	int x; int y;
	special_restoxy(&x,&y,su->degree,order_3);
	array[3] = x; array[2] = y;
	if (order_2 == 0 /*su->degree + 1*/) {
		return arraytonum_5(array);
	} else {
		array[1] = order_2;
	}
	if (order_1 == 1) {
		array[0] = 1;
	} else if(order_1 == 2) {
		array[0] = 0;
	} else if (order_1 == 0) {
		return arraytonum_5(array);
	}
	return arraytonum_5(array);
}

/*=============================================================
example: 1 5 8 1 1
order_1: the unit's place
		1--> first ->1
		0--> second ->2
		_--> third ->3
		if _ _ -> 1
order_2: the ten's place
		degree--> biggest ->d
		1--> smallest ->1
		_ --> d+1
order_3: the hundred's place and the thousand's position
		using tosqential function to order
		1,2 --> samllest
		d-1,d --> biggest

the order = order_1 + (order_2-1)*3 + (order_3-1)*(3*d+1)

==============================================================*/

void print_clause_suduko(clause* c, suduko* s) {
	int i; int x; int y; int v;
	for (i = 0; i < c->size; i++) {
		v = lit_val(c->lits[i]);
		bool row;
		if (abs(v) > s->degree*s->degree) {
			if (v < 0) {
				v = -1 * v;
				printf("-");
			}
			v -= s->degree * s->degree; 
			int max = (3 * s->degree + 1) * (tosequential(s->degree - 1, s->degree, s->degree));
			if (v > max) {
				row = false;
				v -= max;
			} else {
				row = true;
			}
			int order_1; int order_2; int order_3;
			if (v % (3 * s->degree + 1) == 0) {
				order_3 = v / (3 * s->degree + 1) + 1 -1;
			} else {
				order_3 = v / (3 * s->degree + 1) + 1;
			}
			v = v % (3 * s->degree + 1);
			if (v % 3 == 0) {
				order_2 = v / 3 + 1 -1;
			} else {
				order_2 = v / 3 + 1;
			}
			v = v % 3;
			order_1 = v;
			v = trans_fromorder(order_1,order_2,order_3,row,s);
			printf("%d ", v);
		} else {
			vtoxy(&x, &y, s->degree, abs(v));
			if (v < 0) {
				printf("-%d%d ", x, y);
			}
			else {
				printf("%d%d ", x, y);
			}
		}
		
	}
	printf("\n");
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
