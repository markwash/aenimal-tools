#include "integer.h"
#include "integer-private.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simple_vector.h"

#define WORD uint8_t
#define DWORD uint16_t
#define MAX_WORD 0xff
#define WORD_HEX_CODE "%hhx"
#define WORD_HEX_CODE_PAD "%02hhx"

struct integer {
	int positive;
	simple_vector_t *digits;
};

// {{{ static uint8_t convert_ascii_hex_to_number(char c) {
static int8_t convert_ascii_hex_to_number(char c) {
	
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	} else {
		return -1;
	}

} // }}}
// {{{ size_t integer_num_digits(integer_t *i) {
size_t integer_num_digits(integer_t *i) {
	return simple_vector_size(i->digits);
} // }}}

// {{{ integer_t *integer_new() {
integer_t *integer_new() {

	integer_t *i;
	
	if ((i = malloc(sizeof(integer_t))) == NULL) {
		return NULL;
	}

	if ((i->digits = simple_vector_new(2, sizeof(WORD))) == NULL) {
		integer_free(i);
		return NULL;
	}

	i->positive = 1;
	
	return i;

} // }}}
// {{{ integer_t *integer_new_zero() {
integer_t *integer_new_zero() {

	integer_t *i;

	if ((i = integer_new()) == NULL) {
		return NULL;
	}

	integer_zero(i);
	return i;

} // }}}
// {{{ integer_t *integer_new_from_hex(const char *str) {
integer_t *integer_new_from_hex(const char *str) {

	// create the integer
	integer_t *i;
	if ((i = integer_new()) == NULL) {
		return NULL;
	}
	
	// prepare to convert the string
	size_t len = strlen(str);
	size_t hex_digit = 0;
	int8_t num_digit;
	size_t basis;
	WORD w;

	// find the most significant non-zero hex char offset in the string, 
	// up until the least sig place where we'll keep a zero if its there
	size_t most_sig = 0;
	if (str[0] == '+') {
		most_sig += 1;
	} else if (str[0] == '-') {
		i->positive = 0;
		most_sig += 1;
	}
	if (strncmp("0x", str + most_sig, 2) == 0) {
		most_sig += 2;
	}
	for ( ; most_sig < len-1; most_sig++) {
		if (str[most_sig] != '0') {
			break;
		}
	}

	// go through each digit in the string, starting with the least significant
	for (hex_digit = len-1; hex_digit >= most_sig; ) {

		w = 0;

		for (basis = 1; basis < MAX_WORD && hex_digit >= most_sig; basis *= 0x10, hex_digit--) {

			// convert the hex char to a number
			if ((num_digit = convert_ascii_hex_to_number(str[hex_digit])) < 0) {
				integer_free(i);
				return NULL;
			}

			// add this to the word
			w += num_digit * basis;

		}

		// add this word to our digits
		// FIXME: switch this from simple_vector_append to integer_word_power
		if (simple_vector_append(i->digits, &w) < 0) {
			integer_free(i);
			return NULL;
		}

	}
	
	return i;

} // }}}
// {{{ integer_t *integer_new_word_power(WORD w, size_t shift) {
integer_t *integer_new_word_power(WORD w, size_t shift) {
	integer_t *i;
	if ((i = integer_new()) == NULL) {
		return NULL;
	}
	integer_word_power(i, shift, w);
	return i;
} // }}}
// {{{ void integer_free(integer_t *i) {
void integer_free(integer_t *i) {
	if (i != NULL) {
		simple_vector_free(i->digits, 0, NULL);
		free(i);
	}
} // }}}
// {{{ void integer_clear(integer_t *i) {
void integer_clear(integer_t *i) {

	simple_vector_clear(i->digits);

} // }}}
// {{{ void integer_zero(integer_t *i) {
void integer_zero(integer_t *i) {
	integer_clear(i);
	i->positive = 1;
	integer_word_power(i, 0, 0);
} // }}}
// {{{ static void integer_get_word(integer_t *i, WORD w) {
static void integer_get_word(integer_t *i, WORD w) {
	integer_clear(i);
	i->positive = 1;
	simple_vector_append(i->digits, &w);
} // }}}
// {{{ void integer_word_power(integer_t *i, size_t digit, WORD w) {
void integer_word_power(integer_t *i, size_t digit, WORD w) {
	size_t idigits = integer_num_digits(i);
	ssize_t d;
	WORD zero = 0;
	for (d = idigits; d <= digit; d++) {
		simple_vector_append(i->digits, &zero);
	}
	simple_vector_put(i->digits, digit, &w);
} // }}}

