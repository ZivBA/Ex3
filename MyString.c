//
// Created by ziv on 8/9/15.
//


#include <stdlib.h>
#include <assert.h>
#include "MyString.h"

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
	*(temp->refCount) = 0;
	return temp;
}


/**
 * helper method to decrease the reference counter.
 */
static void refCountDecrement(MyString *str)
{
	*(str->refCount) = *(str->refCount) - 1;
}

/**
 * helper method to increase the reference counter.
 */
static void refCountIncrement(MyString *str)
{
	*(str->refCount) = *(str->refCount) + 1;
}

/**
 * helper method to free a cString only if it has no copies.
 */
static void freeCString(MyString *pString)
{
	if (pString->refCount == 1)
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
	temp->actualString = str->actualString;
	refCountIncrement(temp);
	temp->refCount = str->refCount;
	temp->length = str->length;
	return temp;
}

/**
 * @brief   doesnt copy, just change reference. - refCounting
 *          Sets the value of str to the value of other.
 * @param str the MyString to set
 * @param other the MyString to set from
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 */
MyStringRetVal myStringSetFromMyString(MyString *str, const MyString *other)
{
	if (other->actualString == NULL)
	{
		return MYSTRING_ERROR;
	}
	free(str->actualString);
	str->actualString = other->actualString;

	//check and see if the old CString can be freed.
	freeCString(str);

	//re-point counter and increment.
	str->refCount = other->refCount;
	refCountIncrement(str);

	return MYSTRING_SUCCESS;
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
	if (*newString == NULL)
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

	// housekeeping: reallocate memory to minimize array size, decrement refCount for
	// old string in case it has copies, free old
	realloc(newString, counter * sizeof(char));

	// ensure realloc was successfull
	if (sizeof(newString) != counter * sizeof(char))
	{
		return MYSTRING_ERROR;
	}

	refCountDecrement(str);
	freeCString(str);
	str->actualString = newString;
	return MYSTRING_SUCCESS;


}

/**
 * @brief Sets the value of str to the value of the given C string.
 * 			The given C string must be terminated by the null character.
 * 			Checking will not use a string without the null character.
 * @param str the MyString to set.
 * @param cString the C string to set from.
 * RETURN VALUE:
 *  @return MYSTRING_SUCCESS on success, MYSTRING_ERROR on failure.
 **/

MyStringRetVal myStringSetFromCString(MyString *str, const char *cString)
{
	freeCString(str);

}

