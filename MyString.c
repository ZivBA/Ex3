//
// Created by ziv on 8/9/15.
//

/**
 *Answers to questions from exercise description:
 * ****  The struct that i chose and it's members: ****
 * See the struct defined below. it's members are:
 * char *actualString 	: a pointer to a C String. points to some memory location 
 * 						holding the first char of C string
 * int *refCounter : 	a pointer to some integer location, holding the number of references to the actual string.
 * int length : 		an actual int holding the length of the string.
 * 
 * i chose this struct to allow for reference counting as suggested by the bonus assignment instructions.
 * in this fashion, no MyString struct holds an actual string, rather a pointer to a pre-existing string 
 * in the memory. to copy from one MyString to another, a copy of the pointer address is sufficient (always O(1)).
 * to ensure all structs know how many references are made to a string, they all share the address of the counter as well.
 * the lenght is not shared since a pointer location is larger than an int, and requires less updates than a refCounter.
 * 
 * **** uses of malloc/realloc and ways to save on those ****
 * since i chose to use ref-counting this meant that memory allocations are made only when changing a string to a
 * new value, as opposed to an assignment from another string.
 * 
 * the places where i used malloc:
 * myStringAlloc() : 	 	obviously needed to allocate memory for a struct as well as for default string and recount
 * refReset() :			allocate a new integer for reference counting.
 * myStringFilter() :		allocate new memory segment for the filtered string.
 * myStringSetFromCString():	allocate memory for the size of a new string for the struct
 * myStringSetFromInt():	allocate memory for the new string
 * myStringToCString():		allocate memory for a copy of the existing string 
 * myStringCat():		allocate more memory for the concatenated string.
 * myStringCatTo():		allocate memory for the concated string
 *
 * we were asked to use malloc/realloc intelligently. for example, not to reallocate memory for a change of 1 byte.
 * however, the malloc implementation is already intelligent enough to not make changes if the resulting memory
 * block is the same size as the old memory block (i.e. changes that are less than 16 bytes).
 * "Section 7.20.3 of C99 states The pointer returned if the allocation succeeds is suitably aligned so that it may be
 * assigned to a pointer to any type of object."
 * and:
 * "C99 7.20.3.4 §4: The realloc function returns a pointer to the new object (which may have the same
 * value as a pointer to the old object)"
 *
 * thus, for changes smaller a 16 byte "word", realloc will not make any changes, and will return the same pointer
 * in O(1) efficiency, basically making any optimization attempts on my behalf rather pointless.
 *
 * **** special design decisions ****
 * in the customSort method, i created an inner method that is defined at runtime according to the input argument
 * of the outer method. customSort gets a comparator, and the inner method wraps customCompare by providing the 
 * comparator passed to customSort as an argument to customCompare. this wrapper method is then passed to qsort for 
 * array sorting.
 * 
 * i dont feel that i have written any specal algorithms worth mentioning, all helper methods i wrote were written
 * to avoid using methods from external libraries, except 'power' which was written since "pow" didnt work for me.
 * 
 **/


#include <stdlib.h>
#include <string.h>
#include "MyString.h"


#define MALLOC_ERROR "Error allocating memory\n"

//0 is 48 in ascii so thec number 1 is 1+48=49
#define INT_ASCII_DIFF 48
#define INT_ASCII_MAX 57
//max digits for long int =10 + sign
#define MAX_DIGITS_FOR_INT 11
#define DECIMAL_SYSTEM 10
#define NEGATIVE_SIGN '-'

// comparator GReater Than result
#define COMP_GRT 1
// comparator SMaller Than result
#define COMP_SMT -1
#define COMP_EQUAL 0
// true return value for the equal method
#define COMP_TRUE 1
// false return value for the equal method
#define COMP_FALSE 0
// returns the minimum of 2 int values.
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/*
 * MyString represents a manipulable string.
 * Implement reference counting for better memory consumption and improved performance.
 */
typedef struct _MyString
{
    int length;         // length of string
    int *refCount;      // pointer to an int holding the reference counter for this string.
    char *actualString; // pointer to a C string holding the actual string.

} MyString;


