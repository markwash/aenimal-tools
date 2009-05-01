#include <stdio.h>
#include <stdlib.h>

#include <check.h>

#include "../src/integer.h"


// {{{ START_TEST(test_integer_new_zero)
START_TEST(test_integer_new_zero)
{
	integer_t *zero;
	zero = integer_new_zero();
	char *s;
	s = integer_to_hex_string(zero);
	fail_unless(strcmp(s, "0x0") == 0, NULL);
	integer_free(zero);
	free(s);
}
END_TEST // }}}
// {{{ START_TEST(test_integer_new_from_hex)
START_TEST(test_integer_new_from_hex)
{
	integer_t *i;
	char *s;

	i = integer_new_from_hex("0x0");
	s = integer_to_hex_string(i);
	fail_unless(strcmp(s, "0x0") == 0, NULL);

	integer_free(i);
	free(s);

	i = integer_new_from_hex("00f00");
	s = integer_to_hex_string(i);
	fail_unless(strcmp(s, "0xf00") == 0, NULL);

	integer_free(i);
	free(s);

	i = integer_new_from_hex("-f0f0f");
	s = integer_to_hex_string(i);
	fail_unless(strcmp(s, "-0xf0f0f") == 0, NULL);

	integer_free(i);
	free(s);

	i = integer_new_from_hex("+0xfedcba");
	s = integer_to_hex_string(i);
	fail_unless(strcmp(s, "0xfedcba") == 0, NULL);

	integer_free(i);
	free(s);

	i = integer_new_from_hex("0x00fedcba9876543210");
	s = integer_to_hex_string(i);
	fail_unless(strcmp(s, "0xfedcba9876543210") == 0, NULL);

	integer_free(i);
	free(s);
}
END_TEST // }}}
// {{{ START_TEST(test_integer_cmp)
START_TEST(test_integer_cmp)
{
	integer_t *i1;
	integer_t *i2;

	i1 = integer_new_zero();
	fail_unless(integer_cmp(i1, i1) == 0);
	integer_free(i1);

	i1 = integer_new_from_hex("0xf0000");
	i2 = integer_new_from_hex("0xeffff");
	fail_unless(integer_cmp(i1, i2) == 1);
	fail_unless(integer_cmp(i2, i1) == -1);
	integer_free(i1);
	integer_free(i2);

	i1 = integer_new_from_hex("0xffe");
	i2 = integer_new_from_hex("0x5");
	fail_unless(integer_cmp(i1, i2) == 1);
	fail_unless(integer_cmp(i2, i1) == -1);
	integer_free(i1);
	integer_free(i2);

	i1 = integer_new_from_hex("-0xffff");
	i2 = integer_new_from_hex("0xfffe");
	fail_unless(integer_cmp(i1, i2) == -1);
	fail_unless(integer_cmp(i2, i1) == 1);
	integer_free(i1);
	integer_free(i2);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_add)
