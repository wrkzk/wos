#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "string.h"

/**
 * While the character at the current index is not a null terminator, increment the length and the
 * index.
 * 
 * @param str The string to get the length of.
 * 
 * @return The length of the string.
 */
size_t strlen(const char* str) {
	size_t len = 0;
	while(str[len]) len++;
	return len;
}

/**
 * We start at the beginning and end of the string, and swap the characters at those positions. We then
 * move the start and end pointers inwards, towards the middle of the string, and repeat the process
 * 
 * @param str The string to reverse.
 */
void revstr(char *str) {
	int start = 0;
	int end = strlen(str) - 1;
	int temp;

	while (start < end) {
		temp = str[start];
		str[start] = str[end];
		str[end] = temp;

		start++;
		end--;
	}
}


/**
 * It compares two strings and returns the difference between the first two characters that are
 * different
 * 
 * @param a The first string to compare.
 * @param b The string to compare against.
 * 
 * @return The difference between the first two characters that differ in the strings being compared.
 */
size_t strcmp(const char* a, const char* b) {
	while (*a) {
		if (*a != *b) break;

		a++;
		b++;
	}

	return *(const unsigned char*)a - *(const unsigned char*)b;
}

/**
 * The function takes in an integer, a string, and a base. It converts the integer to a string in the
 * given base and returns the string
 * 
 * @param num The number to be converted.
 * @param str The string to store the result.
 * @param base The base in which we want to convert the number.
 * 
 * @return The address of the first character in the string.
 */
char* itoa(int num, char* str, int base) {
	int i = 0;

	if (num == 0) {
		str[i++] = "0";
		str[i] = '\0';
		return str;
	}

	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num /= base;
	}

	str[i] = '\0';
	revstr(str);
	return str;
}