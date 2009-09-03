#ifndef BIGRAM_H
#define BIGRAM_H

#define SUPPORTED_SEARCH_CHARS " !#$%&'()*+,-.0123456789abcdefghijklmnopqrstuvwxyz=?@"
// SERCH_CHR_COUNT needs to be strlen(SUPPORTED_SEARCH_CHARS) + 1
#define SEARCH_CHR_COUNT 54
int bigram_char_idx(char c);
void bigram_encode(char *outStr, char *inStr);
void bigram_decode(char *outStr, char *inStr);
void init_bigram(int fd);
void init_char_idx();

#endif
