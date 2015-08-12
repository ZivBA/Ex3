#include "MyString.h"
#include <stdlib.h>
#include <stdio.h>

/**
*
*/
void exitBad(int test)
{
    printf("Error in test %d.\n", test);
    exit(EXIT_FAILURE);
}



/**
 * Create a string, clone it and compare them
 */
void test1()
{
    // for the sake of clarity, we don't check for errors in this demonstration

    // this function call allocates a number and sets it to 0
    MyString * s1 = myStringAlloc();
	MyStringRetVal rVal = myStringSetFromCString(s1, "Hello world?!?");
	MyString * s2 = myStringClone(s1);
	
	int res = myStringCompare(s1, s2);
	
	if (res != 0) 
	{
        exitBad(1);
    
    }

    myStringFree(s1);
    myStringFree(s2);
}

/**
 * get two strings from ints, concatenate them and parse them back to an int
 */
void test2()
{
    int first = -105, second = 543;

    // This time, we check for errors.
    MyString * high = NULL;
    MyString * low = NULL;
    high = myStringAlloc();
    low = myStringAlloc();
	myStringSetFromInt(high, first);
	myStringSetFromInt(low, second);
    myStringCat(high, low);
	if (myStringToInt(high) != -105543)
	{
        exitBad(2);
    }


    // clean up.
    myStringFree(high);
    myStringFree(low);
}
