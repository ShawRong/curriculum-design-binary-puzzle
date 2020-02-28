#ifndef dpll_h
#define dpll_h

#include "struct.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


//***************************************************************************************************************
/*
/*
//it was used to count the number of the literal and save the information in sequential list counts
bool counts(solver* s) {
	int i, j;
	clause* c;
	for (i = 1; i < s->cap; i++) {
		s->counts[i] = 0;
	}
	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (s->valuation[c->lits[j]] == l_True) {
				return false;
			}
			else if (s->valuation[c->lits[j]] == l_Undef) {
				s->counts[c->lits[j]]++;
			}
		}
	}
	return true;
}

/*
//to find the most common literal
bool max_lit(solver* s,lit* max_l) {
	int i;
	int max = -1;
	*max_l = -1;
	for (i = 1; i < s->cap; i++) {
		if (s->counts[i] > max) {
			max = s->counts[i];
			*max_l = i;
		}
	}
	if (max == 0 || s->valuation[*max_l] == l_False) {
		return false;
	}
	return true;
}

//choose the most common literal in clauses to propagate
lit choosev(solver* s) {
	lit max_l = -1;
	if (!counts(s))
		printf("failed to count\n"), exit(1);
	
	if (!max_lit(s, &max_l)) {
		printf("failed to find\n"), exit(1);
	}
	return max_l;
}
lit choosev(solver* s) {
	int i, maxval;
	lit maxlit;
	if (!counts(s))
		printf("Failed to update\n"), exit(1);
	maxval = -1;
	maxlit = -1;
	for (i = 1; i < s->cap; i++) {
		if (s->counts[i] > maxval) {
			maxval = s->counts[i];
			maxlit = i;
		}
	}
	if (maxval == 0 || s->valuation[maxlit] == l_False)
		printf("Failed to find a lit that exists and isn't false!\n"), exit(1);
	return maxlit;
}
/*
//delete clause when propagete
bool delete_clause_propagate(solver* s, clause* c,int* i) {
	c->mark = s->cur_level;

	if (s->tail == 1) {
		s->tail--;
		s->satisifable = true;
		return true;
	}

	vecp_begin(&s->clauses)[*i] = vecp_begin(&s->clauses)[--s->tail];
	vecp_begin(&s->clauses)[s->tail] = c;
	*i--;
	return false;
}

//
bool propagate_v(solver * s, lit v, bool new_level) {
	bool no_conflict = true;
	int i, j, false_count;
	clause* c;

	if (new_level) {  //if new level update the information
		s->cur_level++;
		s->v_level[s->cur_level] = v;
		s->mark[v] = true;  //v was used
	}

	s->level_v[v] = s->cur_level;

	s->valuation[v] = l_True;
	s->valuation[lit_neg(v)] = l_False;

	//clause traversal
	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;  //reset the false_count

			if (s->valuation[c->lits[j]] == l_False) {
				false_count++;
			} else if (s->valuation[c->lits[j]] == l_True) {
				//delete the clause
				if (delete_clause_propagate(s, c, &i)) {
					return true;
				}
				break;
			}
			if (false_count == clause_size(c)) {
				no_conflict = false; //conflict
			}
		}
	}
	return no_conflict;
}

bool propagate_v(solver* s, lit v, bool new_level) {
	bool no_conflict = true;
	int i, j, false_count;
	clause* c;


	if (new_level) {
		s->cur_level++;
		s->level_v[s->cur_level] = v;
		s->mark[v] = true;  // only change 'decisions' on level decisions.
	}
	s->level_v[v] = s->cur_level;
	s->valuation[v] = l_True;
	s->valuation[lit_neg(v)] = l_False;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			if (s->valuation[c->lits[j]] == l_False) {
				false_count++;
			}
			else if (s->valuation[c->lits[j]] == l_True) {
				c->mark = s->cur_level;
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
lit backtrack_level_choice(solver * s) {
	int i;
	clause* c;
	//backtrace the valuation
	for (i = 1; i < s->cap; i++) {
		if (s->level_v[i] == s->cur_level) {
			s->valuation[i] = l_Undef;
			s->valuation[lit_neg(i)] = l_Undef;
			s->level_v[i] = -1;
		}
	}
	//backtrack the clauses
	for (i = s->tail; i < vecp_size(&s->clauses); i++) {
		c = vecp_begin(&s->clauses)[i];
		if (c->mark == s->cur_level) {
			c->mark = -1;
			s->tail++;
		}
		else break;
	}

	return s->v_level[s->cur_level--];

}

// returns true if backtrack worked, false if top of tree is hit (UNSATISFIABLE)
bool backtrack(solver * s, lit * decision) {
	// CONFLICT FOUND
	if (s->cur_level == 0 && s->mark[lit_neg(s->v_level[0])] == true) { //v or !v is not work in the same time
		return false;
	}

	lit lev_choice = backtrack_level_choice(s);

	while (s->mark[lit_neg(lev_choice)] == true && s->mark[lev_choice] == true) {
		if (s->cur_level + 1 == 0) {
			return false;
		}
		s->mark[lit_neg(lev_choice)] = false;
		s->mark[lev_choice] = false;
		lev_choice = backtrack_level_choice(s);
	}

	*decision = lit_neg(lev_choice);
	return true;
}


// finds a unit clause if there is one.  Returns true if so, where unit_lit is the literal in that clause.
bool find_unit(solver * s, lit * unit_lit) {
	int i, j, false_count;
	clause* c;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			if (s->valuation[c->lits[j]] == l_False) false_count++;
			else *unit_lit = c->lits[j]; // If this is a unit clause, this will be the unit lit.
			if (j == clause_size(c) - 1 && false_count == clause_size(c) - 1) {
				return true; //UNIT CLAUSE!
			}
		}
	}
	return false; // NO UNIT CLAUSES
}

// returns false if conflict is found.  True if not, or solved.
bool propagate_units(solver * s) {
	lit unit_lit;
	while (find_unit(s, &unit_lit)) {
		if (!propagate_v(s, unit_lit, false)) return false; // CONFLICT
		if (s->tail == 0) return true; // SATISFIED
	}
	return true;
}

bool solver_solve(solver * s) {
	//count time
	solver_counttime_begin(s);

	lit v;
	bool flag = false;
	
	while (true) {
		if (!flag) {
			v = choosev(s);
		}
		else {
			flag = false;
		}
		if (!propagate_v(s, v, true)) {
			if (!backtrack(s, &v)) {
				return false;
			}
			else {
				flag = true;
				continue;
			}
		}
		else {
			if (s->satisifable) {
				return true;
			}
			if (!propagate_units(s)) {
				if (!backtrack(s, &v)) {
					return false;
				}
				else {
					flag = true;
					continue;
				}
			}
			else {
				if (s->satisifable) return true;
			}
		}
	}

	//counttime finish
	solver_counttime_finish(s);
	return true;
}*/
/*
bool counts(solver* s) {
	int i, j;
	clause* c;
	for (i = 1; i < s->cap; i++) {
		s->counts[i] = 0;
	}
	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (s->valuation[c->lits[j]] == l_True) return false;
			else if (s->valuation[c->lits[j]] == l_Undef)
				s->counts[c->lits[j]]++;
		}
	}
	return true;
}

lit choosev(solver * s) {
	int i, maxval;
	lit maxlit;
	if (!counts(s))
		printf("Failed to update\n"), exit(1);
	maxval = -1;
	maxlit = -1;
	for (i = 1; i < s->cap; i++) {
		if (s->counts[i] > maxval) {
			maxval = s->counts[i];
			maxlit = i;
		}
	}
	if (maxval == 0 || s->valuation[maxlit] == l_False)
		printf("Failed to find a lit that exists and isn't false!\n"), exit(1);
	return maxlit;
}

bool propagate_v(solver * s, lit v, bool new_level) {
	bool no_conflict = true;
	int i, j, false_count;
	clause* c;


	if (new_level) {
		s->cur_level++;
		s->level_v[s->cur_level] = v;
		s->mark[v] = true;  // only change 'decisions' on level decisions.
	}
	s->level_v[v] = s->cur_level;
	s->valuation[v] = l_True;
	s->valuation[lit_neg(v)] = l_False;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			if (s->valuation[c->lits[j]] == l_False) {
				false_count++;
			}
			else if (s->valuation[c->lits[j]] == l_True) {
				c->mark = s->cur_level;
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
lit backtrack_once(solver * s) {
	int i;
	clause* c;

	for (i = 1; i < s->cap; i++) {
		if (s->level_v[i] == s->cur_level) {
			s->valuation[i] = l_Undef;
			s->valuation[lit_neg(i)] = l_Undef;
			s->level_v[i] = -1;
		}
	}
	for (i = s->tail; i < vecp_size(&s->clauses); i++) {
		c = vecp_begin(&s->clauses)[i];
		if (c->mark == s->cur_level) {
			c->mark = -1;
			s->tail++;
		}
		else break;
	}

	return s->level_v[s->cur_level--];

}

// returns true if backtrack worked, false if top of tree is hit (UNSATISFIABLE)
bool backtrack(solver * s, lit * decision) {
	// CONFLICT FOUND
	if (s->cur_level == 0 && s->mark[lit_neg(s->level_v[0])] == true) return false; //UNSATISFIABLE (boundary case)
	lit lev_choice = backtrack_once(s);
	while (s->mark[lit_neg(lev_choice)] == true && s->mark[lev_choice] == true) {
		if (s->cur_level + 1 == 0) { return false; } //UNSATISFIABLE
		s->mark[lit_neg(lev_choice)] = false;
		s->mark[lev_choice] = false;
		lev_choice = backtrack_once(s);
	}
	*decision = lit_neg(lev_choice);
	assert(s->mark[lev_choice] == true);
	assert(s->mark[lit_neg(lev_choice)] == false);
	return true;
}


// finds a unit clause if there is one.  Returns true if so, where unit_lit is the literal in that clause.
bool find_unit(solver * s, lit * unit_lit) {
	int i, j, false_count;
	clause* c;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			assert(s->valuation[c->lits[j]] != l_True);
			if (s->valuation[c->lits[j]] == l_False) false_count++;
			else *unit_lit = c->lits[j]; // If this is a unit clause, this will be the unit lit.
			if (j == clause_size(c) - 1 && false_count == clause_size(c) - 1) {
				return true; //UNIT CLAUSE!
			}
		}
	}
	return false; // NO UNIT CLAUSES
}

// returns false if conflict is found.  True if not, or solved.
bool propagate_units(solver * s) {
	lit unit_lit;
	while (find_unit(s, &unit_lit)) {
		if (!propagate_v(s, unit_lit, false)) return false; // CONFLICT
		if (s->tail == 0) return true; // SATISFIED
	}
	return true;
}

bool solver_solve(solver * s) {
	lit decision;
	bool forced = false;

	while (true) {
		// pick a variable to decide on (based on counts)
		if (!forced) { decision = choosev(s); }
		else forced = false;
		if (!propagate_v(s, decision, true)) {
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
}*/

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
			if (s->valuation[c->lits[j]] == l_True) {
				return false; 
			}
			else if (s->valuation[c->lits[j]] == l_Undef) {
				s->counts[c->lits[j]]++;
			}

		}
	}
	
	return true;
}

