#ifndef STRING_H
#define STRING_H

size_t strlen(const char* str);
void revstr(char *str);
size_t strcmp(const char* a, const char* b);
char* itoa(int num, char* str, int base);

#endif