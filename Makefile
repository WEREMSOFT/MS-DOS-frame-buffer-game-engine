SRC_F := $(shell find src -name *.c) $(shell find libs -name *.c)
SRC_CPP := $(shell find libs/soloud/src -name *.cpp)
OBJ_FOR_CLEAN_F := $(shell find . -name *.o)
SRC_O := $(patsubst %.c,%.o,$(SRC_F))
SRC_CPP_O := $(patsubst %.cpp,%.o,$(SRC_CPP))

LIBS := -lpthread -lm -lglfw -lGLEW -lGL -lstdc++ -ldl -lasound 
FLAGS := -g -O0 -Wall -Ilibs/include -Ilibs/soloud/include
FLAGS_RELEASE := -O2 -Wall -Ilibs/include -Ilibs/soloud/include


TARGET := bin/main.bin
.PONY: clean

all: $(SRC_O) $(SRC_CPP_O) copy_assets
	gcc $(FLAGS) $(SRC_O) $(SRC_CPP_O) -o $(TARGET) $(LIBS)

run_main: all
	$(TARGET)

run_main_release: all
	$(TARGET)

%.o: %.c
	gcc -c $(FLAGS) $^ -o $@

%.o: %.cpp
	gcc -c $(FLAGS) -lstdc++ -ldl -lasound -DWITH_ALSA=1 $^ -o $@

clean:
	rm -rf $(OBJ_FOR_CLEAN_F)
	rm -rf $(TARGET)
	rm -rf bin/assets

deep_clean: clean
	rm -rf $(SRC_CPP_O)
	

copy_assets:
	cp -r assets bin