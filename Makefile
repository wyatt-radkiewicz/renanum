# Get build mode
CHKGOAL		=$(findstring $(1),$(MAKECMDGOALS))

# Building flags and output directory
RENA_FLAGS	:=-std=gnu89 -pedantic -Werror -Wall
EKUTIL_FLAGS	:=-DEK_USE_TEST=1 -DEK_USE_UTIL=1
CFLAGS		:=$(CFLAGS) $(EKUTIL_FLAGS)
LDFLAGS		:=$(LDFLAGS)
ifeq ($(call CHKGOAL,rel),rel)
BIN_DIR		:=bin/rel
CFLAGS		+=-O2
else
BIN_DIR		:=bin/dbg
CFLAGS		+=-g -O0
endif

# Collecting sources
SRC2OBJ		=$(patsubst %.c,$(BIN_DIR)/obj/%.o,$(1))
SRCS		:=$(sort $(shell find test -type f -name "*.c") test/ek.c)

# Targets
.PHONY: dbg rel run clean
dbg: $(BIN_DIR)/test rena_obj
rel: $(BIN_DIR)/test rena_obj
clean:
	rm -rf bin/
	rm -rf test/ek* test/.ek*

# Run debugger or just release executable?
ifeq ($(call CHKGOAL,rel),rel)
run: $(BIN_DIR)/test
	$<
else
run: $(BIN_DIR)/test
	@if which -s gdb; then gdb $<;\
	else lldb $<; fi
endif

# Build main executable
$(BIN_DIR)/test: $(call SRC2OBJ,$(SRCS))
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $^ -o $@

# Build the rena.h into an object file so that it can be tested for compilation
# this object file isn't actually used in the test executable
.PHONY: rena_obj
rena_obj: $(BIN_DIR)/src/rena.o
$(BIN_DIR)/src/rena.o: src/rena.h
	@mkdir -p $(dir $@)
	$(CC) $(RENA_FLAGS) $(CFLAGS) -c $^ -o $@

# $(1) source file
define MKFILE
$(dir $(call SRC2OBJ,$(1)))$(shell gcc -M $(CFLAGS) $(1) | tr '\\\n' '  ')
	@mkdir -p $$(dir $$@)
	$(CC) $(CFLAGS) -c $$(firstword $$^) -o $$@
endef

# Create ekutils files if needed
ifneq ($(call CHKGOAL,clean),clean)
_PHONY	:=$(shell touch test/ek.h test/ek.c)
_PHONY	:=$(shell tools/update_ekutils.sh)
endif

# Build all source file rules
$(foreach src,$(SRCS),$(eval $(call MKFILE,$(src))))

