extern int isalpha(int c);
extern int isdigit(int c);
int isalnum(int c)                { return (isalpha(c)||isdigit(c));}
