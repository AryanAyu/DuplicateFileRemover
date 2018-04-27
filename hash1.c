#include <stdio.h>
#include <stddef.h>

size_t stringLength(const char* source)
{
    if(source == NULL) { return 0; }

    size_t length = 0;
    while(*source != '\0') {
        length++;
        source++;
    }
    return length;
}
static size_t getHash(const char* cp)
{
    size_t hash = 0;
    while (*cp)
        hash = (hash * 10) + *cp++ - '0';
    return hash;
}
#define TABLE_SIZE (1)
int main()
{
    size_t name = getHash("ayusharyan.txt5");
    printf("%zu\n", name);
    return 0;
}
