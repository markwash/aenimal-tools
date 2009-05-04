#ifndef integer_private_h
#define integer_private_h

#include "integer.h"

integer_t *integer_new();
void integer_clear(integer_t *i);

size_t integer_num_digits(integer_t *i);
void integer_word_power(integer_t *i, size_t digit, WORD w);

integer_t *integer_new_word_power(WORD w, size_t shift);

#endif
