// SAT 0223.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include <assert.h>
#include <time.h>

//************************************************************************************
//simple type



//************************************************************************************
//solver
static inline solver* solver_new() {
	solver* s = (solver*)malloc(sizeof(solver));
	assert(s != NULL);
	s->numofvar = 0;
	s->cap = 0;
	s->tail = 0;
	s->satisifable = false;
	s->cur_level = -1;

	vecp_new(&s->clauses);
	s->level_choice = NULL;
	s->assigns = NULL;
	s->counts = NULL;
	s->desicions = NULL;
	s->levels = NULL;

	return s;
}

static inline void solver_set(solver* s) {   //set the member to 0 or l_Udef
	s->cap = s->numofvar * 2 + 1;
	s->assigns = (lbool*)realloc(s->assigns, sizeof(lbool) * s->cap);
	s->level_choice = (lit*)realloc(s->level_choice, sizeof(lit) * s->cap);
	s->counts = (int*)realloc(s->counts, sizeof(int) * s->cap);
	s->desicions = (bool*)realloc(s->desicions, sizeof(bool) * s->cap);
	s->levels = (int*)realloc(s->levels, sizeof(int) * s->cap);

	int i = 0;
	for (i = 0; i < s->cap; i++) {
		s->assigns[i] = l_Undef;
		s->level_choice[i] = 0;
		s->desicions[i] = false;
		s->levels[i] = -1;
		s->counts[i] = 0;
	}
}
static inline void solver_addclause(solver* s, clause* c) {
	vecp_push(&s->clauses, c);
	s->tail++;    //the index after last point
}

static inline void destroy_solver(solver* s) {
	free(s->assigns);
	free(s->level_choice);
	vecp_delete(&s->clauses);
	free(s);
}
//***************************************************************************************************************

bool update_counts(solver* s)
{
	int i, j;
	clause* c;
	
	for (i = 1; i < s->cap; i++) {
		s->counts[i] = 0;
	}

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			// A true literal should not be in the working set of clauses!
			if (s->assigns[c->lits[j]] == l_True) return false;
			else if (s->assigns[c->lits[j]] == l_Undef) // Only count if not False
				s->counts[c->lits[j]]++;
		}
	}
	return true;
}

lit make_decision(solver* s)
{
	int i, maxval;
	lit maxlit;
	if (!update_counts(s))
		fprintf(stderr, "ERROR! Failed to update literal counts at level %d\n", s->cur_level),
		exit(1);
	maxval = -1;
	maxlit = -1;
	for (i = 1; i < s->cap; i++) {
		if (s->counts[i] > maxval) {
			maxval = s->counts[i];
			maxlit = i;
		}
	}
	if (maxval == 0 || s->assigns[maxlit] == l_False)
		fprintf(stderr, "ERROR! make_decision failed to find a lit that exists and isn't false!\n"),
		exit(1);

	return maxlit;

}

bool propagate_decision(solver* s, lit decision, bool new_level) {
	bool no_conflict = true;
	int i, j, false_count;
	clause* c;


	if (new_level) {
		s->cur_level++;
		s->level_choice[s->cur_level] = decision;
		s->desicions[decision] = true;  // only change 'decisions' on level decisions.
	}
	s->levels[decision] = s->cur_level;
	s->assigns[decision] = l_True;
	s->assigns[lit_neg(decision)] = l_False;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			if (s->assigns[c->lits[j]] == l_False) {
				false_count++;
			}
			else if (s->assigns[c->lits[j]] == l_True) {
				c->level_sat = s->cur_level;
				if (s->tail == 1) {
					s->tail--;
					s->satisifable = true;
					return true;
				}
				vecp_begin(&s->clauses)[i] = vecp_begin(&s->clauses)[--s->tail];
				vecp_begin(&s->clauses)[s->tail] = c;
				i = i--; // be sure to check the current i again - it isn't the same one it was!
				break;
			}
			if (false_count == clause_size(c)) {
				no_conflict = false; //Conflict found!
			}
		}
	}
	return no_conflict;
}

// returns the level_choice of the level backtracked to


lit backtrack_once(solver* s) {
	int i;
	clause* c;

	for (i = 1; i < s->cap; i++) {
		if (s->levels[i] == s->cur_level) {
			s->assigns[i] = l_Undef;
			s->assigns[lit_neg(i)] = l_Undef;
			s->levels[i] = -1;
		}
	}
	for (i = s->tail; i < vecp_size(&s->clauses); i++) {
		c = vecp_begin(&s->clauses)[i];
		if (c->level_sat == s->cur_level) {
			c->level_sat = -1;
			s->tail++;
		}
		else break;
	}

	return s->level_choice[s->cur_level--];

}