lit make_decision(solver * s)
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
	if (maxval == 0 || s->valuation[maxlit] == l_False)
		fprintf(stderr, "ERROR! make_decision failed to find a lit that exists and isn't false!\n"),
		exit(1);

	return maxlit;

}

bool propagate_decision(solver * s, lit decision, bool new_level) {
	bool no_conflict = true;
	int i, j, false_count;
	clause* c;


	if (new_level) {
		s->cur_level++;
		s->v_level[s->cur_level] = decision;
		s->mark[decision] = true;  // only change 'decisions' on level decisions.
	}
	s->level_v[decision] = s->cur_level;
	s->valuation[decision] = l_True;
	s->valuation[lit_neg(decision)] = l_False;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			if (s->valuation[c->lits[j]] == l_False) {
				false_count++;
			}
			else if (s->valuation[c->lits[j]] == l_True) {
				c->mark = s->cur_level;
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


lit backtrack_once(solver * s) {
	int i;
	clause* c;

	for (i = 1; i < s->cap; i++) {
		if (s->level_v[i] == s->cur_level) {
			s->valuation[i] = l_Undef;
			s->valuation[lit_neg(i)] = l_Undef;
			s->level_v[i] = -1;
		}
	}
	for (i = s->tail; i < vecp_size(&s->clauses); i++) {
		c = vecp_begin(&s->clauses)[i];
		if (c->mark == s->cur_level) {
			c->mark = -1;
			s->tail++;
		}
		else break;
	}

	return s->v_level[s->cur_level--];

}

// returns true if backtrack worked, false if top of tree is hit (UNSATISFIABLE)
bool backtrack(solver * s, lit * decision) {
	// CONFLICT FOUND
	if (s->cur_level == 0 && s->mark[lit_neg(s->v_level[0])] == true) return false; //UNSATISFIABLE (boundary case)
	lit lev_choice = backtrack_once(s);
	while (s->mark[lit_neg(lev_choice)] == true && s->mark[lev_choice] == true) {
		if (s->cur_level + 1 == 0) { return false; } //UNSATISFIABLE
		s->mark[lit_neg(lev_choice)] = false;
		s->mark[lev_choice] = false;
		lev_choice = backtrack_once(s);
	}
	*decision = lit_neg(lev_choice);
	assert(s->mark[lev_choice] == true);
	assert(s->mark[lit_neg(lev_choice)] == false);
	return true;
}


// finds a unit clause if there is one.  Returns true if so, where unit_lit is the literal in that clause.
bool find_unit(solver * s, lit * unit_lit) {
	int i, j, false_count;
	clause* c;

	for (i = 0; i < s->tail; i++) {
		c = vecp_begin(&s->clauses)[i];
		for (j = 0; j < clause_size(c); j++) {
			if (j == 0) false_count = 0;
			assert(s->valuation[c->lits[j]] != l_True);
			if (s->valuation[c->lits[j]] == l_False) false_count++;
			else *unit_lit = c->lits[j]; // If this is a unit clause, this will be the unit lit.
			if (j == clause_size(c) - 1 && false_count == clause_size(c) - 1) {
				return true; //UNIT CLAUSE!
			}
		}
	}
	return false; // NO UNIT CLAUSES
}

// returns false if conflict is found.  True if not, or solved.
bool propagate_units(solver * s) {
	lit unit_lit;
	while (find_unit(s, &unit_lit)) {
		if (!propagate_decision(s, unit_lit, false)) return false; // CONFLICT
		if (s->tail == 0) return true; // SATISFIED
	}
	return true;
}

bool solver_solve(solver * s) {
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

