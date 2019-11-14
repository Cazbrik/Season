CC = gcc
CFLAGS = -W -Wall -std=c17 -pedantic
LDFLAGS = -ltest

OUT_DIR = build

TEST_SRC = ./test/test_serializer.c
TEST_OBJ = $(addprefix $(OUT_DIR)/,$(TEST_SRC:.c=.o))
TEST_EXEC = test_exec

SRC_DIR = src
SRC = $(shell find $(SRC_DIR) -name '*.c' -print)
OBJ = $(addprefix $(OUT_DIR)/,$(SRC:.c=.o))
LIB = libSeason

MV_DIR = /usr/local/lib/

static : $(OBJ)
	@ar -r $(LIB).a $^ > /dev/null
	@ranlib $(LIB).a > /dev/null
	@sudo mv $(LIB).a $(MV_DIR)

dynamic : $(OBJ)
	@$(CC) -shared -o $(LIB).so $^
	@sudo mv $(LIB).so $(MV_DIR)

test : $(TEST_OBJ)
	@$(MAKE) static -C libtest > /dev/null
	@$(CC) -o $(TEST_EXEC) $^ $(LDFLAGS)
	@./$(TEST_EXEC)
	@rm $(TEST_EXEC)
	@$(MAKE) clean -C libtest > /dev/null

$(OUT_DIR)/%.o : %.c
	@mkdir -p $(shell dirname $@)
	@$(CC) -o $@ -fpic -c $< $(CDFLAGS)

clean:
	@rm -rf $(OUT_DIR)
	@sudo rm $(MV_DIR)$(LIB).* > /dev/null