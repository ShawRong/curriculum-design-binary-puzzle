#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "struct.h"
#include "dpll.h"
#include "FileIO.h"


int main() {
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
