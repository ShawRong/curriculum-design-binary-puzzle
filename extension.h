#ifndef extension_h
#define extension_h

#include "struct.h"
#include "new-dpll.h"
#include "FileIO.h"
#include "suduko.h"
#include "createsuduko.h"
#include "showsuduko.h"

void SAT(char* filename1, char* filename2) {
	solver* s;
	s = solver_new();
	readClauseSet(s, filename1);

	clock_t start, stop;


	solver_set(s);

	start = clock();

	if (solver_solve(s)) {
		printf("OK\n");
	}
	else {
		printf("NOT OK\n");
	}

	stop = clock();

	double time = (double)(stop - start) / CLOCKS_PER_SEC;

	printf("time = %lf ms\n",time*1000);

	s->time = time*1000;

	writeSolution(s, filename2);
	print_solution(filename2);
	destroy_solver(s);
	free(s);
}

suduko* GENERATE_SUDUKO() {
	int degree;
	printf("input the degree and it has to be even number(4<= x and x <= 16)\n");
	while (true) {
		scanf("%d", &degree);
		if (degree <= 16 && degree >= 4) {
			break;
		}
	}
	suduko_generater* generater = new_suduko_generater(degree);
	suduko* su = Generate_Suduko(generater);
	destroy_suduko_generater(generater);
	system("cls");
	return su;
}


void SUDUKO() {
	suduko* su = GENERATE_SUDUKO();
	Show(su);
	int i, j, k;
	while (true) {
		scanf("%d/%d/%d", &i, &j, &k);
		if (i == 0 && j == 0 && k == 0) {
			system("cls");
			solver* s = solver_new();
			sudukotosat(su, s);
			solver_set(s);
			solver_solve(s);

			int t; int x; int y; lit l;
			for (t = 1; t <= su->degree * su->degree; t++) {
				l = tolit(t);
				vtoxy(&x, &y, su->degree, t);
				if (s->valuation[l] < 0) {
					su->space[toindex(x)][toindex(y)] = 0;
				}
				else {
					su->space[toindex(x)][toindex(y)] = 1;
				}
			}

			destroy_solver(s);
			free(s);
			Show(su);
			break;
		}
		else if (i == 0 && j == 0 && k == 1) {
			system("cls");
			su = GENERATE_SUDUKO();
			Show(su);
		}
		else if (k == 1 || k == 0) {
			if (su->space[i - 1][j - 1] == -1) {
				new_num(i - 1, j - 1, 4, 11, k);
			}
			gotoxy(4 + su->degree * 4 + 2, 11 + 8);
			printf("                                 ");
			gotoxy(4 + su->degree * 4 + 2, 11 + 8);
		}
		else {
			continue;
		}
	}
	
}



#endif
