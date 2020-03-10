#ifndef dpll_h
#define dpll_h

#include "struct.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

/********************/
void print_clause(clause* c) {
	int i;
	for (i = 0; i < c->size; i++) {
		printf("%d ", lit_val(c->lits[i]));
	}
}
/*******************/

//********************************************************************************************************
//****************************************MEMORY*****************************************************
typedef struct memory_t {
	int cur_level;
	bool* mark;  //mark the literal which was used to simplify the cnf true or false
	lit* v_level;  //  return the literal l in level i was chose
	int* level_v; // return the level i where the literal l was chose
}memory;

memory* new_memory(solver* s) {
	memory* itr = (memory*)malloc(sizeof(memory));
	itr->cur_level = -1;
	itr->level_v = (int*)malloc(sizeof(int) * s->cap);
	itr->v_level = (lit*)malloc(sizeof(lit) * s->cap);
	itr->mark = (bool*)malloc(sizeof(bool) * s->cap);
	int i = 0;
	for (i = 0; i < s->cap; i++) {
		itr->v_level[i] = 0;
		itr->level_v[i] = -1;
		itr->mark[i] = false;
	}
	return itr;
}

void destroy_memory(memory* memo) {
	free(memo->v_level);
	free(memo->level_v);
	free(memo->mark);
	free(memo);
}

//********************************************************************************************************


bool propagate(solver* s, lit choice, bool unit,memory* memo) {
	bool no_conflict = true;
	int i; int j; int false_count;
	clause* c;

	//update the info of literal
	if (!unit) {
		memo->v_level[memo->cur_level] = choice;
		memo->mark[choice] = true;
	}
	memo->level_v[choice] = memo->cur_level;

	s->valuation[choice] = l_True;
	s->valuation[lit_neg(choice)] = l_False;

	//propagating
	for (i = 0; i < s->tail; i++) {
		c = (clause*)vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) {
				false_count = 0;
			}
			if (s->valuation[c->lits[j]] == l_False) {
				false_count++;
			}
			else if (s->valuation[c->lits[j]] == l_True) { //delete clause from clauses
				c->mark = memo->cur_level;
				if (s->tail == 1) {
					s->tail--;
					s->satisifable = true;
					return true;
				}
				vecp_begin(&s->clauses)[i] = vecp_begin(&s->clauses)[--s->tail];
				vecp_begin(&s->clauses)[s->tail] = c;
				i = i--;
				break;
			}
			if (false_count == clause_size(c)) { //check false count
				no_conflict = false;
			}
		}
	}
	return no_conflict;
}

bool find_unit(solver* s, lit* unit_lit) {
	int i; int j; int false_count;
	clause* c;

	for (i = 0; i < s->tail; i++) {
		c = (clause*)vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) {
				false_count = 0;
			}
			if (s->valuation[c->lits[j]] == l_False) {
				false_count++;
			}
			else {
				*unit_lit = c->lits[j];
			}
			if (j == clause_size(c) - 1 && false_count == clause_size(c) - 1) {
				return true;
			}
		}
	}
	return false;
}

bool recount(solver* s) { //recount the literals in the clauses
	int i; int j; clause* c;

	for (i = 1; i < s->cap; i++) {
		s->record[i] = 0;
	}

	for (i = 0; i < s->tail; i++) {
		c = (clause*)vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {

			if (s->valuation[c->lits[j]] == l_True) {
				return false;
			}
			else if (s->valuation[c->lits[j]] == l_Undef) {
				s->record[c->lits[j]]++;
			}

		}
	}

	return true;
}

lit choosev_MODE(solver* s) {//choose the most common literal according to the counts
	int i; int max;
	lit maxlit;
	if (!recount(s)) {
		exit(1);
	}
	max = -1;
	maxlit = -1;
	for (i = 1; i < s->cap; i++) {
		if (s->record[i] > max) {
			max = s->record[i];
			maxlit = i;
		}
	}
	if (max == 0 || s->valuation[maxlit] == l_False) {
		exit(1);
	}

	return maxlit;

}