/**
 * @brief Allocates a new MyString and sets its value to "" (the empty string).
 * 			It is the caller's responsibility to free the returned MyString.
 *
 * RETURN VALUE:
 * @return a pointer to the new string, or NULL if the allocation failed.
 *
 * Time Complexity: given that malloc/realloc time complexity is non-deterministic, i'll treat it as O(MALL),
 * here and throughout this file.
 * Total complexity for myStringAlloc: O(1) + O(MALL).
 *
 */
MyString *myStringAlloc()
{

	MyString *temp = malloc(sizeof(MyString));
	temp->actualString = malloc(sizeof(char));
	*temp->actualString = '\0';
	temp->length = 0;
	temp->refCount = malloc(sizeof(int));
	*temp->refCount = 0;
	return temp;
}


/**
 * helper method to decrease the reference counter.
 */
inline static void refCountDecrement(MyString *str)
{
	*str->refCount -= 1;
}

/**
 * helper method to increase the reference counter.
 */
inline static void refCountIncrement(MyString *str)
{
	*str->refCount += 1;
}

/**
 * helper method to free a cString only iff it has no copies.
 * Time Complexity: O(1)
 */
static void freeCString(MyString *pString)
{
	if (*pString->refCount == 0)
	{
		free(pString->refCount);
		free(pString->actualString);
	}
	else
	{
		refCountDecrement(pString);
	}
}

/**
 * @brief **ref-Counting** Frees the memory and resources allocated to str.
 *
 * @param str the MyString to free.
 * If str is NULL, no operation is performed.
 * Time Complexity: I'm assuming that a FREE operation is O(1), so total complexity is O(1).
 */
void myStringFree(MyString *str)
{
	freeCString(str);
	free(str);
}

/**
 * helper method that checks if a MyString object is null, or if it's contents is.
 */
inline static bool isStringNull(const MyString *str)
{
	return (str == NULL || str->actualString == NULL);
}

/**
 * @brief   ** implementing O(1) string cloning by ref-counting.
 *          Allocates a new MyString with the same value as str. It is the caller's
 * 			responsibility to free the returned MyString.
 * @param str the MyString to clone.
 * RETURN VALUE:
 *   @return a pointer to the new string, or NULL if the allocation failed.
 *
 * Time Complexity: O(1) + O(MALL)
 */
MyString *myStringClone(const MyString *str)
{
	MyString *temp = myStringAlloc();

	//check if string to clone is non-null
	if (isStringNull(str))
	{
		return temp;
	}
	//else do reference copy for fields..
	temp->actualString = str->actualString;
	temp->length = str->length;
	temp->refCount = str->refCount;
	refCountIncrement(temp);
	return temp;
}


/**
 * helper method that reallocates memory for a new reference counter and sets it to zero.
 * O(1) + O(MALL)
 */
static void refReset(MyString *str)
{
	str->refCount = malloc(sizeof(int));
	*str->refCount = 0;
}

/**
 * @brief   doesnt copy value, just reference. - refCounting
 *          Sets the <s>value</s> reference of str to the <s>value</s> reference of other.
 * @param str the MyString to set
 * @param other the MyString to set from
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *  Time Complexity: O(1) + O(MALL)
 */
MyStringRetVal myStringSetFromMyString(MyString *str, const MyString *other)
{
	// check that the source string is non-null
	if (isStringNull(other))
	{
		return MYSTRING_ERROR;
	}

	//check and see if the old CString can be freed.
	freeCString(str);
	refReset(str);
	str->actualString = other->actualString;
	//re-point counter and increment.
	str->refCount = other->refCount;
	refCountIncrement(str);


	return MYSTRING_SUCCESS;
}

/**
 * helper method that checks if a cString is null;
 */
inline static bool cStringNullCheck(const char *cStringToCheck)
{
	return cStringToCheck == NULL;
}

/**
 * @brief filter the value of str acording to a filter.
 * 	remove from str all the occurrence of chars that are filtered by filt
 *	(i.e. filr(char)==true)
 * @param str the MyString to filter
 * @param filt the filter
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 * Time Complexity: standard case- O(n) + O(MALL) where n is the number of chars in string.
 *                  best case (null or empty string)- O(1) + O(MALL)
 **/
