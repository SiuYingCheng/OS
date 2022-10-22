
GIT_HOOKS := .git/hooks/applied
CC := gcc
JSON_C_DIR=/usr/local
CFLAGS += -std=gnu99 -g -Wall -I$(JSON_C_DIR)/include/json-c
LDFLAGS+= -L$(JSON_C_DIR)/lib -ljson-c
all: $(GIT_HOOKS) 

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

simulator:simulator.o os2021_thread_api.o function_libary.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o simulator simulator.o os2021_thread_api.o function_libary.o -ljson-c

simulator.o:simulator.c os2021_thread_api.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c simulator.c

os2021_thread_api.o:os2021_thread_api.c os2021_thread_api.h function_libary.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c os2021_thread_api.c

function_libary.o: function_libary.c function_libary.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c function_libary.c

.PHONY: clean
clean:
	rm *.o
