#ifndef _LCD_RESTRICTED_H
#define _LCD_RESTRICTED_H

#define MAX_PASSWORD_LEN 9
#define RESTRICTED_MARK_LINK 9999999

void set_password(void);
void get_password(void);
void check_password(void);
int check_restriction(long idx_article);
int password_add_char(char c);
int password_remove_char(void);
int clear_password_string(void);
int get_password_string_len(void);
void handle_password_key(char keycode);
void draw_restricted_mark(char *screen_buf);
void filter_option(void);

#endif /* _LCD_RESTRICTED_H */
