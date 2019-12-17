CC = gcc
C11 = -std=gnu99 -lpthread  -ljson
 
# path assignment
WORK_DIR = ./
INC_PATH = $(join $(WORK_DIR), include)
SRC_PATH = $(join $(WORK_DIR), libsrc)
OBJ_PATH = $(join $(WORK_DIR), obj)
BIN_PATH = $(join $(WORK_DIR), bin)

# src/*.c
SRC_FILE_WITH_DIR = $(wildcard $(join $(SRC_PATH), /*.c))
# *.c
SRC_FILE = $(notdir $(SRC_FILE_WITH_DIR))
# *.o
OBJ_FILE = $(SRC_FILE:%.c=%.o)
# obj/*.o
OBJ_FILE_WITH_DIR = $(patsubst %.o, $(OBJ_PATH)/%.o, $(OBJ_FILE))

TARGET = $(join $(BIN_PATH), /usbhotplug)

OBJ_FILE_WITH_DIR += main.o

$(OBJ_PATH)/%.o : $(SRC_PATH)/%.c
	$(CC) $(C11) -I$(INC_PATH) -c $< -o $@

$(TARGET) : $(OBJ_FILE_WITH_DIR)
	$(CC) $(C11) -o $@ $^

.PHONY: clean
clean:
	@rm $(OBJ_FILE_WITH_DIR) 
	@rm $(TARGET)
