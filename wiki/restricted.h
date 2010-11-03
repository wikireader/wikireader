/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LCD_RESTRICTED_H
#define _LCD_RESTRICTED_H

#define MAX_PASSWORD_LEN 9

void set_password(void);
void get_password(void);
void check_password(void);
int check_restriction(void);
int password_add_char(char c);
int password_remove_char(void);
int clear_password_string(void);
int get_password_string_len(void);
void handle_password_key(char keycode);
void filter_option(void);
int init_article_filter(void);

#endif /* _LCD_RESTRICTED_H */
