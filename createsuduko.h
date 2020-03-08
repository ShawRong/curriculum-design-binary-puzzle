#ifndef create_suduko_h
#define create_suduko_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "extension.h"

typedef struct suduko_generater_t {
	suduko* borad;
	stack save;
	bool* canbe_hole;
	int* sequence;
	bool OK;
}suduko_generater;

bool las_vegas(int n,int degree,suduko* su) {
	suduko_set(su, degree);
	srand((unsigned)time(NULL));
	int i; int random; int x; int y; int t; bool res;
	for (i = 0; i <= n; i++) {
		random = rand() %(su->degree*su->degree) + 1;
		vtoxy(&x, &y, su->degree, random);
		t = rand() % 2;
		su->space[toindex(x)][toindex(y)] = t;
	}
	solver* s = solver_new();
	sudukotosat(su, s);
	solver_set(s);
	solver_solve(s);
	res = s->satisifable;
	destroy_solver(s);
	free(s);
	if (res == true) {
		return true;
	} else {
		destroy_suduko(su);
		return false;
	}
}

suduko* make_board(int degree) {  //use random num as seed
	suduko* su = new_suduko();
	suduko_set(su, degree);
	while (!las_vegas(5, su->degree, su));
	return su;
}

suduko_generater* new_suduko_generater(int degree) {
	suduko_generater* generater = (suduko_generater*)malloc(sizeof(suduko_generater));
	generater->borad = make_board(degree);
	new_stack(&generater->save);
	generater->OK = false;
	generater->canbe_hole = (bool*)malloc(sizeof(bool) * (degree * degree + 1));
	int i;
	for (i = 0; i < degree * degree; i++) {
		generater->canbe_hole[i] = true;
	}
	generater->sequence = (int*)malloc(sizeof(int) * (degree * degree));
	return generater;
}

void destroy_suduko_generater(suduko_generater* generater) {
	free(generater->canbe_hole);
	delete_stack(&generater->save);
	free(generater->sequence);
	free(generater);
}


void fill_borad(suduko_generater* generater) {
	int i; int x; int y; lit l;
	suduko* su = generater->borad;
	solver* s = solver_new();
	sudukotosat(su, s);
	solver_set(s);
	solver_solve(s);
	for (i = 1; i <= su->degree * su->degree; i++) {
		l = tolit(i);
		vtoxy(&x, &y, su->degree, i);
		if (s->valuation[l] < 0) {
			su->space[toindex(x)][toindex(y)] = 0;
		} else {
			su->space[toindex(x)][toindex(y)] = 1;
		}
	}
	destroy_solver(s);
	free(s);
}

void dig(suduko_generater* generater,int i,int j) {
	suduko* su = generater->borad;
	su->space[i][j] = -1;
	
}

bool gene_back(suduko_generater* generater) {
	int v; int x; int y; int mark;
	suduko* su = generater->borad;
	if (stack_pop(&generater->save, &v)) {
		if (v < 0) {
			mark = 0;
			v = -1 * v;
		}
		else if (v > 0) {
			mark = 1;
		}
		vtoxy(&x, &y, su->degree, v);
		su->space[toindex(x)][toindex(y)] = mark;
		return true;
	} else {
		return false;
	}
	
}

void set_sequence(suduko_generater* generater) {  //sequential
	int i;
	for (i = 0; i < generater->borad->degree* generater->borad->degree; i++) {
		generater->sequence[i] = i + 1;
	}
}

/*
void set_forbid(suduko_generater* genetater) {
	suduko* su = genetater->borad;
	int i; int j; int v;
	for (i = 0; i < su->degree; i++) {
		for (j = 0; j < su->degree; j++) {
			if (su->space[i][j] != -1) {
				v = xytov(toi(i), toi(j), su->degree);
				genetater->canbe_hole[v] = false;
			}
		}
	}
}*/

bool exist_canbe_hole(suduko_generater* generater) {
	int i = 0; suduko* su = generater->borad;
	for (i = 0; i < su->degree * su->degree; i++) {
		if (generater->canbe_hole[i]) {
			return true;
		}
	}
	return false;
}

bool sol_isunique(suduko_generater* generater,int choice) {
	int x; int y;
	vtoxy(&x, &y, generater->borad->degree, choice);

	suduko* su = generater->borad;
	solver* s = solver_new();
	
	int save = su->space[toindex(x)][toindex(y)];
	if (su->space[toindex(x)][toindex(y)] == 0) {
		su->space[toindex(x)][toindex(y)] = 1;
	} else if (su->space[toindex(x)][toindex(y)] == 1) {
		su->space[toindex(x)][toindex(y)] = 0;
	}
	sudukotosat(su, s);
	solver_set(s);
	if (solver_solve(s)) {
		su->space[toindex(x)][toindex(y)] = save;
		destroy_solver(s);
		free(s);
		return false;
	} else {
		su->space[toindex(x)][toindex(y)] = save;
		destroy_solver(s);
		free(s);
		return true;
	}
}

suduko* Generate_Suduko(suduko_generater* generater) {
	set_sequence(generater);
	int i = 0; int choice; int v; int x; int y; solver* s;
	bool flag = false;
	fill_borad(generater);
	i = 0;
	while (exist_canbe_hole(generater) && i < generater->borad->degree * generater->borad->degree) {
		choice = generater->sequence[i];
		if (generater->canbe_hole[choice]) {
			generater->canbe_hole[choice] = false;
			vtoxy(&x,&y,generater->borad->degree,choice);
			if (sol_isunique(generater,choice)) {
				dig(generater, toindex(x), toindex(y));
			}
		}
		else {
			i++;
		}
	}
	return generater->borad;
}




#endif create_suduko_h