// {{{ char *integer_to_hex_string(integer_t *i) {
char *integer_to_hex_string(integer_t *i) {
	

	// allocate enough space for the string
	char *str;
	size_t len = 
			(i->positive ? 0 : 1)								// for minus sign
			+ 2 												// for 0x prefix
			+ simple_vector_size(i->digits) * sizeof(WORD) * 2	// for actual hex code
			+ 1; 												// for terminating NULL

	if ((str = malloc(len)) == NULL) {
		return NULL;
	}

	// prepare to print the number to the string
	ssize_t digit;
	size_t print_len = 0;
	WORD w;
	int result;

	// print minus sign for negatives
	if (!i->positive) {
		if ((result = sprintf(str + print_len, "-")) < 0) {
			free(str);
			return NULL;
		}
		print_len += result;
	}

	// print hex prefix
	if ((result = sprintf(str + print_len, "0x")) < 0) {
		free(str);
		return NULL;
	}
	print_len += result;

	// print the first digit (unpadded)
	digit = simple_vector_size(i->digits) - 1;

	simple_vector_get(i->digits, digit, &w);
	if ((result = sprintf(str + print_len, WORD_HEX_CODE, w)) < 0) {
		free(str);
		return NULL;
	}
	print_len += result;
	
	// print the remaining digits (zero padded)
	for (digit -= 1; digit >= 0; digit--) {

		simple_vector_get(i->digits, digit, &w);
		if ((result = sprintf(str + print_len, WORD_HEX_CODE_PAD, w)) < 0) {
			free(str);
			return NULL;
		}
		print_len += result;

	}

	return str;

} // }}}

// {{{ static int integer_magnitude_cmp(integer_t *lhs, integer_t *rhs) {
static int integer_magnitude_cmp(integer_t *lhs, integer_t *rhs) {

	// compare numbers of digits
	size_t ldigits, rdigits;
	ldigits = integer_num_digits(lhs);
	rdigits = integer_num_digits(rhs);
	if (ldigits < rdigits) {
		return -1;
	} else if (ldigits > rdigits) {
		return 1;
	}

	// same number of digits -- compare digits
	int digit;
	WORD wl, wr;
	for (digit = ldigits - 1; digit >= 0; digit -= 1) {
		simple_vector_get(lhs->digits, digit, &wl);
		simple_vector_get(rhs->digits, digit, &wr);
		if (wl < wr) {
			return -1;
		} else if (wl > wr) {
			return 1;
		}
	}
	
	// equal
	return 0;

} // }}}
// {{{ int integer_cmp(integer_t *lhs, integer_t *rhs) {
int integer_cmp(integer_t *lhs, integer_t *rhs) {

	// simple case, different signs
	if (lhs->positive && !rhs->positive) {
		return 1;
	} else if (!lhs->positive && rhs->positive) {
		return -1;
	}

	// if they're both negative, then reverse the decision of the absolute comparison
	int sign_factor;
	if (lhs->positive) { // both positive
		sign_factor = 1;
	} else { // both negative
		sign_factor = -1;
	}

	// same sign -- compare magnitudes
	return sign_factor * integer_magnitude_cmp(lhs, rhs);

} // }}}

// {{{ void integer_copy(integer_t *i1, integer_t *i2) {
void integer_copy(integer_t *i1, integer_t *i2) {

	integer_clear(i1);

	size_t digit;
	size_t digits = integer_num_digits(i2);
	WORD w;

	for (digit = 0; digit < digits; digit++) {
		simple_vector_get(i2->digits, digit, &w);
		simple_vector_append(i1->digits, &w);
	}

} // }}}

