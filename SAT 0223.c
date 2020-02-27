#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "struct.h"
#include "dpll.h"
#include "FileIO.h"
#include "suduko.h"


void SAT(char* filename1,char* filename2) {
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

	solver_set(s);

	read_suduko(su, filename1);
	print_suduko(su);
	sudukotosat(su, s);

	destroy_suduko(su);
	free(su);
	writeCnf(s, filename2);
	destroy_solver(s);
	free(s);
}

int main() {
	int op = 1; solver* s;
	suduko* su;
	char filename1[100];
	char filename2[100];
	char filename3[100];
	while (op) {
		system("cls");	printf("\n\n");
		printf("				SAT or Puzzle  \n");
		printf("-------------------------------------------------\n");
		printf("		1.SAT				2.Puzzle\n");
		printf("	0.Exit\n");
		printf("-------------------------------------------------\n");
		printf("    请选择你的操作[0~2]:");
		scanf("%d", &op);
		switch (op) {
			case 1:
				printf("input:");
				scanf("%s", filename1);
				printf("output:");
				scanf("%s", filename2);
				SAT(filename1, filename2);
				getchar(); getchar();
				break;
			case 2:
				printf("input the suduko:");
				scanf("%s", filename1);
				printf("output the cnf:");
				scanf("%s", filename2);
				SUDUKO(filename1, filename2);
				printf("output:");
				scanf("%s", filename3);
				SAT(filename2,filename3);
				/*
				solver* sol = solver_new();
				printf("output:");
				scanf("%s", filename2);
				readClauseSet(sol, "CNF.cnf");

				if (solver_solve(sol)) {
					printf("OK\n");
				}
				else {
					printf("NOT OK\n");
				}

				writeSolution(sol, filename2);
				print_solution(filename2);
				destroy_solver(sol);*/
				getchar(); getchar();
				break;
			case 0:
				break;
		}
	}



	/*
	solver* s = solver_new();
	char filename1[100];
	char filename2[100];

	printf("input:");
	scanf("%s", filename1);
	printf("output:");
	scanf("%s", filename2);
	readClauseSet(s,filename1);
	

	solver_set(s);
	if (solver_solve(s)) {
		printf("OK\n");
	}
	else {
		printf("NOT OK\n");
	}

	printf("%.100lf\n",s->time);

	writeSolution(s, filename2);
	
	//suduko
	/*
	solver* s = solver_new();
	solver_set(s);
	suduko* su = new_suduko();
	read_suduko(su, "suduko.txt");
	print_suduko(su);
	sudukotosat(su,s);
	int i = 0;
	for (i = 0; i < vecp_size(&s->clauses); i++) {
		clause* c = vecp_begin(&s->clauses)[i];
		print_clause_suduko(c,su);
		printf("\n");
	}
	printf("%d ", s->numofclause);
	printf("%d ", s->numofvar);
	destroy_suduko(su);
	writeCnf(s, "CNF.cnf");
	destroy_solver(s);
	free(s);*/
	/*
	solver* sol = solver_new();
	char filename1[100];
	char filename2[100];

	printf("input:");
	scanf("%s", filename1);
	printf("output:");
	scanf("%s", filename2);
	readClauseSet(sol, filename1);


	solver_set(sol);
	if (solver_solve(sol)) {
		printf("OK\n");
	}
	else {
		printf("NOT OK\n");
	}

	printf("%.100lf\n", sol->time);

	writeSolution(sol, filename2);

	/*
	lit l1 = additional_bool(278  ,8);
	lit l2 = additional_bool(15710,8);
	lit l3 = additional_bool(1571 ,8);
	printf("%d\n", tosequential(7, 8, 8));
	printf("278  :%d\n", lit_val(l1));
	printf("15710:%d\n", lit_val(l2));
	printf("1571 :%d\n", lit_val(l3));*/
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
