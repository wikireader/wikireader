#ifndef BIGRAM_H
#define BIGRAM_H

#define SEARCH_CHR_COUNT 54
int bigram_char_idx(char c);
void bigram_encode(char *outStr, char *inStr);
void bigram_decode(char *outStr, char *inStr);
void init_bigram(int fd);
void init_char_idx();

#endif
