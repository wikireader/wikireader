extern int isdigit(int c);

int isxdigit(int c)
{
 return (isdigit(c) || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'));
}