MyStringRetVal myStringFilter(MyString *str, bool (*filt)(const char *))
{
	// prep new string - alloc maximum size =old size, inject passing chars
	char *newString = malloc((str->length) * sizeof(char));

	// ensure malloc is successful.
	if (cStringNullCheck(newString))
	{
		printf(MALLOC_ERROR);
		return MYSTRING_ERROR;
	}

	int counter = 0;
	for (int i = 0; i < str->length; ++i)
	{
		const char *currentChar = &str->actualString[i];
		if (filt(currentChar))
		{
			newString[counter++] = *currentChar;
		}
	}

	// housekeeping: reallocate memory to minimize array size, free old Cstring
	// ensure realloc was successfull
	if (sizeof(realloc(newString, counter * sizeof(char))) != counter * sizeof(char))
	{
		printf(MALLOC_ERROR);
		return MYSTRING_ERROR;
	}

	freeCString(str);
	str->actualString = newString;
	refReset(str);

	return MYSTRING_SUCCESS;


}

/**
 * helper method, counts the number of chars in a string.
 * returns the... number of cars in a string.
 * accepts a..... string.
 */
static int cStringCheckLength(const char *cString)
{
	if (cString == NULL) return -1;
	int counter = 1;
	char curChar = *cString;
	while (curChar != '\0')
	{
		curChar = *(cString + (sizeof(char) * counter));
		counter++;
	}
	return counter;
}

/**
 * @brief Sets the <s>value</s> reference of str to the <s>value</s> reference of the given C string.
 * 			The given C string must be terminated by the null character.
 * 			Checking will not use a string without the null character.
 * 			*note:* it was not defined that the source Cstring is final. being const in the scope of this
 * 			method means that i return a struct with the same string as i got (no change in the scope) but
 * 			does not mean that it is not to be changed in other methods. i hope my understanding of the
 * 			const usage is correct in this regards.
 * @param str the MyString to set.
 * @param cString the C string to set from.
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *  Time Complexity: standard case: again, assuming all memory allocation/copying operations are roughly
 *  constant time, then O(1) + O(MALL).
 **/

MyStringRetVal myStringSetFromCString(MyString *str, const char *cString)
{
	//check that the string isn't null
	if (cStringNullCheck(cString)) return MYSTRING_ERROR;

	int length = cStringCheckLength(cString);

	char *copyOfCstring = malloc(length * sizeof(char));

	//check memory alloc
	if (copyOfCstring == NULL)
	{
		printf(MALLOC_ERROR);
		return MYSTRING_ERROR;
	}

	memcpy(copyOfCstring, cString, length * sizeof(char));
	copyOfCstring[length]='\0';
	//free old Cstring then reassign the pointer to new string.
	freeCString(str);
	str->actualString = copyOfCstring;
	refReset(str);

	str->length = length-1;
	return MYSTRING_SUCCESS;
}


/**
 * helper method that gets a cString and it's length, and reverses the order of the chars.
 * used for converting integer value to number string.
 * O(n/2)
 */
static char *reverseCstring(char *tempNum, int i)
{
	for (int j = 0; j <= i - j - 1; ++j)
	{

		char tempChar;

		if (tempNum[j] == NEGATIVE_SIGN) j++, i++; // if neg, shift one position right before reversing.
		tempChar = tempNum[j];
		tempNum[j] = tempNum[i - j - 1];
		tempNum[i - j - 1] = tempChar;

	}
	return tempNum;
}

/**
 * helper method that gets an integer and returns a pointer to a cString with that value.
 * O(n) + O(n/2) + O(MALL)
 */
static char *intToCString(int n)
{
	int i = 0;
	char *tempNum = malloc(MAX_DIGITS_FOR_INT * sizeof(char));

	if (cStringNullCheck(tempNum))
	{
		return NULL;
	}
	// check if the number is negative, adding the neg-sign to the string.
	if (n < 0)
	{
		tempNum[i] = NEGATIVE_SIGN;
		// switch to positive int to prevent calculation issues with mod/floor.
		n = abs(n);
		i++;
	}
	//iterate over digits by moding for the digit value and flooring to discard it.
	for (; n != 0 && i < MAX_DIGITS_FOR_INT; ++i)
	{
		tempNum[i] = (char) (n % DECIMAL_SYSTEM + INT_ASCII_DIFF);
		n /= DECIMAL_SYSTEM;

	}
	// create the actual string of length 'i'
	char *temp = realloc(tempNum, i * sizeof(char));
	if (*temp != *tempNum)
	{
		printf(MALLOC_ERROR);
		return NULL;
	}
	tempNum[i] = '\0';

	return reverseCstring(tempNum, i);
}

