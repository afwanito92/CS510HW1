PROJNAME = sbp
VIEWER = less
reverse = $(if $(wordlist 2,2,$(1)),$(call reverse,$(wordlist 2,$(words $(1)),$(1))) $(firstword $(1)),$(1))

IDIR = include
ODIR = build
SRCDIR = src

BUILD_ENV_BASE = $(ODIR)
BUILD_ENV =  $(BUILD_ENV_BASE)
BUILD_ENV += $(patsubst %,$(BUILD_ENV_BASE)/%,sk_list)
BUILD_ENV += $(patsubst %,$(BUILD_ENV_BASE)/%,sk_types)
BUILD_ENV += $(patsubst %,$(BUILD_ENV_BASE)/%,sk_random)
BUILD_ENV += $(patsubst %,$(BUILD_ENV_BASE)/%,sk_str)
BUILD_ENV += $(patsubst %,$(BUILD_ENV_BASE)/%,sk_tree)
BUILD_ENV += $(patsubst %,$(BUILD_ENV_BASE)/%,sk_map)
BUILD_ENV += $(patsubst %,$(BUILD_ENV_BASE)/%,sk_hash_table)

DEPF =  utils.h
DEPF += printer.h
DEPF += sk_clargs.h
DEPF += sk_iterator/sk_iterator.h
DEPF += sk_iterator/sk_const_iterator.h
DEPF += sk_list/sk_list.h
DEPF += sk_tree/sk_tree.h
DEPF += sk_map/sk_tree_map.h
DEPF += sk_hash_table/sk_hash_table.h
DEPF += sk_types/sk_types.h
DEPF += sk_types/sk_types_concat.h
DEPF += sk_types/sk_types_parse.h
DEPF += sk_random/sk_random.h
DEPF += sk_random/sk_random_default.h
DEPF += sk_str/c_str.h
DEPF += sk_str/sk_str.h

DEPF += sk_iterator_utils.h
DEPS = $(patsubst %,$(IDIR)/%,$(DEPF))

OBJF =  main.o 
OBJF += printer.o 
OBJF += sk_clargs.o
OBJF += sk_list/sk_list.o 
OBJF += sk_tree/sk_tree.o
OBJF += sk_map/sk_tree_map.o
OBJF += sk_hash_table/sk_hash_table.o
OBJF += sk_types/sk_types_concat.o
OBJF += sk_types/sk_types_parse.o
OBJF += sk_str/c_str.o
OBJF += sk_str/sk_str.o
OBJF += sk_random/sk_random_default.o

OBJF += sk_iterator_utils.o
OBJ = $(patsubst %,$(ODIR)/%,$(OBJF))

DRIVER = $(SRCDIR)/main.c

CC = gcc
CCOPTS = -g -Wall -Wextra -lm -Wno-unused-variable -Wno-missing-field-initializers -I$(IDIR)

.PHONY: all view build env clean rebuild preprocessor
.DEFAULT: build

build : | env all

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	-@ $(CC) -c -o $@ $< $(CCOPTS)

all : $(OBJ)
	- $(CC) -o $(PROJNAME) $^ $(CCOPTS)


env: 
	-@ mkdir -p $(BUILD_ENV)

clean :
	-@ \rm -f $(OBJ) $(SRCDIR)/*~ $(T_SRCDIR)/*~ core $(INCDIR)/*~ $(T_INCDIR)/*~ $(PROJNAME)
	-@ \rmdir $(call reverse, $(BUILD_ENV)) 

rebuild : | clean build

view :
	-@ $(VIEWER) $(DRIVER) $(DEPS)

run :
	-@ ./$(PROJNAME)

preprocessor :
	-@ $(CC) -c $(DRIVER) -E $(CCOPTS)


