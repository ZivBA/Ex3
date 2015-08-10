//
// Created by ziv on 8/9/15.
//


#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "MyString.h"
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
// default comparator, return a-b for two chars casted to ints.
#define DEFAULT_COMPARATOR(a, b) ((int)a-(int)b)
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
 */
MyString *myStringAlloc()
{
	MyString *temp = malloc(sizeof(MyString));
	temp->actualString = "";
	temp->length = 0;
	temp->refCount = 0;
	return temp;
}


/**
 * helper method to decrease the reference counter.
 */
static void refCountDecrement(MyString *str)
{
	str->refCount--;
}

/**
 * helper method to increase the reference counter.
 */
static void refCountIncrement(MyString *str)
{
	str->refCount++;
}

/**
 * helper method to free a cString only if it has no copies.
 */
static void freeCString(MyString *pString)
{
	if (*pString->refCount == 1)
	{
		free(pString->actualString);
	}
	else
	{
		refCountDecrement(pString);
	}
}

/**
 * @brief **ref-Counting** Frees the memory and resources allocated to str.
 *        If reference counter is at 0 then all copies are freed and this string can be freed as well.
 *        otherwise decrement the refCount and free the struct itself.
 * @param str the MyString to free.
 * If str is NULL, no operation is performed.
 */
void myStringFree(MyString *str)
{
	freeCString(str);
	free(str);
}

static bool strCheckNull(const MyString *str)
{
	if (str == NULL || str->actualString == NULL)
	{
		return false;
	}
}

/**
 * @brief   ** implementing O(1) string cloning by ref-counting.
 *          Allocates a new MyString with the same value as str. It is the caller's
 * 			responsibility to free the returned MyString.
 * @param str the MyString to clone.
 * RETURN VALUE:
 *   @return a pointer to the new string, or NULL if the allocation failed.
 */
