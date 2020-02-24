#ifndef struct_h
#define struct_h

#include <stdlib.h>
#include <stdio.h> 
#include<math.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
//********************************************************************************************************
// simple type to mark the assignment of literals
typedef char lbool;
static const lbool l_Undef = 0;
static const lbool l_True = 1;
static const lbool l_False = -1;

//********************************************************************************************************
// Literal
/*========================================================================================================
In this structure, the lit and the value obey the following rule to translate which aims to simply the 
operation in the sequential vector. Because array[-1] is unfeasiable in C language.
Rule:
		if the literal is like 20, it will be transformed into 39 (20*2 - 1)
		if the literal is like -20, it will be transformed into 40 (20*2)
		if the literal is 0, it will be transformed into 0

Addition: if there was 100 variable number, the capacity should be 2 * 100 + 1
=========================================================================================================*/
typedef int lit;

static inline lit tolit(int value) {  // translate lit
	if (value == 0) {
		return 0;
	}
	if (value < 0) {
		return abs(value) * 2;
	}
	else {
		return value * 2 - 1;
	}
};

static inline lit lit_neg(lit l) {
	if (l == 0) return 0;
	if (l % 2 == 0) {
		return l - 1;
	}
	else {
		return l + 1;
	}
}
static inline lit lit_sign(lit l) {
	if (l == 0) return 1;
	if (l % 2 == 0) {
		return -1;
	}
	else {
		return 1;
	}
}
static inline int lit_val(lit l) {
	if (l == 0) return 0;
	if (l % 2 == 0) {
		return -1 * l / 2;
	}
	else {
		return (l + 1) / 2;
	}
}
//********************************************************************************************************
// Clause
/*========================================================================================================
Clause lits is a pointer to the sequential vector of literals
Cluase size is the size of the sqential vector of literals
Clause mark is used to mark in which level it was deleted and was used when backtracing
========================================================================================================*/
typedef struct Clause {
	lit* lits;
	int size;
	int mark;
}clause;

static inline int clause_size(clause * c){ 
	return c->size; 
}
static inline int clause_level(clause * c) { 
	return c->mark; 
};
static inline lit* clause_begin(clause * c) {
	return c->lits; 
}

static clause* new_clause(lit * begin, lit * end) {
	clause* c;

	c = (clause*)malloc(sizeof(clause));
	c->size = end - begin;
	c->lits = (lit*)malloc(sizeof(lit) * c->size);
	c->mark = -1;
	int i;
	for (i = 0; i < c->size; i++)
		c->lits[i] = begin[i];

	return c;
}

//********************************************************************************************************
//vector of pointer
/*========================================================================================================
Vector of pointer is a imitation of c++ vector.
And ptr points to a sequential list
the initial capicity is 4 pointers. When it was full, it will expands to 2 * capicity + 1;
========================================================================================================*/
typedef struct vecp_t {
	int size;
	int cap;
	void** ptr;
}vecp;

static inline void vecp_new(vecp * v) {
	v->size = 0;
	v->cap = 4;
	v->ptr = (void**)malloc(sizeof(void*) * v->cap);
}

static inline void   vecp_delete(vecp * v) { free(v->ptr); }
static inline void** vecp_begin(vecp * v) { return v->ptr; }
static inline int    vecp_size(vecp * v) { return v->size; }
static inline void   vecp_push(vecp * v, void* e)
{
	if (v->size == v->cap) {
		int newsize = v->cap * 2 + 1;
		v->ptr = (void**)realloc(v->ptr, sizeof(void*) * newsize);
		v->cap = newsize;
	}
	v->ptr[v->size++] = e;
}
//********************************************************************************************************
//vector of lit
/*========================================================================================================
Vector of lit is a imitation of c++ vector
And ptr points to a sequential list
the initial capicity is 4 lits. When it was full, it will expands to 2 * capicity + 1;
========================================================================================================*/
typedef struct vecl_t {
	int    size;
	int    cap;
	lit* ptr;
}vecl;


static inline void vecl_new(vecl *v) {
	v->size = 0;
	v->cap = 4;
	v->ptr = (lit*)malloc(sizeof(lit) * v->cap);
}

static inline void   vecl_delete(vecl* v) { free(v->ptr); }
static inline lit* vecl_begin(vecl* v) { return v->ptr; }
static inline int    vecl_size(vecl* v) { return v->size; }
static inline void   vecl_push(vecl* v, lit e)
{
	if (v->size == v->cap) {
		int newsize = v->cap * 2 + 1;
		v->ptr = (lit*)realloc(v->ptr, sizeof(lit) * newsize);
		v->cap = newsize;
	}
	v->ptr[v->size++] = e;
}

//********************************************************************************************************
// Solver
/*========================================================================================================
	Create a structure to save some information which was useful in dpll.
========================================================================================================*/
typedef struct Solver {
	int numofvar; // num of var
	int numofclause;  //num of clause
	int cap;  // = numofvar * 2 + 1

	clock_t start, stop;  // to count time when the solver_solve begin
	double time;

	vecp clauses;  //the vector of clause
	int tail; //index of the end of clauses
	
	int cur_level; //level in the tree
	bool satisifable; //true = satisifable
	
	bool* mark;  //mark the literal which was used to simplify the cnf true or false

	lit* v_level;  //  return the literal l in level i was choosed
	int* level_v; // return the level i where the literal l was choosed
	
	lbool* valuation;  //the valuation of each literal
	
	int* counts; //number of each literal

}solver;

static inline solver* solver_new() {
	solver* s = (solver*)malloc(sizeof(solver));
	assert(s != NULL);
	s->numofvar = 0;
	s->cap = 0;
	s->tail = 0;
	s->satisifable = false;
	s->cur_level = -1;
	s->time = 0;

	vecp_new(&s->clauses);
	s->v_level = NULL;
	s->valuation = NULL;
	s->counts = NULL;
	s->mark = NULL;
	s->level_v = NULL;

	return s;
}

static inline void solver_set(solver * s) {   //set the member to 0 or l_Udef
	s->cap = s->numofvar * 2 + 1;
	s->valuation = (lbool*)realloc(s->valuation, sizeof(lbool) * s->cap);
	s->v_level = (lit*)realloc(s->v_level, sizeof(lit) * s->cap);
	s->counts = (int*)realloc(s->counts, sizeof(int) * s->cap);
	s->mark = (bool*)realloc(s->mark, sizeof(bool) * s->cap);
	s->level_v = (int*)realloc(s->level_v, sizeof(int) * s->cap);

	int i = 0;
	for (i = 0; i < s->cap; i++) {
		s->valuation[i] = l_Undef;
		s->v_level[i] = 0;
		s->mark[i] = false;
		s->level_v[i] = -1;
		s->counts[i] = 0;
	}
}
static inline void solver_addclause(solver * s, clause * c) {
	vecp_push(&s->clauses, c);
	s->tail++;    //the index after last point
}

// Function to count time
static inline void solver_counttime_begin(solver* s) {
	s->start = clock();
}
static inline void solver_counttime_finish(solver* s) {
	s->stop = clock();
	s->time = (double)((s->stop - s->start) / CLK_TCK);
}

static inline void destroy_solver(solver * s) {
	free(s->valuation);
	free(s->v_level);
	vecp_delete(&s->clauses);
	free(s);
}
//********************************************************************************************************

#endif

