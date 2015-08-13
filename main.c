#include <stdio.h>
#include "mystring_demo.h"
#include "MyString.h"

int main2()
{


	test1();
	test2();

	MyString* arr[5];
	arr[0] = myStringAlloc();
	arr[1] = myStringAlloc();
	arr[2] = myStringAlloc();
	arr[3] = myStringAlloc();
	arr[4] = myStringAlloc();
	myStringSetFromCString(&arr[0],"Fuck this FUCKIN FUCK");
	myStringSetFromCString(&arr[1],"Oh My GOD i HATE C");
	myStringSetFromCString(&arr[2],"Fuck this So Annoying");
	myStringSetFromCString(&arr[3],"Fuck I cant stand this");
	myStringSetFromCString(&arr[4],"Fuck me.");

	printf("%lu\n", myStringMemUsage(arr[0]));

	printf("comparing 2 strings: %d\n",myStringCompare(arr[2],arr[4]));
	for (int i = 0; i < 5; ++i)
	{
		char* curString = myStringToCString(arr[i]);
		printf("string num %i is: %s\n",i,curString);
	}

	myStringSort(arr,5);

	for (int i = 0; i < 5; ++i)
	{
		char* curString = myStringToCString(arr[i]);
		printf("string num %i is: %s\n",i,curString);
	}

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




	return 0;
}

