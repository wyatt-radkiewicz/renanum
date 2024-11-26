#include <stdio.h>

#include "ek.h"
#include "../src/rena.h"

static bool test_uint_deserialize_dec_1(unsigned id) {
	rena_uint_result_t res = rena_uint_deserialize_dec("0");
	return res.end && res.val == 0;
}

bool test_uint_deserialize_dec_speed(unsigned id);

static test_t tests[] = {
	TEST_ADD(test_uint_deserialize_dec_1)
	TEST_ADD(test_uint_deserialize_dec_speed)
};

int main(int argc, char **argv) {
	return tests_run_foreach(NULL, tests, arrlen(tests), stdout) ? 0 : -1;
}

