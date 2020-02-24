//#ifdef struct_h
//#define struct_h

#include <stdlib.h>
#include <stdio.h> 
#include<math.h>
#include <assert.h>
#include <stdbool.h>
//********************************************************************************************************
// simple type to mark the assignment of literals
typedef char lbool;
static const lbool l_Undef = 0;
static const lbool l_True = 1;
static const lbool l_False = -1;

//********************************************************************************************************
// Literal
typedef int lit;

static inline lit tolit(int value) {
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
// clause
typedef struct Clause {
	lit* lits;
	int size;
	int level_sat;
}clause;

static inline int clause_size(clause * c){ 
	return c->size; 
}
static inline int clause_level(clause * c) { 
	return c->level_sat; 
};
static inline lit* clause_begin(clause * c) {
	return c->lits; 
}

static clause* new_clause(lit * begin, lit * end) {
	clause* c;

	c = (clause*)malloc(sizeof(clause));
	assert(c != NULL);
	c->size = end - begin;
	c->lits = (lit*)malloc(sizeof(lit) * c->size);
	assert(c->lits != NULL);
	c->level_sat = -1;
	int i;
	for (i = 0; i < c->size; i++)
		c->lits[i] = begin[i];

	return c;
}

//********************************************************************************************************
//vector of pointer
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
// solver representation
typedef struct Solver {
	int numofvar; // num of var
	int numofclause;  //num of clause
	int cap;  // = numofvar * 2 + 1

	vecp clauses;  //the vector of clause
	int tail; //index of the end of clauses
	
	int cur_level; //level in the tree
	bool satisifable; //true = satisifable
	bool* mark;  //mark the unit literal true or false
	lit* level_v;  // in level x choose level[x] to propagate
	lbool* valuation;  //the valuation of each literal
	int* levels; // which level that lit was assigned
	int* counts; //number of each literal

}solver;


//********************************************************************************************************
//#endif