START_TEST(test_integer_add)
{

	integer_t *i1, *i2, *sum;
	char *s;

	i1 = integer_new_from_hex("0xfedcba9876543210");
	i2 = integer_new_from_hex("0x0123456789abcdef");
	sum = integer_new_zero();
	integer_add(i1, i2, sum);
	s = integer_to_hex_string(sum);
	fail_unless(strcmp(s, "0xffffffffffffffff") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(sum);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_add_uneven_digits)
START_TEST(test_integer_add_uneven_digits)
{

	integer_t *i1, *i2, *sum;
	char *s;

	i1 = integer_new_from_hex("0xfedcba9876543210");
	i2 = integer_new_from_hex("0xabcdef");
	sum = integer_new_zero();
	integer_add(i1, i2, sum);
	s = integer_to_hex_string(sum);
	fail_unless(strcmp(s, "0xfedcba9876ffffff") == 0);

	free(s);
	integer_add(i2, i1, sum);
	s = integer_to_hex_string(sum);
	fail_unless(strcmp(s, "0xfedcba9876ffffff") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(sum);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_add_carry)
START_TEST(test_integer_add_carry)
{

	integer_t *i1, *i2, *sum;
	char *s;

	i1 = integer_new_from_hex("0xffff0fff");
	i2 = integer_new_from_hex("0x00010001");
	sum = integer_new_zero();
	integer_add(i1, i2, sum);
	s = integer_to_hex_string(sum);
	fail_unless(strcmp(s, "0x100001000") == 0);

	free(s);
	integer_add(i2, i1, sum);
	s = integer_to_hex_string(sum);
	fail_unless(strcmp(s, "0x100001000") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(sum);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_sub)
START_TEST(test_integer_sub)
{

	integer_t *i1, *i2, *diff;
	char *s;

	i1 = integer_new_from_hex("0xffffffffffffffff");
	i2 = integer_new_from_hex("0xfedcba9876543210");
	diff = integer_new_zero();
	integer_sub(i1, i2, diff);
	s = integer_to_hex_string(diff);
	fail_unless(strcmp(s, "0x123456789abcdef") == 0);
	free(s);

	integer_sub(i2, i1, diff);
	s = integer_to_hex_string(diff);
	fail_unless(strcmp(s, "-0x123456789abcdef") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(diff);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_sub_uneven_digits)
START_TEST(test_integer_sub_uneven_digits)
{

	integer_t *i1, *i2, *diff;
	char *s;

	i1 = integer_new_from_hex("0x100000000");
	i2 = integer_new_from_hex("0x1");
	diff = integer_new_zero();
	integer_sub(i1, i2, diff);
	s = integer_to_hex_string(diff);
	fail_unless(strcmp(s, "0xffffffff") == 0);
	free(s);

	integer_sub(i2, i1, diff);
	s = integer_to_hex_string(diff);
	fail_unless(strcmp(s, "-0xffffffff") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(diff);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_add_neg)
START_TEST(test_integer_add_neg)
{

	integer_t *i1, *i2, *sum;
	char *s;

	i1 = integer_new_from_hex("0xffffffffffffffff");
	i2 = integer_new_from_hex("-0xfedcba9876543210");
	sum = integer_new_zero();
	integer_add(i1, i2, sum);
	s = integer_to_hex_string(sum);
	fail_unless(strcmp(s, "0x123456789abcdef") == 0);
	free(s);

	integer_add(i2, i1, sum);
	s = integer_to_hex_string(sum);
	fail_unless(strcmp(s, "0x123456789abcdef") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(sum);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_sub_neg)
START_TEST(test_integer_sub_neg)
{

	integer_t *i1, *i2, *diff;
	char *s;

	i1 = integer_new_from_hex("0x99999999");
	i2 = integer_new_from_hex("-0x66666667");
	diff = integer_new_zero();
	integer_sub(i1, i2, diff);
	s = integer_to_hex_string(diff);
	fail_unless(strcmp(s, "0x100000000") == 0);
	free(s);

	integer_sub(i2, i1, diff);
	s = integer_to_hex_string(diff);
	fail_unless(strcmp(s, "-0x100000000") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(diff);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_mult)
START_TEST(test_integer_mult)
{

	integer_t *i1, *i2, *prod;
	char *s;

	i1 = integer_new_from_hex("0xa00b00c00d00e00f");
	i2 = integer_new_from_hex("0x12");
	prod = integer_new_zero();
	integer_mult(i1, i2, prod);
	s = integer_to_hex_string(prod);
	fail_unless(strcmp(s, "0xb40c60d80ea0fc10e") == 0);

	free(s);
	integer_mult(i2, i1, prod);
	s = integer_to_hex_string(prod);
	fail_unless(strcmp(s, "0xb40c60d80ea0fc10e") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(prod);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_mult_neg)
START_TEST(test_integer_mult_neg)
{

	integer_t *i1, *i2, *prod;
	char *s;

	i1 = integer_new_from_hex("0x5004003002001");
	i2 = integer_new_from_hex("-0xfff");
	prod = integer_new_zero();
	integer_mult(i1, i2, prod);
	s = integer_to_hex_string(prod);
	fail_unless(strcmp(s, "-0x4ffeffeffeffefff") == 0);

	free(s);
	integer_mult(i2, i1, prod);
	s = integer_to_hex_string(prod);
	fail_unless(strcmp(s, "-0x4ffeffeffeffefff") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(prod);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_div_remainder_only)
START_TEST(test_integer_div_remainder_only)
{

	integer_t *i1, *i2, *quot, *rem;
	char *s;

	i1 = integer_new_from_hex("0x1234567890");
	i2 = integer_new_from_hex("0xfedcba09876");
	quot = integer_new_zero();
	rem = integer_new_zero();
	integer_div(i1, i2, quot, rem);
	s = integer_to_hex_string(quot);
	fail_unless(strcmp(s, "0x0") == 0);
	free(s);

	s = integer_to_hex_string(rem);
	fail_unless(strcmp(s, "0x1234567890") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(quot);
	integer_free(rem);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_div_word_size)
START_TEST(test_integer_div_word_size)
{

	integer_t *i1, *i2, *quot, *rem;
	char *s;

	i1 = integer_new_from_hex("0x8f");
	i2 = integer_new_from_hex("0x10");
	quot = integer_new_zero();
	rem = integer_new_zero();
	integer_div(i1, i2, quot, rem);
	s = integer_to_hex_string(quot);
	fail_unless(strcmp(s, "0x8") == 0);
	free(s);

	s = integer_to_hex_string(rem);
	fail_unless(strcmp(s, "0xf") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(quot);
	integer_free(rem);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_div)
START_TEST(test_integer_div)
{

	integer_t *i1, *i2, *quot, *rem;
	char *s;

	i1 = integer_new_from_hex("0x70ef00");
	i2 = integer_new_from_hex("0x1001");
	quot = integer_new_zero();
	rem = integer_new_zero();
	integer_div(i1, i2, quot, rem);
	s = integer_to_hex_string(quot);
	fail_unless(strcmp(s, "0x70e") == 0);
	free(s);

	s = integer_to_hex_string(rem);
	fail_unless(strcmp(s, "0x7f2") == 0);

	free(s);
	integer_free(i1);
	integer_free(i2);
	integer_free(quot);
	integer_free(rem);

}
END_TEST // }}}
// {{{ START_TEST(test_integer_zero)
START_TEST(test_integer_zero)
{
	integer_t *i;
	i = integer_new_from_hex("0xabcdef");
	char *s;
	integer_zero(i);
	s = integer_to_hex_string(i);
	fail_unless(strcmp(s, "0x0") == 0, NULL);
	integer_free(i);
	free(s);
}
END_TEST // }}}
// {{{ START_TEST(test_integer_copy)
START_TEST(test_integer_copy)
{
	integer_t *i1, *i2;
	i1 = integer_new_from_hex("0xabcdef");
	i2 = integer_new_zero();
	char *s;
	integer_copy(i2, i1);
	s = integer_to_hex_string(i2);
	fail_unless(strcmp(s, "0xabcdef") == 0, NULL);
	integer_free(i1);
	integer_free(i2);
	free(s);
}
END_TEST // }}}

// {{{ Suite *integer_suite() {
Suite *integer_suite() {
	
	Suite *s = suite_create("Integer");

	// Core test case
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_integer_new_zero);
	tcase_add_test(tc_core, test_integer_new_from_hex);
	tcase_add_test(tc_core, test_integer_cmp);
	tcase_add_test(tc_core, test_integer_add);
	tcase_add_test(tc_core, test_integer_add_uneven_digits);
	tcase_add_test(tc_core, test_integer_add_carry);
	tcase_add_test(tc_core, test_integer_sub);
	tcase_add_test(tc_core, test_integer_sub_uneven_digits);
	tcase_add_test(tc_core, test_integer_add_neg);
	tcase_add_test(tc_core, test_integer_sub_neg);
	tcase_add_test(tc_core, test_integer_mult);
	tcase_add_test(tc_core, test_integer_mult_neg);
	tcase_add_test(tc_core, test_integer_div_remainder_only);
	//tcase_add_test(tc_core, test_integer_div_word_size);
	//tcase_add_test(tc_core, test_integer_div);
	tcase_add_test(tc_core, test_integer_zero);
	tcase_add_test(tc_core, test_integer_copy);
	suite_add_tcase(s, tc_core);
	
	return s;
} // }}}

// {{{ int main (void)
int main (void)
{
	int number_failed;
	Suite *s = integer_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
} // }}}

// vim: fdm=marker ts=4
