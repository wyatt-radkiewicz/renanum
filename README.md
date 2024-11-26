# Rena Num
> A single-header number parsing/serializing library in c99

## What is Rena Num?
Rena num is supposed to be a simple to use, relatively fast, portable number
parser written in c99. TODO: Documentation

## How to build
Just drop rena.h from src/ or from the releases page to your project and start
coding!
It's single header and has static functions but if you want to make it so that
rena.h only contains declarations you can go into rena.h and define
```c
#define RENA_HEADER_ONLY 1
```
to make it so that only declarations are in rena.h by default. From there you
can then include the file into main.c or some other file like this:
```c
#define RENA_IMPLEMENTATION 1
#include "rena.h"
```
to include the function definitions and build the code for the parser/writer.

## How to run tests
Run ```make dbg``` or ```make rel``` to make debug or release versions of the
tester binary. You can also add on the target ```run``` afterwards to either
run (eg. ```make dbg run```) the debug build in a debugger or the release build
after the binary was generated.

TODO LIST:
- Float parsing
- Integer parsing
- Float serializing
- Integer serializing

