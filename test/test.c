#include <stdio.h>

#include "ek.h"

static test_t tests[] = {
	TEST_PAD
};

int main(int argc, char **argv) {
	return tests_run_foreach(NULL, tests, arrlen(tests), stdout) ? 0 : -1;
}