/**
 * @brief Sets the value of str to the value of the integer n.
 *	(i.e. if n=7 than str should contain ‘7’)
 * 	You are not allowed to use itoa (or a similar functions) here but must code your own conversion function.
 * @param str the MyString to set.
 * @param n the int to set from.
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 * 
 * Time Complexity: O(MALL) + O(n) where n is the length in digits of the int.
 * 
 */
MyStringRetVal myStringSetFromInt(MyString *str, int n)
{
	//free old string
	freeCString(str);
	// parse int to Cstring and assign reference to string, then zero the counter;
	str->actualString = (intToCString(n));
	//check that the procedure was succesfull
	if (str->actualString == NULL)
	{
		return MYSTRING_ERROR;
	}

	str->length = cStringCheckLength(str->actualString);
	refReset(str);
	return MYSTRING_SUCCESS;

}

/**
 * helper method to calculate power, since for some reason importing math.h didnt make pow() available.
 * complexity is o(factor), and bounded by 9 since that's the largest 10 base factor an integer supports.
 */
static double power(int base, int factor)
{
	int sum = 1;
	for (int i = 1; i < factor; ++i)
	{
		sum *= base;
	}
	return sum;
}

/**
 * get char (representing a valid number) return int with that value.
 */
static int charToInt(char c)
{
	return (int) c - INT_ASCII_DIFF;
}

/**
 * @brief Returns the value of str as an integer.
 * 	If str cannot be parsed as an integer,
 * 	the return value should be MYSTR_ERROR_CODE
 * 	NOTE: positive and negative integers should be supported.
 * 	Assume the string is a valid int, as answered by Levi Offen in the exercise forum.
 * 	under this assumption i limit the number of digits to 10 + optional minus sign.
 * 	also if a non-supported charachter is encountered, an error is returned regardless of whether we
 * 	managed to read some chars before.
 *
 * @param str the MyString
 * @return an integer
 *
 * Time complexity: O(n) (power calculations are at most sum(1to9)=O(n), as n is bounded by 10)
 */
int myStringToInt(const MyString *str)
{
	signed int tempSum = 0;
	// since reading from left to right, keep the sign conversion to later.
	int signFlip = 1;
	int i = 0;

	// if first char is the neg sine, make the signFlip be negative.
	if (str->actualString[i] == NEGATIVE_SIGN)
	{
		signFlip = -1;
		i++;
	}

	//iterate over string, multiply each digit by  10^(digitNumber) and add to sum
	for (; i < str->length; ++i)
	{
		if (str->actualString[i] > INT_ASCII_MAX || str->actualString[i] < INT_ASCII_DIFF)
		{
			if (i == 0) tempSum = MYSTR_ERROR_CODE;
			return tempSum;
		}
		tempSum += (charToInt(str->actualString[i]) * (power(DECIMAL_SYSTEM, str->length - i)));
	}
	//make sure the sign is maintained.
	return tempSum * signFlip;

}


/**
 * @brief Returns the value of str as a C string, terminated with the
 * 	null character. It is the caller's responsibility to free the returned
 * 	string by calling free().
 * @param str the MyString
 * RETURN VALUE:
 *  @return the new string, or NULL if the allocation failed.
 *
 *  Time complexity: O(MALL)
 */
char *myStringToCString(const MyString *str)
{
	char *tempString = malloc(str->length * sizeof(char));

	return memcpy(tempString, str->actualString, str->length * sizeof(char));
}


/**
 * @brief Appends a copy of the source MyString src to the destination MyString dst.
 * @param dest the destination
 * @param src the MyString to append
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *  Time Complexity: O(MALL) + O(1).
 */
