#include <stdio.h>

#include "ek.h"
#include "../src/rena.h"

static bool test_read_uint_dec_1(unsigned id) {
	rena_uint_t res = rena_read_uint_dec("0");
	return res.end && res.val == 0ull;
}
static bool test_read_uint_dec_2(unsigned id) {
	rena_uint_t res = rena_read_uint_dec("123");
	return res.end && res.val == 123ull;
}
static bool test_read_uint_dec_3(unsigned id) {
	rena_uint_t res = rena_read_uint_dec_ws("1234567890123456");
	return res.end && res.val == 1234567890123456ull;
}

bool test_read_uint_dec_speed(unsigned id);

static test_t tests[] = {
	TEST_ADD(test_read_uint_dec_1)
	TEST_ADD(test_read_uint_dec_2)
	TEST_ADD(test_read_uint_dec_3)
	TEST_ADD(test_read_uint_dec_speed)
};

int main(int argc, char **argv) {
	return tests_run_foreach(NULL, tests, arrlen(tests), stdout) ? 0 : -1;
}

