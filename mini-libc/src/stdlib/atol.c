/* written by marekm@linux.org.pl, hardly worth copyrighting :-) */

#include <stdlib.h>

long
atol(const char *p)
{
    return strtol(p, (char **) 0, 10);
}
