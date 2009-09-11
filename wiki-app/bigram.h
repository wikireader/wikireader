#ifndef BIGRAM_H
#define BIGRAM_H

#define SUPPORTED_SEARCH_CHARS " !#$%&'()*+,-.0123456789abcdefghijklmnopqrstuvwxyz=?@"
// SERCH_CHR_COUNT needs to be strlen(SUPPORTED_SEARCH_CHARS) + 1
#define SEARCH_CHR_COUNT 54
#define MAX_SEARCH_STRING_HASHED_LEN 20
#define SIZE_BIGRAM_BUF (128 * 2)
int bigram_char_idx(char c);
void bigram_encode(char *outStr, char *inStr);
void bigram_decode(char *outStr, char *inStr);
void init_bigram(int fd);
void init_char_idx();
int is_supported_search_char(char c);
int search_string_cmp(char *title, char *search, int len);

#endif
