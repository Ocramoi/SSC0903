##
# Trabalho 1 - SSC0903
#
# @file
# @version 0.1

CC=gcc
SRC_FILES=$(wildcard **/*.c)
SRC_FILES:=$(filter-out $(wildcard **/flycheck_*.c), $(SRC_FILES))
EXEC_PARALLEL=./build/parallel
DEXEC_PARALLEL=./build/dparallel
EXEC_SEQ=./build/seq
DEXEC_SEQ=./build/dseq
COMPILE_FLAGS=-Wall -Wextra -Ofast -std=gnu17 -fopenmp
DEBUG_FLAGS=-Wall -Wextra -Og -g3 -DDEBUG -std=gnu17 -fopenmp
VALGRIND_FLAGS=--leak-check=full --show-leak-kinds=all --track-origins=yes -s
LD_FLAGS:=-lm

parallel: parallel.o lib
	$(CC) $(COMPILE_FLAGS) parallel.o utils.o -o $(EXEC_PARALLEL) $(LD_FLAGS)

sequential: sequential.o lib
	$(CC) $(COMPILE_FLAGS) sequential.o utils.o -o $(EXEC_SEQ) $(LD_FLAGS)

parallel.o: parallel.c
	$(CC) $(COMPILE_FLAGS) parallel.c -c

sequential.o: sequential.c
	$(CC) $(COMPILE_FLAGS) sequential.c -c

lib: lib/utils.c lib/utils.h
	$(CC) $(COMPILE_FLAGS) $(SRC_FILES) -c

setup:
	rm -rf build
	mkdir build

run_parallel:
	$(EXEC_PARALLEL)

run_sequential:
	$(EXEC_SEQ)

debug_parallel:
	$(CC) $(DEBUG_FLAGS) parallel.c $(filter-out $(wildcard **/flycheck_*.c), $(wildcard **/*.c)) -o $(DEXEC_PARALLEL) $(LD_FLAGS)

debug_sequential:
	$(CC) $(DEBUG_FLAGS) sequential.c $(filter-out $(wildcard **/flycheck_*.c), $(wildcard **/*.c)) -o $(DEXEC_SEQ) $(LD_FLAGS)

val_parallel:
	valgrind $(VALGRIND_FLAGS) ./$(DEXEC_PARALLEL)

val_sequential:
	valgrind $(VALGRIND_FLAGS) ./$(DEXEC_SEQ)

clear:
	rm *.o

.PHONY: clear val debug sequential parallel utils lib

# end
