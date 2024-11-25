# Get build mode
ISREL		:=$(findstring rel,$(MAKECMDGOALS))

# Building flags and output directory
CFLAGS		?=
LDFLAGS		?=
ifeq ($(ISREL),rel)
BIN_DIR		:=bin/rel
CFLAGS		+=-O2
else
BIN_DIR		:=bin/dbg
CFLAGS		+=-g -O0
endif

# Collecting sources
SRC2OBJ		=$(patsubst %.c,$(BIN_DIR)/obj/%.o,$(1))
SRCS		:=$(shell find test -type f -name "*.c")

# Targets
.PHONY: dbg rel run clean
dbg: $(BIN_DIR)/test
rel: $(BIN_DIR)/test
clean:
	rm -rf bin/

# Run debugger or just release executable?
ifeq ($(ISREL),rel)
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

# $(1) source file
define MKFILE
$(dir $(call SRC2OBJ,$(1)))$(shell gcc -M $(CFLAGS) $(1) | tr '\\\n' '  ')
	@mkdir -p $$(dir $$@)
	$(CC) $(CFLAGS) -c $$(firstword $$^) -o $$@
endef

# Build all source file rules
$(foreach src,$(SRCS),$(eval $(call MKFILE,$(src))))