MyStringRetVal myStringCat(MyString *dest, const MyString *src)
{
	if (*dest->refCount != 0)
	{
		char *oldString = myStringToCString(dest);
		freeCString(dest);
		dest->actualString = oldString;
		refReset(dest);
	}
	//try to allocate memory.
	char *newPointer = realloc(dest->actualString, (dest->length + src->length) * sizeof(char));
	if (newPointer == NULL)
	{
		return MYSTRING_ERROR;
	}
	//if successful, copy src Cstring into the segment newly allocated, from the end of original destString
	// over the course of src.length chars.
	dest->actualString = newPointer;
	memcpy(&dest->actualString[dest->length], src->actualString, src->length * sizeof(char));
	dest->length = dest->length + src->length;
	return MYSTRING_SUCCESS;

}


/**
 * @brief Sets result to be the concatenation of str1 and str2.
 * 	result should be initially allocated by the caller.
 * 	result shouldn't be the same struct as str1 or str2.
 * @param str1
 * @param str2
 * @param result
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *  Time Complexity: O(MALL) + O(1)
 */
MyStringRetVal myStringCatTo(const MyString *str1, const MyString *str2, MyString *result)
{
	if (myStringSetFromMyString(result, str1) == MYSTRING_ERROR) return MYSTRING_ERROR;
	if (myStringCat(result, str2) == MYSTRING_ERROR) return MYSTRING_ERROR;
	return MYSTRING_SUCCESS;
}


/**
 * @brief Compares str1 and str2.
 * @param str1
 * @param str2
 * @param comparator
 * RETURN VALUE:
 * @return an integral value indicating the relationship between the strings:
 * 	A zero value indicates that the strings are equal according to the custom comparator (3rd parameter).
 * 	A value greater than zero indicates that the first MyString is bigger according to the comparator.
 * 	And a value less than zero indicates the opposite.
 * 	If strings cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	Time Complexity: best case O(1), worst case O(min(n,m)) where n,m are lengths of the strings 1,2.
 */

int myStringCustomCompare(const MyString *str1, const MyString *str2,
                          int (*comp)(const char *a, const char *b))
{

	// check if both strings are valid.
	if (isStringNull(str1) || isStringNull(str2)) return MYSTR_ERROR_CODE;

	// check if both strings are referencing the same CString.
	if (str1->actualString == str2->actualString) return COMP_EQUAL;

	int diff;
	//compare each pair of chars, return if mismatch found.
	for (int i = 0; i < MIN(str1->length, str2->length); ++i)
	{
		diff = comp((const char *) &str1->actualString[i], (const char *) &str2->actualString[i]);

		if (diff > 0) return COMP_GRT;
		if (diff < 0) return COMP_SMT;
	}

	// if all chars untill the max length of the shorter string match, then the longer string's next char
	// is greater than NULL==0, in which case the diff of lengths gives the required result.
	return str1->length - str2->length;


}

/**
 * helper method.
 * default comparator used by compare/equal methods
 * returns the diff betwen the integer values of two chars.
 */
static int defaultComparator(const char *a, const char *b)
{
	return (int) *a - (int) *b;
}

/**
 * @brief Compare str1 and str2.
 * @param str1
 * @param str2
 *
 * RETURN VALUE:
 * @return an integral value indicating the relationship between the strings:
 * 	A zero value indicates that the strings are equal.
 * 	A value greater than zero indicates that the first character that does not match has a greater ASCII value in str1 than in str2;
 * 	And a value less than zero indicates the opposite.
 * 	If strings cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	Time Complexity: see myStringCustomCompare.
 *
 */
int myStringCompare(const MyString *str1, const MyString *str2)
{
	//utilize the more powerful customCompare with a default comparator macro.
	return myStringCustomCompare(str1, str2, defaultComparator);

}


/**
 * @brief Check if str1 is equal to str2.
 * @param str1
 * @param str2
 *
 * RETURN VALUE:
 * @return an integral value indicating the equality of the strings (logical equality - that they are composed of the very same characters):
 * 	A zero value indicates that the strings are not equal.
 * 	A greater than zero value indicates that the strings are equal.
 * 	If strings cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	Time Complexity: see myStringCompare
 *
  */
