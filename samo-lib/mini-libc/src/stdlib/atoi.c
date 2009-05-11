
#include <stdlib.h>
#include <ctype.h>


int atoi( const char *p)
{
    int sign = 0;
    int res = 0;

    while(   *p==' '
             || *p=='\t'
             || *p=='\n'
             || *p=='\f'
             || *p=='\r'
             || *p=='\v' ) p++;

    if(*p == '-' ) 
    {
      sign = 1; 
      p++;
    }
    else if(*p == '+') p++;

    if(!isdigit(*p)) return 0;

    while(1)
    {
        res += *p - '0';
        p++;
        if(!isdigit(*p)) break;
        res = res*10;
    }

    if(sign) res = -res;

    return res;
}