// {{{ static void integer_magnitude_add(integer_t *big, integer_t *lit, integer_t *sum_r) {
static void integer_magnitude_add(integer_t *big, integer_t *lit, integer_t *sum_r) {

	ssize_t digit;
	size_t bdigits = integer_num_digits(big);
	size_t ldigits = integer_num_digits(lit);
	DWORD dw;
	WORD w1, w2, w3, carry = 0;

	// digit by digit
	for (digit = 0; digit < bdigits; digit++) {
		simple_vector_get(big->digits, digit, &w1);
		if (digit < ldigits) {
			simple_vector_get(lit->digits, digit, &w2);
		} else {
			w2 = 0;
		}
		dw = (DWORD) w1 + (DWORD) w2 + (DWORD) carry;
		w3 = dw & MAX_WORD;
		simple_vector_append(sum_r->digits, &w3);
		carry = dw >> (sizeof(WORD) * 8); 
	}

	// append the carry if it's nonzero
	if (carry != 0) {
		simple_vector_append(sum_r->digits, &carry);
	}

} // }}}
// {{{ static void integer_magnitude_sub(integer_t *big, integer_t *lit, integer_t *diff_r) {
static void integer_magnitude_sub(integer_t *big, integer_t *lit, integer_t *diff_r) {

	ssize_t digit;
	size_t bdigits = integer_num_digits(big);
	size_t ldigits = integer_num_digits(lit);
	WORD w1, w2, w3, carry = 0;
	DWORD dw;
	int skipped_zeros = 0;

	// digit by digit
	for (digit = 0; digit < bdigits; digit += 1) {
		simple_vector_get(big->digits, digit, &w1);
		if (digit < ldigits) {
			simple_vector_get(lit->digits, digit, &w2);
		} else {
			w2 = 0;
		}
		dw = (DWORD) w1 - (DWORD) w2 - (DWORD) carry;
		if (dw > (DWORD) w1) {
			carry = 1;
		} else {
			carry = 0;
		}
		w3 = dw & MAX_WORD;
		if (w3 == 0) {
			skipped_zeros += 1;
		} else {
			w1 = 0;
			for ( ; skipped_zeros > 0; skipped_zeros -= 1) {
				simple_vector_append(diff_r->digits, &w1);
			}
			simple_vector_append(diff_r->digits, &w3);
		}
	}

	// there really shouldn't be anything in carry

} // }}}
// {{{ void integer_add(integer_t *i1, integer_t *i2, integer_t *sum_r) {
void integer_add(integer_t *i1, integer_t *i2, integer_t *sum_r) {

	// clear out sum
	integer_clear(sum_r);

	// determine big and little by magnitudes
	integer_t *big, *lit;
	if (integer_magnitude_cmp(i1, i2) >= 0) {
		big = i1;
		lit = i2;
	} else {
		big = i2;
		lit = i1;
	}

	// sign of sum is always sign of big
	sum_r->positive = big->positive;

	// if signs agree, add magnitudes, otherwise, subtract magnitudes
	if (big->positive == lit->positive) {
		integer_magnitude_add(big, lit, sum_r);
	} else {
		integer_magnitude_sub(big, lit, sum_r);
	}

} // }}}
// {{{ void integer_sub(integer_t *i1, integer_t *i2, integer_t *diff_r)
void integer_sub(integer_t *i1, integer_t *i2, integer_t *diff_r) {

	// clear out sum
	integer_clear(diff_r);

	// determine big and little by magnitudes
	integer_t *big, *lit;
	if (integer_magnitude_cmp(i1, i2) >= 0) {
		big = i1;
		lit = i2;
	} else {
		big = i2;
		lit = i1;
	}
	
	// if signs differ, its just addition and takes the sign of i1
	// else, subtract magnitudes
	if (i1->positive != i2->positive) {
		diff_r->positive = i1->positive;
		integer_magnitude_add(big, lit, diff_r);
	} else { 
		if (big == i1) {
			diff_r->positive = big->positive;
		} else {
			diff_r->positive = !big->positive;
		}
		integer_magnitude_sub(big, lit, diff_r);
	}

} // }}}

