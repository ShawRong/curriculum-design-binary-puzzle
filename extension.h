#ifndef extension_h
#define extension_h

#include "struct.h"
#include "dpll.h"
#include "FileIO.h"
#include "suduko.h"
#include "createsuduko.h"

void SAT(char* filename1, char* filename2) {
	solver* s;
	s = solver_new();
	readClauseSet(s, filename1);

	solver_set(s);
	if (solver_solve(s)) {
		printf("OK\n");
	}
	else {
		printf("NOT OK\n");
	}

	writeSolution(s, filename2);
	print_solution(filename2);
	destroy_solver(s);
	free(s);
}
void SUDUKO(char* filename1, char* filename2) {
	suduko* su = new_suduko();
	solver* s = solver_new();

	read_suduko(su, filename1);
	print_suduko(su);
	sudukotosat(su, s);
	solver_set(s);

	destroy_suduko(su);
	free(su);
	writeCnf(s, filename2);
	destroy_solver(s);
	free(s);
}
void GENERATE_SUDUKO(char* filename) {
	int degree;
	printf("input the degree\n");
	scanf("%d", &degree);
	suduko_generater* generater = new_suduko_generater(degree);
	suduko* su = Generate_Suduko(generater);
	write_suduko(filename,su);
	destroy_suduko_generater(generater);
}


#endif
