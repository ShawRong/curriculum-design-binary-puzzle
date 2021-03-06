#ifndef FileIO_h
#define FileIO_h

#include "struct.h"
#include <string.h>
#include "dpll-plus.h""
#include "suduko.h"

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

void writeCnf(solver* s, const char* filename) {

	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	fprintf(f, "p cnf %d %d\n", s->numofvar, s->numofclause);
	int i; int j; clause* c;
	for (i = 0; i < vecp_size(&s->clauses); i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < c->size; j++) {
			fprintf(f, "%d ", lit_val(c->lits[j]));
		}
		fprintf(f, "0");
		fprintf(f,"\n");
	}
	fclose(f);
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
		if (is_uniquesolution(s)) {
			fprintf(f, "s 1\n");
		}
		else {                                    //?
			fprintf(f, "s -1\n");
		}
		fprintf(f, "v ");
		for (i = 1; i < s->numofvar + 1; i++) {
			l = tolit(i);
			if (s->valuation[l] < 0) {
				fprintf(f, "-%d \n", i);
			}
			else {
				fprintf(f, "%d \n", i);
			}
		}
		fprintf(f, "\n");
		fprintf(f,"t %lf", s->time);
	} else {   //if not satisfiable
		fprintf(f, "s 0\n");
		fprintf(f, "t %lf", s->time);
	}
	fclose(f);
}

void print_solution(const char* filename) {
	char line[256]; int mark;
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	char* token;
	while (fgets(line, sizeof(line), fp)) {
		if (line[0] == 's') {
			sscanf(line, "s %d",&mark);
			if (mark == 0) {
				continue;
			} else if (mark == -1) {
				printf("not unique solution\n");
			}
		}
		else if (line[0] == 't') {
			token = strtok(line, " ");
			double value = atof(token);
			printf("time: %1f", value);
		}
		else {
			token = strtok(line, " ");
			while (token != NULL) {
				int value = atoi(token);
				if (value < 0) {
					printf("%d: false\n",abs(value));
				} else if(value > 0){
					printf("%d: true\n",value);
				}
				token = strtok(NULL, " ");
			}
		}
	}
}

void write_suduko(const char* filename,suduko* su) {
	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	int i = 0; int v; int x; int y;
	fprintf(f, "d %d\n", su->degree);
	for (i = 0; i < su->degree * su->degree; i++) {
		v = toi(i);
		vtoxy(&x, &y,su->degree, v);
		if (su->space[toindex(x)][toindex(y)] == -1) {
			continue;
		}
		fprintf(f,"%d %d %d\n", x, y, su->space[toindex(x)][toindex(y)]);
	}
	fclose(f);
}


#endif
