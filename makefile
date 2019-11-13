CC = gcc
CFLAGS = -W -Wall -std=c17 -pedantic
LDFLAGS = -ltest

OUT_DIR = build

TEST_SRC = ./test/test_serializer.c
TEST_OBJ = $(addprefix $(OUT_DIR)/,$(TEST_SRC:.c=.o))

TEST_EXEC = test_exec

all : init_test test

init_test :
	@$(MAKE) static -C libtest > /dev/null

test : $(TEST_OBJ)
	@$(CC) -o $(TEST_EXEC) $^ $(LDFLAGS)
	@./$(TEST_EXEC)

$(OUT_DIR)/%.o : %.c
	@mkdir -p $(shell dirname $@)
	@$(CC) -o $@ -c $< $(CDFLAGS)

clean_test : 
	@rm -rf $(OUT_DIR)
	@rm $(TEST_EXEC)

clean: clean_test
	@$(MAKE) clean -C libtest > /dev/null	