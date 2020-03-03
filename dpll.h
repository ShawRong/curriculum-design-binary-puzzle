#ifndef dpll_h
#define dpll_h

#include "struct.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


bool propagate(solver* s, lit choice, bool new_level) {
	bool no_conflict = true;
	int i; int j; int false_count;
	clause* c;

	//update the info of literal
	if (new_level) {
		s->cur_level++;
		s->v_level[s->cur_level] = choice;
		s->mark[choice] = true;
	}
	s->level_v[choice] = s->cur_level;
	s->valuation[choice] = l_True;
	s->valuation[lit_neg(choice)] = l_False;

	//propagating
	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) {
				false_count = 0;
			}
			if (s->valuation[c->lits[j]] == l_False) {
				false_count++;
			}
			else if (s->valuation[c->lits[j]] == l_True) { //delete clause from clauses
				c->mark = s->cur_level;
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

/*	unit propagation algorithm
*	unit clause i.e. it contains only a single unassigned
*	literal
*/

/*
*	find the unit clause from the clauses and return the literal
*/

bool find_unit(solver* s, lit* unit_lit) {
	int i; int j; int false_count;
	clause* c;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
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

/*	unit propagation algorithm
*	if it return false--->can not continue
*	if it return true--->success
*/
bool unit_propagate(solver* s) {
	lit l;
	while (find_unit(s, &l)) {
		if (!propagate(s, l, false)) {
			return false;
		}
		if (s->tail == 0) {
			return true;
		}
	}
	return true;
}


bool recount(solver* s){ //recount the literals in the clauses
	int i;int j; clause* c;

	for (i = 1; i < s->cap; i++) {
		s->counts[i] = 0;
	}

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {

			if (s->valuation[c->lits[j]] == l_True) {
				return false; 
			} else if (s->valuation[c->lits[j]] == l_Undef) {
				s->counts[c->lits[j]]++;
			}

		}
	}
	
	return true;
}

lit choosev(solver * s) {//choose the most common literal according to the counts
	int i; int max;
	lit maxlit;
	if (!recount(s)) {
		exit(1);
	}
	max = -1;
	maxlit = -1;
	for (i = 1; i < s->cap; i++) {
		if (s->counts[i] > max) {
			max = s->counts[i];
			maxlit = i;
		}
	}
	if (max == 0 || s->valuation[maxlit] == l_False) {
		exit(1);
	}

	return maxlit;

}

lit back(solver * s) {
	int i;
	clause* c;

	for (i = 1; i < s->cap; i++) { //reset the literals
		if (s->level_v[i] == s->cur_level) {
			s->valuation[i] = l_Undef;
			s->valuation[lit_neg(i)] = l_Undef;
			s->level_v[i] = -1;
		}
	}

	for (i = s->tail; i < vecp_size(&s->clauses); i++) { //reset the clauses
		c = vecp_begin(&s->clauses)[i];
		if (c->mark == s->cur_level) {
			c->mark = -1;
			s->tail++;
		}
		else break;
	}

	return s->v_level[s->cur_level--];  //return the literal in this level

}


bool backtrack(solver * s, lit * decision) {

	if (s->cur_level == 0 && s->mark[lit_neg(s->v_level[0])] == true) {
		return false; 
	}//come to the root and return flase

	lit choice = back(s);

	while (s->mark[lit_neg(choice)] == true && s->mark[choice] == true) {
		if (s->cur_level == -1) {
			return false; 
		}
		s->mark[lit_neg(choice)] = false;  //reset the mark of choice
		s->mark[choice] = false;
		choice = back(s);
	}
	*decision = lit_neg(choice);

	return true;
}

bool solver_solve(solver * s) {
	lit choice;
	bool flag = false;

	while (true) {

		//make choice and if flag = true, jump to next step
		if (!flag) {
			choice = choosev(s); 
		} else {
			flag = false;
		}

		

		//propagate
		if (!propagate(s, choice, true)) {

			//backtrace
			if (!backtrack(s, &choice)) {//backtrace to the root and return false 
				return false; 
			} else { // if backtrace is worked the choice will be changed. change to it's neg
				flag = true;
				continue;
			}
		} else {
			//go further
			if (s->satisifable) { //judge if satisfied after propagating
				return true; 
			}
			if (!unit_propagate(s)) {  //delete unit clauses
				//backtrace
				if (!backtrack(s, &choice)) {//backtrace to the root and return false
					return false; 
				} else {// if backtrace is worked the choice will be changed. change to it's neg
					flag = true;
					continue;
				}
			} else {
				if (s->satisifable) { //judge if satisfied after propagating
					return true; 
				}
			}
		}
	}
	return true;
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
		printf("%d ", lit_val(c->lits[i]));
	}
}

#endif

