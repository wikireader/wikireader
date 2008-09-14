#include <ctype.h>

int ispunct(int c)
{
 return (isprint (c) && !islower(c) && !isupper(c) && c != ' ' && !isdigit(c));
}

 
 