int myStringEqual(const MyString *str1, const MyString *str2)
{
	int result = myStringCompare(str1, str2);
	return result == MYSTR_ERROR_CODE ? MYSTR_ERROR_CODE : (result == COMP_EQUAL ? COMP_TRUE : COMP_FALSE);
}


/**
 * @brief Check if str1 is equal to str2.
 * @param str1
 * @param str2
 * @param comparator
 *
 * RETURN VALUE:
 * @return an integral value indicating the equality of the strings using a custom comparator (3rd parameter):
 * 	A zero value indicates that the strings are not equal.
 * 	A greater than zero value indicates that the strings are equal.
 * 	If string cannot be compared, the return value should be MYSTR_ERROR_CODE
 *
 * 	Time Complexity: see myStringCustomCompare
 */
int myStringCustomEqual(const MyString *str1, const MyString *str2,
                        int (*comp)(const char *a, const char *b))
{
	int result = myStringCustomCompare(str1, str2, comp);
	return result == MYSTR_ERROR_CODE ? MYSTR_ERROR_CODE : (result == COMP_EQUAL ? COMP_TRUE : COMP_FALSE);
}

/**
 * @return the amount of memory (all the memory that used by the MyString object itself and its allocations),
 * in bytes, allocated to str1.
 * Time Complexity: O(1)
 */
unsigned long myStringMemUsage(const MyString *str1)
{
	unsigned long sumBytes = 0;
	sumBytes += sizeof(*str1);
	sumBytes += str1->length * sizeof(char);
	sumBytes += sizeof(*str1->refCount);
	return sumBytes;
}

/**
 * @return the length of the string in str1.
 *
 * O(1)...
 */
unsigned long myStringLen(const MyString *str1)
{
	return (unsigned long) str1->length;
}

/**
 * Writes the content of str to stream. (like fputs())
 *
 * RETURNS:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 *
 *  Time complexity: O(fprintf * 5) + O(fclose). whatever those are. otherwise- O(1)?
 */
MyStringRetVal myStringWrite(const MyString *str, FILE *stream)
{


	if (stream == NULL) return MYSTRING_ERROR;
	fprintf(stream, "\t%50s\n", "Here layeth the contents of a String Struct");
	fprintf(stream, "\t%50.40s%-10i\n", "It's length is: ", str->length);
	fprintf(stream, "\t%45.40s%10i%5s\n", "It has been referenced ", *str->refCount, " times.");
	fprintf(stream, "\t%50s\n", "And it's content is:");
	fprintf(stream, "\t%50s\n", str->actualString);

	fclose(stream);
	return MYSTRING_SUCCESS;
}



/**
 * as glibc qsort somehow corrupted the cStrings within my structs, i opted to stop wasting time (over 5
 * hours!) trying to understand what's wrong with it, and just occam's razor it out.
 * so here's a simple, not very efficient, but very working implementation of bubblesort while using
 * myStringCustomCompare with help of an external comparator for the actual comparisons.
 *
 */
static void myQsort(MyString** arr, int len,  int (*comp)(const char *a, const char *b))
{
	for(int x=0; x<len; x++)

	{
		for(int y=0; y<len-1; y++)
		{
			if(myStringCustomCompare(arr[y],arr[y+1],comp)>0)
			{
				MyString* temp = arr[y+1];
				arr[y+1] = arr[y];
				arr[y] = temp;
			}
		}
	}
}

/**
 * @brief sort an array of MyString pointers
 * @param arr
 * @param len
 * @param comparator custom comparator
 *
 * RETURN VALUE:none
 *
 * Time Complexity: O(qsort).
 * from cplusplus.com: complexity- Unspecified, but quicksorts are generally linearithmic in num, on average,
 * calling compar approximately num*log2(num) times.
 * so.. maybe O(NlgN)?
 * */
void myStringCustomSort(MyString **arr, int len, int (*comp)(const char *a, const char *b))
{
	myQsort(arr,len,comp);

}

/**
 * @brief sorts an array of MyString pointers according to the default comparison (like in myStringCompare)
 * @param arr
 * @param len
 *
 * RETURN VALUE: none
 *
 * Time Complexity: see myStringCustomSort.
  */
void myStringSort(MyString **arr, int len)
{
	myQsort(arr,len,defaultComparator);
}