/*
lit choosev_PURE(solver* s) {//choose the pure literal
	int i; int j;
	lit purelit; bool mark = true;
	
	//update record
	if (!recount(s)) {
		exit(1);
	}
	lit l; clause* c; lit save;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			l = clause_begin(c)[j];
			if (s->valuation[l] != l_False) {
				if (mark) {
					save = l;
					mark = false;
				}
				if()
				return l;
			}
		}
	}
	return save;

}*/

lit choosev_RANDOM(solver* s) {//choose the pure literal
	lit l;
	clause* c;
	int i;int random;
	srand((unsigned)time(NULL));
	while (true) {
		random = rand() % (s->tail);
		c = vecp_begin(&s->clauses)[random];
		for (i = 0; i < clause_size(c); i++) {
			l = c->lits[i];
			if (s->valuation[l] != l_False) {
				return l;
			}
		}
	}
}

lit back(solver* s,memory* memo) {
	int i;
	clause* c;

	for (i = 1; i < s->cap; i++) { //reset the literals
		if (memo->level_v[i] == memo->cur_level) {
			s->valuation[i] = l_Undef;
			s->valuation[lit_neg(i)] = l_Undef;
			memo->level_v[i] = -1;
		}
	}

	for (i = s->tail; i < vecp_size(&s->clauses); i++) { //reset the clauses
		c = (clause*)vecp_begin(&s->clauses)[i];
		if (c->mark == memo->cur_level) {
			c->mark = -1;
			s->tail++;
		}
		else break;
	}

	return memo->v_level[memo->cur_level--];  //return the literal in this level

}


bool backtrack(solver* s,memory* memo) {

	if (memo->cur_level == 0 && memo->mark[lit_neg(memo->v_level[0])] == true) {
		return false;
	}//come to the root and return flase

	lit choice = back(s,memo);
	/*********
	printf("back choice:%d\n", lit_val(choice));
	/*********/

	while (memo->mark[lit_neg(choice)] == true && memo->mark[choice] == true) {
		if (memo->cur_level == -1) {
			return false;
		}
		memo->mark[lit_neg(choice)] = false;  //reset the mark of choice
		memo->mark[choice] = false;
		choice = back(s,memo);
		/*********
		printf("back choice:%d\n", lit_val(choice));
		/*********/
	}

	return true;
}



bool dpll(solver* s,memory* memo) {
	lit unitl;
	if (memo->cur_level == -1) {
		
	} else {
		while (find_unit(s, &unitl)) {
			/*********
			printf("unit:%d\n", lit_val(unitl));
			/*********/
			if (!propagate(s, unitl, true, memo)) {
				backtrack(s, memo);
				return false;
			}
			else if (s->satisifable == true) {
				return true;
			}
		}
	}
	/*********
	printf("level:%d\n", memo->cur_level);
	int temp = 0; clause* c;
	for (temp = 0; temp < s->tail; temp++) {
		c = vecp_begin(&s->clauses)[temp];
		print_clause(c);
		printf("\n");
	}
	/*********/
	
	lit nextl = choosev_MODE(s); //choose mode
	memo->cur_level++;
	/*********
	printf("p choice:%d\n", lit_val(nextl));
	/*********/
	if (!propagate(s, nextl, false, memo)) {
		backtrack(s, memo);
		return false;
	}
	else if (s->satisifable == true) {
		return true;
	}
	
	if (dpll(s,memo)) {
		return true;
	}

	nextl = lit_neg(nextl);
	memo->cur_level++;
	/*********
	printf("n choice:%d\n", lit_val(nextl));
	/*********/
	if (!propagate(s, nextl, false, memo)) {
		backtrack(s, memo);
		return false;
	}
	else if (s->satisifable == true) {
		return true;
	}
	return dpll(s,memo);
}


bool solver_solve(solver* s) {
	memory* memo = new_memory(s);
	if (dpll(s,memo)) {
		destroy_memory(memo);
		return true;
	} else {
		destroy_memory(memo);
		return false;
	}
}




#endif
