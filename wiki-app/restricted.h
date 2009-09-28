#ifndef _LCD_RESTRICTED_H
#define _LCD_RESTRICTED_H

#define MAX_PASSWORD_LEN 17

void set_password(void);
void get_password(void);
int check_password(char *password);
int check_restriction(long idx_article);
int password_add_char(char c);
int password_remove_char(void);
int clear_password_string(void);
int get_password_string_len(void);

#endif /* _LCD_RESTRICTED_H */