/**
 * this method swaps the refcounter and actualString pointers between two strings.
 * in O(1).
 * since you gave no method description, i'm assuming this is what you meant.
 */
void myStringSwap(MyString *str1, MyString *str2)
{
	//single temp general pointer.
	void *tempPoint;
	//swap strings
	tempPoint = str1->actualString;
	str1->actualString = str2->actualString;
	str2->actualString = tempPoint;
	//swap references
	tempPoint = str1->refCount;
	str1->refCount = str2->refCount;
	str2->refCount = tempPoint;
	//swap lengths
	*((int *) tempPoint) = str1->length;
	str1->length = str2->length;
	str2->length = *((int *) tempPoint);

	//and done.

}


#ifndef NDEBUG

#include <string.h>

#define TEST_STRING1 "A string is A string"
#define TEST_STRING2 "B string is another String"
#define TEST_STRING3 "not all strings begin with uppercase Letters"
#define TEST_STRING6 "1234"
#define TEST_STRING7 "xv"
#define TEST_STRING4 "further strings"
#define TEST_STRING5 "A string is A string!"
#define TEST_NUM1 1234567890
#define TEST_NUM2 -1234567890
#define TEST_NUM3 99999999999


void sortTest()
{

	MyString **arr = malloc(5*8);
	MyString *a = myStringAlloc();
	MyString *b = myStringAlloc();
	MyString *c = myStringAlloc();
	MyString *d = myStringAlloc();
	MyString *e = myStringAlloc();

	arr[0] = myStringAlloc();
	arr[1] = myStringAlloc();
	arr[2] = myStringAlloc();
	arr[3] = myStringAlloc();
	arr[4] = myStringAlloc();

	myStringSetFromCString(arr[0], TEST_STRING3);
	myStringSetFromCString(arr[1], TEST_STRING2);
	myStringSetFromCString(arr[2], TEST_STRING1);
	myStringSetFromCString(arr[3], TEST_STRING6);
	myStringSetFromCString(arr[4], TEST_STRING7);

//	printf("comparing 2 strings: %d\n", myStringCompare(arr[2], arr[4]));
	for (int i = 0; i < 5; ++i)
	{
		char *curString = myStringToCString(arr[i]);
		printf("string num %i is: %s\n", i, curString);
	}

	myStringSort(arr, 5);

	for (int i = 0; i < 5; ++i)
	{
		char *curString = myStringToCString(arr[i]);
		printf("string num %i is: %s\n", i, curString);
	}

	int result = 0;
	result += strcmp(arr[0]->actualString, TEST_STRING6);
	printf("The results arrr: %d\n", result);
	result += strcmp(arr[1]->actualString, TEST_STRING7);
	printf("The results arrr: %d\n", result);
	result += strcmp(arr[2]->actualString, TEST_STRING1);
	printf("The results arrr: %d\n", result);
	result += strcmp(arr[3]->actualString, TEST_STRING2);
	printf("The results arrr: %d\n", result);
	result += strcmp(arr[4]->actualString, TEST_STRING3);
	printf("The results arrr: %d\n", result);


}

void swapTest()
{
	MyString *str1;
	str1 = myStringAlloc();
	myStringSetFromCString(str1, TEST_STRING1);
	MyString *str2;
	str2 = myStringAlloc();
	myStringSetFromCString(str2, TEST_STRING2);
	myStringSwap(str1, str2);

	int result = strcmp(str1->actualString, TEST_STRING2);
	int result2 = strcmp(str2->actualString, TEST_STRING1);

	if (result + result2 != 0)
	{
		printf("Tested the swap method, but it failed\n");
	}

}


int main()
{
	printf("running tests\n");

//	allocTest();
//	freeTest();
//	cloneTest();
//	filterTest();
//	myStringFromMyStringTest();
//	myStringFromCstringTest();
//	myStringFromIntTest();
//	myStringToIntTest();
//	myStringToCstrinTest();
//	catTest();
//	catToTest();
//	compareTest();
//	customCompareTest();
//	equalTest();
//	customEqualTest();
//	memUsageTest();
//	stringLengthTest();
//	writeTest();
//	customSortTest();
	sortTest();
	swapTest();

}


#endif