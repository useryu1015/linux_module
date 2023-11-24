#include <stdio.h>

void func(){
	int *x=(int*)malloc(10*sizeof(int));
	x[10]=0;
	x[2]=0;

    // free(x);
}

int main() 
{
	func();

    printf("done\n");
	return 0;
}