//
// Created by ziv on 8/11/15.
//


#include <stdio.h>
#include <stdlib.h>
#include "MyString.h"
#include "mystring_demo.h"

int main() {

	test1();
	test2();

	char *tempString = malloc(sizeof(char));
	MyString *stringA = myStringAlloc();
	MyString *stringB = myStringAlloc();



	printf("Gimme a String\n");
	scanf (" %500[^\n]", tempString);

	myStringSetFromCString(stringA,tempString);
	printf("Gimme another String\n");
	scanf (" %500[^\n]", tempString);

	myStringSetFromCString(stringB,tempString);

	int result = myStringCompare(stringA,stringB);
	if (result > 0) myStringSwap(stringA,stringB);
	printf("%s is smaller than %s\n",myStringToCString(stringA),myStringToCString(stringB));


}