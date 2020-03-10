#ifndef dpll_plus_h
#define dpll_plus_h

#include "FileIO.h"
#include "new-dpll.h"

bool is_uniquesolution(solver* s) {
	int i; int mark = true; lit l;
	for (i = 1; i <= s->numofvar; i++) {
		l = tolit(i);
		if (s->valuation[l] == l_Undef) {
			mark = false;
			break;
		}
	}
	if (!mark) {
		return false;
	}
	else {
		return true;
	}
}

void writeSolution_plus(solver* s, const char* filename) {

	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	int i; lit l;
	//if satisfiable
	if (s->satisifable == true) {
		if (is_uniquesolution(s)) {
			fprintf(f, "s 1\n");
		}
		else {
			fprintf(f, "s -1\n");
		}
		fprintf(f, "v ");
		for (i = 1; i < s->numofvar + 1; i++) {
			l = tolit(i);
			if (s->valuation[l] < 0) {
				fprintf(f, "-%d ", i);
			}
			else if (s->valuation[l] == 0) {
				fprintf(f, "?%d ", i);
			}
			else {
				fprintf(f, "%d ", i);
			}
		}
		fprintf(f, "\n");
		fprintf(f, "t %20lf", s->time * 1000);
	}
	else {   //if not satisfiable
		fprintf(f, "s 0\n");
		fprintf(f, "t %20lf", s->time * 1000);
	}
	fclose(f);
}


#endif
