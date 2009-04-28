#ifndef INTEGER_H
#define INTEGER_H

#include <sys/types.h>
#include <stdint.h>

#define WORD uint8_t

struct integer;
typedef struct integer integer_t;

integer_t *integer_new_zero();
integer_t *integer_new_from_hex(const char *string);
void integer_free(integer_t *i);

char *integer_to_hex_string(integer_t *i);

int integer_cmp(integer_t *lhs, integer_t *rhs);

// i = 0
void integer_zero(integer_t *i);
// i1 = i2
void integer_copy(integer_t *i1, integer_t *i2);

// sum_r = i1 + i2
void integer_add(integer_t *i1, integer_t *i2, integer_t *sum_r);
// diff_r = i1 - i2
void integer_sub(integer_t *i1, integer_t *i2, integer_t *diff_r);
// prod_r = i1 * i2
void integer_mult(integer_t *i1, integer_t *i2, integer_t *prod_r);
// i1 = quot_r * i2 + rem_r, 0 <= rem_r < i2
void integer_div(integer_t *i1, integer_t *i2, integer_t *quot_r, integer_t *rem_r);


// i += w * (MAX_WORD + 1) ^ shift
void integer_accumulate_word(integer_t *i, WORD w, size_t shift);
// acc_r += i * w * (MAX_WORD + 1) ^ shift
void integer_mult_word_add(integer_t *i, WORD w, size_t shift, integer_t *acc_r);
// acc_r -= i * w * (MAX_WORD + 1) ^ shift
void integer_mult_word_sub(integer_t *i, WORD w, size_t shift, integer_t *acc_r);


#endif