// {{{ void integer_mult(integer_t *i1, integer_t *i2, integer_t *prod_r) {
void integer_mult(integer_t *i1, integer_t *i2, integer_t *prod_r) {

	// set the product to zero
	integer_zero(prod_r);

	// multiply i1 by each digit in i2, accumulating in prod_r
	size_t i2_digits = integer_num_digits(i2);
	ssize_t digit;
	WORD w;
	for (digit = 0; digit < i2_digits; digit += 1) {
		simple_vector_get(i2->digits, digit, &w);
		integer_mult_word_add(i1, w, digit, prod_r);
	}

	// resolve sign of product
	prod_r->positive = i1->positive == i2->positive;

} // }}}

// {{{ void integer_div_approx(integer_t *i1, integer_t *i2, WORD *w_r, size_t *shift_r) {
void integer_div_approx(integer_t *i1, integer_t *i2, WORD *w_r, size_t *shift_r) {
} // }}}
// {{{ void integer_div2(integer_t *i1, integer_t *i2, integer_t *quot_r, integer_t *rem_r) {
void integer_div2(integer_t *i1, integer_t *i2, integer_t *quot_r, integer_t *rem_r) {

	WORD w;
	size_t shift;

	/*integer_zero(quot_r);
	integer_copy(rem_r, i2);
	
	while (integer_cmp(rem_r, i2) >= 0) {

		// rem_r / i2 ~= w followed by shift zeros, must always undershoot
		integer_div_approx(rem_r, i2, &w, &shift);

		// quot_r += w followed by shift zeros
		integer_accumulate_word(quot_r, w, shift);

		// rem_r -= i2 * (w followed by shift zeros)
		integer_mult_word_sub(i2, w, shift, rem_r);



	}*/

} // }}}
// {{{ void integer_div(integer_t *i1, integer_t *i2, integer_t *quot_r, integer_t *rem_r) {
void integer_div(integer_t *i1, integer_t *i2, integer_t *quot_r, integer_t *rem_r) {

	WORD w2, wq, wr;
	DWORD dw2, dwr, dwq;
	size_t n2, nr;

	n2 = integer_num_digits(i2);
	simple_vector_get(i2->digits, n2 - 1, &w2);

	integer_zero(quot_r);
	integer_copy(rem_r, i1);
	
	while (integer_cmp(rem_r, i2) >= 0) {

		nr = integer_num_digits(rem_r);
		simple_vector_get(rem_r->digits, nr - 1, &wr);

		// approximate quotient for this remaining part of i1
		if (w2 <= wr) {
			wq = wr / w2;
		} else {
			dwr = wr << (sizeof(WORD) * 8);
			nr--;
			simple_vector_get(rem_r->digits, nr - 1, &wr);
			dwr += wr;
			wq = (WORD) (dwr / (DWORD) wq);
		}
		
		// calculate real remainder for this approximate quotient
		integer_mult_word_sub(i1, wq, nr - n2, rem_r);

		// if we overshot, correct
		if (!rem_r->positive) {
			integer_mult_word_add(i1, 1, nr - n2, rem_r);
			wq--;
		}

		// accumulate the quotient
		integer_accumulate_word(quot_r, wq, nr - n2);

	}

} // }}}