MyString *myStringClone(const MyString *str)
{
	MyString *temp = myStringAlloc();

	//check if string to clone is non-null
	if (strCheckNull(str))
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
 * @brief   doesnt copy value, just reference. - refCounting
 *          Sets the <s>value</s> reference of str to the <s>value</s> reference of other.
 * @param str the MyString to set
 * @param other the MyString to set from
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 */
MyStringRetVal myStringSetFromMyString(MyString *str, const MyString *other)
{
	// check that the source string is non-null
	if (!strCheckNull(other))
	{
		free(str->actualString);
		str->actualString = other->actualString;
	}
	//check and see if the old CString can be freed.
	freeCString(str);

	//re-point counter and increment.
	str->refCount = other->refCount;
	refCountIncrement(str);

	return MYSTRING_SUCCESS;
}


static bool cStringNullCheck(char *cStringToCheck)
{
	return *cStringToCheck == NULL;
}

/**
 * @brief filter the value of str acording to a filter.
 * 	remove from str all the occurrence of chars that are filtered by filt
 *	(i.e. filr(char)==true)
 * @param str the MyString to filter
 * @param filt the filter
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure. */
MyStringRetVal myStringFilter(MyString *str, bool (*filt)(const char *))
{
	// prep new string - alloc maximum size =old size, inject passing chars
	char *newString = malloc((str->length) * sizeof(char));

	// ensure malloc is successful.
	if (cStringNullCheck(newString))
	{
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
	realloc(newString, counter * sizeof(char));

	// ensure realloc was successfull
	if (sizeof(newString) != counter * sizeof(char))
	{
		return MYSTRING_ERROR;
	}

	freeCString(str);
	str->actualString = newString;
	return MYSTRING_SUCCESS;


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
 **/

MyStringRetVal myStringSetFromCString(MyString *str, const char *cString)
{

	//check that the string isn't null
	if (cStringNullCheck(cString)) return MYSTRING_ERROR;
	//free old Cstring then reassign the pointer to new string.
	freeCString(str);
	str->actualString = (char *) cString;
	//reset the refcount for this string.
	str->refCount = 0;
	return MYSTRING_SUCCESS;
}


static char *reverseCstring(char *tempNum, int i)
{
	for (int j = 0; j < i - 1; ++j)
	{
		printf("Current num =:%s\n", tempNum);
		printf("Current j =:%i\n", j);
		char tempChar;

		tempChar = tempNum[j];
		tempNum[j] = tempNum[i - j - 1];
		tempNum[i - j - 1] = tempChar;

	}
	return tempNum;
}

static char *intToCString(int n)
{
	int i = 0;
	char *tempNum = malloc(MAX_DIGITS_FOR_INT * sizeof(char));

	if (cStringNullCheck(tempNum))
	{
		return NULL;
	}
	if (n < 0)
	{
		tempNum[i] = NEGATIVE_SIGN;
		// switch to positive int to prevent calculation issues with mod/floor.
		n *= -1;
		i++;
	}
	for (; n != 0 || i < MAX_DIGITS_FOR_INT; ++i)
	{
		tempNum[i] = (char) (n % 10 + INT_ASCII_DIFF);
		n /= DECIMAL_SYSTEM;
		printf("Current num =:%s\n", tempNum);
		printf("Current i =:%i\n", i);
	}

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
 */
MyStringRetVal myStringSetFromInt(MyString *str, int n)
{
	//free old string
	freeCString(str);
	// parse int to Cstring and assign reference to string, then zero the counter;
	str->actualString = (intToCString(n));
	str->refCount = 0;
	//check that the procedure was succesfull
	if (str->actualString == NULL)
	{
		return MYSTRING_ERROR;
	}
	return MYSTRING_SUCCESS;

}

/**
 * @brief Returns the value of str as an integer.
 * 	If str cannot be parsed as an integer,
 * 	the return value should be MYSTR_ERROR_CODE
 * 	NOTE: positive and negative integers should be supported.
 * @param str the MyString
 * @return an integer
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
	for (; i <= str->length; ++i)
	{
		if (str->actualString[i] > INT_ASCII_MAX || str->actualString[i] < INT_ASCII_DIFF)
		{
			tempSum = MYSTRING_ERROR;
			return tempSum;
		}
		tempSum += (str->actualString[i] * (pow(DECIMAL_SYSTEM, str->length - i)));
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
 */
MyStringRetVal myStringCat(MyString *dest, const MyString *src)
{
	if (dest->refCount != 0)
	{
		char *oldString = myStringToCString(dest);
		freeCString(dest);
		dest->actualString = oldString;
		dest->refCount = 0;
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
 */

int myStringCustomCompare(const MyString *str1, const MyString *str2,
                          int (*comp)(const char *a, const char *b))
{

	// check if both strings are valid.
	if (strCheckNull(str1) || strCheckNull(str2)) return MYSTRING_ERROR;

	// check if both strings are referencing the same CString.
	if (str1->actualString == str2->actualString) return COMP_EQUAL;

	int diff;
	//compare each pair of chars, return if mismatch found.
	for (int i = 0; i < MIN(str1->length, str2->length); ++i)
	{
		diff = comp((const char *) str1->actualString[i], (const char *) str2->actualString[i]);

		if (diff > 0) return COMP_GRT;
		if (diff < 0) return COMP_SMT;
	}

	// if all chars untill the max length of the shorter string match, then the longer string's next char
	// is greater than NULL==0, in which case the diff of lengths gives the required result.
	return str1->length - str2->length;


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
 */
int myStringCompare(const MyString *str1, const MyString *str2)
{
	//utilize the more powerful customCompare with a default comparator macro.
	return myStringCustomCompare(str1, str2, DEFAULT_COMPARATOR);

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
  */
int myStringEqual(const MyString *str1, const MyString *str2)
{
	return myStringCompare(str1, str2) == COMP_EQUAL ? COMP_TRUE : COMP_FALSE;
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
 */
int myStringCustomEqual(const MyString *str1, const MyString *str2,
                        int (*comp)(const char *a, const char *b))
{
	return myStringCustomCompare(str1, str2, comp) == COMP_EQUAL ? COMP_TRUE : COMP_FALSE;
}

/**
 * @return the amount of memory (all the memory that used by the MyString object itself and its allocations), in bytes, allocated to str1.
 */
unsigned long myStringMemUsage(const MyString *str1)
{
	unsigned long sumBytes = 0;
	sumBytes += sizeof(MyString);
	sumBytes += str1->length * sizeof(char);
	return sumBytes;
}

/**
 * @return the length of the string in str1.
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
 */
MyStringRetVal myStringWrite(const MyString *str, FILE *stream)
{


	if (stream == NULL) return MYSTRING_ERROR;
	fprintf(stream, "\t%50s\n", "Here layeth the contents of a String Struct");
	fprintf(stream, "\t%50.40s%-10i\n", "It's length is: ",str->length);
	fprintf(stream, "\t%45.40s%10i%5s\n", "It has been referenced ",*str->refCount, " times.");
	fprintf(stream, "\t%50s\n", "And it's content is:");
	fprintf(stream, "\t%50s\n", str->actualString);

	fclose(stream);
}



/**
 * @brief sort an array of MyString pointers
 * @param arr
 * @param len
 * @param comparator custom comparator
 *
 * RETURN VALUE:none
  */
void myStringCoustomSort(MyString *arr, int len, int (*comp)(const char *a, const char *b))
{

	int cmpfunc(const void *a, const void *b)
	{
		int result = myStringCustomCompare((MyString *) a, (MyString *) b, comp);
		if (result == MYSTRING_ERROR) return NULL;
		return result;
	}


	qsort(arr, (size_t) len, sizeof(MyString), cmpfunc);

}
/**
 * @brief sorts an array of MyString pointers according to the default comparison (like in myStringCompare)
 * @param arr
 * @param len
 *
 * RETURN VALUE: none
  */
//TODO insert myStringSort signature here