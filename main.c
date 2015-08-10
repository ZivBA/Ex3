#include <stdio.h>
#include <stdlib.h>
#include "mystring_demo.h"
#include "MyString.h"

int main()
{


	//test1();
	//test2();
/*

	FILE *f = fopen("test.out", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}


	const char *strA;
	const char *strB;
	//not code repetition this is not code repetition this is not code repetition this is not code repetition
	printf("Enter string A:\n");
	scanf ( "%500s", strA);
	printf("Enter string B:\n");
	scanf ( "%500s", strB);

	int result = myStringCompare(strA,strB);




	fclose(f);
*/

	//0 is 48 in ascii so thec number 1 is 1+48=49
#define INT_ASCII_DIFF 48
//max digits for long int =10 + sign
#define MAX_DIGITS_FOR_INT 11
#define DECIMAL_SYSTEM 10

	int n = 65345;
	int i;
	char *tempNum = malloc(MAX_DIGITS_FOR_INT * sizeof(char));

	for ( i = 0; n != 0; ++i)
	{
		tempNum[i] = (char) (n % 10 + INT_ASCII_DIFF);
		n /= DECIMAL_SYSTEM;
		printf("Current num =:%s\n",tempNum);
		printf("Current i =:%i\n",i);
	}
	for (int j = 0; j < i-1; ++j)
	{
		printf("Current num =:%s\n",tempNum);
		printf("Current j =:%i\n",j);
		char tempChar;

		tempChar = tempNum[j] ;
		tempNum[j] =  tempNum[i-j-1];
		tempNum[i-j-1] = tempChar;


	}





	return 0;
}

