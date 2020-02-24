#ifndef FileIO_h
#define FileIO_h

#include "struct.h"
#include <string.h>

//***************************************************************************************************************
//read and write
void readClauseSet(solver* s, const char* filename) {
	FILE* fp;
	char line[256];
	size_t len = 0;

	fp = fopen(filename, "r");
	if (fp == NULL) printf("cannot open file"), exit(1);

	// define loop variables
	char* token;
	clause * currentClause = NULL;
	lit currentLiteral;
	//lit lits[1000];   //it need to be change to vec in later;

	while (fgets(line, sizeof(line), fp)) {
		vecl lits; vecl_new(&lits);
		// ignore comment lines
		if (line[0] == 'c') continue;
		// this line is metadata information
		if (line[0] == 'p') {
			sscanf(line, "p cnf %d %d", &s->numofvar, &s->numofclause);
		}
		else {
			// split the line by the space characted and parse integers as literals
			int i = 0;
			token = strtok(line, " ");
			while (token != NULL) {
				int value = atoi(token);
				currentLiteral = tolit(value);
				if (currentLiteral != 0) {
					vecl_push(&lits, currentLiteral);
				}
				token = strtok(NULL, " ");
			}
			currentClause = new_clause(vecl_begin(&lits), vecl_begin(&lits) + lits.size);
			solver_addclause(s, currentClause);
		}
		vecl_delete(&lits);
	}
	fclose(fp);
}

void writeSolution(solver * s, const char* filename) {
	
	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	int i; lit l;
	//if satisfiable
	if (s->satisifable == true) {
		fprintf(f,"s 1\n");
		fprintf(f, "v ");
		for (i = 1; i < s->numofvar + 1; i++) {
			l = tolit(i);
			if (s->valuation[l] < 0) {
				fprintf(f, "-%d ", i);
			}
			else {
				fprintf(f, "%d ", i);
			}
		}
		fprintf(f, "\n");
		fprintf(f,"t %20lf", s->time * 1000);
	} else {   //if not satisfiable
		fprintf(f, "s 0\n");
		fprintf(f, "t %20lf", s->time * 1000);
	}
	fclose(f);
}



#endif
