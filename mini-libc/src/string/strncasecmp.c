/* (c) bsd */

/* $Id */

#include <sys/cdefs.h>
#include <string.h>
#include <ctype.h>

int
strncasecmp(s1, s2, n)
const char *s1, *s2;
register size_t n;
{
    if (n != 0)
    {
        register const u_char
        *us1 = (const u_char *)s1,
               *us2 = (const u_char *)s2;

        do
        {
            if (tolower(*us1) != tolower(*us2++))
                return (tolower(*us1) - tolower(*--us2));
            if (*us1++ == '\0')
                break;
        }
        while (--n != 0);
    }
    return (0);
}