// returns true if backtrack worked, false if top of tree is hit (UNSATISFIABLE)
bool backtrack(solver* s, lit* decision) {
	// CONFLICT FOUND
	if (s->cur_level == 0 && s->desicions[lit_neg(s->level_choice[0])] == true) return false; //UNSATISFIABLE (boundary case)
	lit lev_choice = backtrack_once(s);
	while (s->desicions[lit_neg(lev_choice)] == true && s->desicions[lev_choice] == true) {
		if (s->cur_level + 1 == 0) { return false; } //UNSATISFIABLE
		s->desicions[lit_neg(lev_choice)] = false;
		s->desicions[lev_choice] = false;
		lev_choice = backtrack_once(s);
	}
	*decision = lit_neg(lev_choice);
	assert(s->desicions[lev_choice] == true);
	assert(s->desicions[lit_neg(lev_choice)] == false);
	return true;
}


// finds a unit clause if there is one.  Returns true if so, where unit_lit is the literal in that clause.
bool find_unit(solver* s, lit* unit_lit) {
	int i, j, false_count;
	clause* c;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			assert(s->assigns[c->lits[j]] != l_True);
			if (s->assigns[c->lits[j]] == l_False) false_count++;
			else *unit_lit = c->lits[j]; // If this is a unit clause, this will be the unit lit.
			if (j == clause_size(c) - 1 && false_count == clause_size(c) - 1) {
				return true; //UNIT CLAUSE!
			}
		}
	}
	return false; // NO UNIT CLAUSES
}

// returns false if conflict is found.  True if not, or solved.
bool propagate_units(solver* s) {
	lit unit_lit;
	while (find_unit(s, &unit_lit)) {
		if (!propagate_decision(s, unit_lit, false)) return false; // CONFLICT
		if (s->tail == 0) return true; // SATISFIED
	}
	return true;
}

bool solver_solve(solver* s) {
	lit decision;
	bool forced = false;

	while (true) {
		// pick a variable to decide on (based on counts)
		if (!forced) { decision = make_decision(s); }
		else forced = false;
		if (!propagate_decision(s, decision, true)) {
			// CONFLICT
			if (!backtrack(s, &decision)) return false;//UNSATISFIABLE
			else { //Backtrack worked, decision must be forced
				forced = true;
				continue;
			}
		}
		else {
			// NO CONFLICT
			if (s->satisifable) return true;
			if (!propagate_units(s)) {
				// CONFLICT
				if (!backtrack(s, &decision)) return false; //UNSATISFIABLE
				else {
					forced = true;
					continue;
				}
			}
			else {
				// NO CONFLICT
				if (s->satisifable) return true;
			}
		}
	}
	return true;
}


//***************************************************************************************************************
//read and write
void readClauseSet(solver* s,const char* filename) {
	FILE* fp;
	char line[256];
	size_t len = 0;

	fp = fopen(filename, "r");
	if (fp == NULL) printf("cannot open file"),exit(1);

	// define loop variables
	char* token;
	clause* currentClause = NULL;
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
			currentClause = new_clause(vecl_begin(&lits),vecl_begin(&lits) + lits.size);
			solver_addclause(s,currentClause);
		}
		vecl_delete(&lits);
	}
	fclose(fp);
}

void writeSolution(solver* s,const char* filename) {
	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}

	// iterate over valuation array to print the values of each literal
	int i; lit l;
	for (i = 1; i < s->numofvar + 1; i++) {
		l = tolit(i);
		fprintf(f, "%d: %d\n", i, s->assigns[l] < 0 ? -1 : 1);
	}

	fclose(f);
}


//***********************************************************************************************************************
//debug
void print_solver(solver* s) {
	printf("size:%d\n", s->numofvar);
	printf("cap:%d\n", s->cap);
	printf("cur_level:%d", s->cur_level);
	printf("satisfable:%d", s->satisifable);
	printf("tail:%d", s->tail);
}

void print_clause(clause* c) {
	int i;
	for (i = 0; i < c->size; i++) {
		printf("%d ", c->lits[i]);
	}
}

int main() {
	clock_t start, stop;
	solver* s = solver_new();
	char filename1[100];
	char filename2[100];

	printf("input:");
	scanf("%s", filename1);
	printf("output:");
	scanf("%s", filename2);
	readClauseSet(s,filename1);
	
	start = clock();

	solver_set(s);
	if (solver_solve(s)) {
		printf("OK\n");
	}
	else {
		printf("NOT OK\n");
	}

	stop = clock();
	double duration = (double)((stop - start) / CLK_TCK);
	printf("%.100lf\n",duration);

	writeSolution(s, filename2);

	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
