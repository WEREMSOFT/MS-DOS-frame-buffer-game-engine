SRC_F := $(shell find src -name *.c)
SRC_O := $(patsubst %.c,%.o,$(SRC_F))

FLAGS := -g -Wall -Ilibs/include 
LIBS := -lpthread -lm -lglfw -lGLEW -lGL

TARGET := bin/main.bin

all: $(SRC_O) copy_assets
	gcc $(FLAGS) $(SRC_O) -o $(TARGET) $(LIBS)

run_main: all
	$(TARGET)

%.o: %.c
	gcc -c $(FLAGS) $^ -o $@

clean:
	rm -rf $(SRC_O)
	rm -rf $(TARGET)
	rm -rf bin/assets

copy_assets:
	cp -r assets bin