#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

static void f1(int, int, int, int);
static void f2(void);

static jmp_buf jmpbuff;
static int globval;

int main(){
	int autoval;
	register int regval;
	volatile int volaval;
	static int statval;
	globval = 1; autoval=2; regval=3; volaval=4; statval=5;
	if(setjmp(jmpbuff) != 0){
		printf("after longjmp:\n");
		printf("globval=%d, autoval=%d, regval=%d, volaval=%d, statval=%d\n", globval, autoval, regval, volaval, statval);
		exit(0);
	}
	// 在 longjmp 之前 setjmp 之后改变值
	globval=95; autoval=96; regval=97; volaval=98; statval=99;
	f1(autoval, regval, volaval, statval);
	return 0;
}
static void f1(int i, int j, int k, int l){
	printf("in f1():\n");
	printf("globval=%d, autoval=%d, regval=%d, volaval=%d, statval=%d\n", globval, i, j, k, l);
	f2();
}
static void f2(){
	longjmp(jmpbuff, 1);
}
