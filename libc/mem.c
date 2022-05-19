#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mem.h"

/**
 * While the length is greater than zero, set the value of the pointer to the value of the destination.
 * 
 * @param dest The destination address to copy to.
 * @param val The value to set each byte to.
 * @param len The number of bytes to set.
 * 
 * @return The address of the first byte of the memory block.
 */
void memset(void *dest, int val, size_t len) {
	unsigned char *ptr = dest;
	while (len-- > 0)
		*ptr++ = val;
	return dest;
}