// {{{ void integer_accumulate_word(integer_t *i, WORD w, size_t shift) {
void integer_accumulate_word(integer_t *i, WORD w, size_t shift) {

	size_t digits = integer_num_digits(i);
	DWORD dw;
	WORD iw, carry;

	// Add with carry starting in the middle of i (if necessary)
	while (w > 0 && shift < digits) {

		simple_vector_get(i->digits, shift, &iw);
	
		dw = (DWORD) iw + (DWORD) w;
		iw = dw & MAX_WORD;
		carry = dw >> (sizeof(WORD) * 8);

		shift++;
		w = carry;
	}

	// Add beyond the most significant digits of i
	if (w > 0) {
		ssize_t digit;

		// add leading zeroes, if any
		iw = 0;
		for (digit = digits; digit < shift; digit++) {
			simple_vector_append(i->digits, &iw);
		}
	
		// add w as most significant digit
		simple_vector_append(i->digits, &w);
	}

} // }}}
// {{{ void integer_mult_word_add(integer_t *i, WORD w, size_t shift, integer_t *acc_r) {
void integer_mult_word_add(integer_t *i, WORD w, size_t shift, integer_t *acc_r) {

	ssize_t digit;
	size_t digits = integer_num_digits(i);
	size_t adigits = integer_num_digits(acc_r);
	DWORD dw;
	WORD iw, aw, mult_carry = 0, add_carry = 0;

	// if w == 0, do nothing
	if (w == 0) {
		return;
	}
	
	// digit by digit, multiply the word, save the mult_carry, 
	// add it to the accumulator, and save the add_carry
	for (digit = 0; digit < digits; digit++) {

		simple_vector_get(i->digits, digit, &iw);

		dw = (DWORD) iw * (DWORD) w + (DWORD) mult_carry;
		mult_carry = dw >> (sizeof(WORD) * 8);

		if (digit + shift < adigits) {
			simple_vector_get(acc_r->digits, digit + shift, &aw);
		} else {
			aw = 0;
		}

		dw = (dw & MAX_WORD) + (DWORD) aw + (DWORD) add_carry;
		add_carry = dw >> (sizeof(WORD) * 8);
		aw = dw & MAX_WORD;

		if (digit + shift < adigits) {
			simple_vector_put(acc_r->digits, digit + shift, &aw);
		} else {
			simple_vector_append(acc_r->digits, &aw);
		}

	}

	// push it all into the add_carry
	add_carry += mult_carry;

	// keep adding until the add_carry goes away
	for ( ; add_carry > 0; digit++) {

		if (digit + shift < adigits) {
			simple_vector_get(acc_r->digits, digit + shift, &aw);
		} else {
			aw = 0;
		}

		dw = (DWORD) aw + (DWORD) add_carry;
		add_carry = dw >> (sizeof(WORD) * 8);
		aw = dw & MAX_WORD;

		if (digit + shift < adigits) {
			simple_vector_put(acc_r->digits, digit + shift, &aw);
		} else {
			simple_vector_append(acc_r->digits, &aw);
		}

	}

} // }}}
// {{{ void integer_mult_word_sub(integer_t *i, WORD w, size_t shift, integer_t *acc_r) {
void integer_mult_word_sub(integer_t *i, WORD w, size_t shift, integer_t *acc_r) {

	// assumes w * (MAX_WORD + 1) ^ shift < i

	// create temporary variable
	integer_t *temp;
	temp = integer_new_zero();

	// insert trailing zeroes
	ssize_t digit;
	WORD tw = 0;
	for (digit = 0; digit < shift; digit++) {
		simple_vector_append(temp->digits, &tw);
	}

	// perform multiplication digit by digit
	size_t idigits = integer_num_digits(i);
	DWORD dw;
	WORD iw, carry = 0;
	for (digit = 0; digit < idigits; digit++) {
		simple_vector_get(i->digits, digit, &iw);
		dw = (DWORD) w * (DWORD) iw + (DWORD) carry;
		tw = dw & MAX_WORD;
		carry = dw >> (sizeof(WORD) * 8);
		simple_vector_append(temp->digits, &tw);
	}

	// append carry if necessary
	if (carry > 0) {
		simple_vector_append(temp->digits, &carry);
	}

	// subtract temp from accumulator
	WORD w1, w2, w3;
	size_t adigits = integer_num_digits(acc_r);
	size_t tdigits = integer_num_digits(temp);
	int skipped_zeros = 0;
	for (digit = 0; digit < adigits; digit += 1) {
		simple_vector_get(acc_r->digits, digit, &w1);
		if (digit < tdigits) {
			simple_vector_get(temp->digits, digit, &w2);
		} else {
			w2 = 0;
		}
		dw = (DWORD) w1 - (DWORD) w2 - (DWORD) carry;
		if (dw > (DWORD) w1) {
			carry = 1;
		} else {
			carry = 0;
		}
		w3 = dw & MAX_WORD;
		if (w3 == 0) {
			skipped_zeros += 1;
		} else {
			w1 = 0;
			for ( ; skipped_zeros > 0; skipped_zeros -= 1) {
				simple_vector_append(acc_r->digits, &w1);
			}
			simple_vector_append(acc_r->digits, &w3);
		}
	}

	// there really shouldn't be anything in carry

} // }}}

// vim: fdm=marker ts=4
