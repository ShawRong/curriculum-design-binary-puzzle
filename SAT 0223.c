#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "extension.h"
#include "dpll-plus.h"
#include "createsuduko.h"

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
		printf("		3.GENERATE SUDUKO RANDOMLY \n");
		printf("	0.Exit\n");
		printf("-------------------------------------------------\n");
		printf("    请选择你的操作[0~3]:");
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
				getchar(); getchar();
				break;
			case 3:
				printf("output:");
				scanf("%s", filename1);
				GENERATE_SUDUKO(filename1);
				getchar(); getchar();
				break;
			case 0:
				break;
		}
	}
	/*
	lit l; int v; int* array;
	l = additional_bool(new_addition(1, 15, 16, -1, -1), 16);
	array = (int*)malloc(sizeof(int) * 5);
	array =  new_addition(1, 15, 16, -1, -1);
	v = lit_val(l);
	printf("%d\n", v);
	int i = 0;
	for (i = 0; i < 5; i++) {
		printf("%d ", array[i]);
	}*/
	


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
	if (is_uniquesolution(s)) {
		printf("it is unique solution");
	}
	else {
		printf("it isn't unique solution");
	}
	writeSolution_plus(s, filename2);
	*/
	//suduko
	

	/*******************************************************************/
	
	//明天就把DEBUG部分拿到main函数里运行看看是哪里出了问题包括las vegas部分
	/*******************************************************************/
	/*sudukotosat(su,s);
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
	free(s);
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
