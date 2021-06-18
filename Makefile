SRC_F := $(shell find src -name *.c)
OBJ_FOR_CLEAN_F := $(shell find src -name *.o)
SRC_O := $(patsubst %.c,%.o,$(SRC_F))

LIBS := -lpthread -lm -lglfw -lGLEW -lGL
FLAGS := -g -O0 -Wall -Ilibs/include

TARGET := bin/main.bin
.PONY: clean

all: $(SRC_O) copy_assets
	gcc $(FLAGS) $(SRC_O) -o $(TARGET) $(LIBS)

run_main: all
	$(TARGET)

run_main_release: all
	$(TARGET)

%.o: %.c
	gcc -c $(FLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_FOR_CLEAN_F)
	rm -rf $(TARGET)
	rm -rf bin/assets

copy_assets:
	cp -r assets bin