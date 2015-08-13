//
// Created by ziv on 8/11/15.
//


#include <stdio.h>
#include <stdlib.h>
#include "MyString.h"

/**
 * this is a main method
 */
int main()
{


	char *tempString = malloc(501 * sizeof(char));
	MyString *stringA = myStringAlloc();
	MyString *stringB = myStringAlloc();


	printf("Gimme a String\n");
	scanf(" %500[^\n]", tempString);

	myStringSetFromCString(stringA, tempString);
	free(tempString);
	tempString = malloc(501 * sizeof(char));
	printf("Gimme another String\n");
	scanf(" %500[^\n]", tempString);

	myStringSetFromCString(stringB, tempString);

	int result = myStringCompare(stringA, stringB);
	if (result > 0)
	{
		myStringSwap(stringA, stringB);
	}
	char *cstringA = myStringToCString(stringA);
	char *cstringB = myStringToCString(stringB);
	printf("%s is smaller than %s\n", cstringA, cstringB);

	free(tempString);
	free(cstringA);
	free(cstringB);
	myStringFree(stringA);
	myStringFree(stringB);

	return 0;


}