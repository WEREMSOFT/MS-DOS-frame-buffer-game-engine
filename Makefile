SRC_F := $(shell find src -name *.c) $(shell find libs -name *.c)
SRC_CPP := $(shell find libs/soloud/src -name *.cpp)
OBJ_FOR_CLEAN_F := $(shell find ./src -name *.o)
SRC_O := $(patsubst %.c,%.o,$(SRC_F))
SRC_CPP_O := $(patsubst %.cpp,%.o,$(SRC_CPP))

LIBS := -lpthread -lm -lglfw -lGL -lstdc++ -ldl -lasound 

FLAGS_DEBUG := -g -O0 -w
FLAGS__DEBUG := -O3
FLAGS := -Wall -Wextra -Ilibs/include -Ilibs/soloud/include

# Vars for emscripten build
EMSC_CFLAGS := -O2 -s -Wall -D_DEFAULT_SOURCE -DWITH_MINIAUDIO=1 -s MIN_WEBGL_VERSION=2 -Wno-missing-braces -s OFFSCREEN_FRAMEBUFFER=1 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -s USE_GLFW=3 -sFULL_ES3 -s TOTAL_MEMORY=67108864 --preload-file assets -v -D OS_WEB
EMSC_CC := emcc

TARGET := bin/main.bin
.PONY: clean web statistics

all: clean $(SRC_O) $(SRC_CPP_O) copy_assets
	gcc $(FLAGS_DEBUG) $(FLAGS) $(SRC_O) $(SRC_CPP_O) -o $(TARGET) $(LIBS)

run_main: all
	$(TARGET)

%.o: %.c
	gcc -c $(FLAGS_DEBUG) $(FLAGS) $^ -o $@

%.o: %.cpp
	gcc -c $(FLAGS_RELEASE) $(FLAGS) -lstdc++ -ldl -lasound -DWITH_ALSA=1 $^ -o $@

web:
	rm -rf docs
	mkdir docs
	emcc -O2 -Ilibs/include -Ilibs/soloud/include -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -DWITH_MINIAUDIO=1 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 --preload-file assets -s MIN_WEBGL_VERSION=2 -gsource-map --source-map-base https://weremsoft.github.io/MS-DOS-frame-buffer-game-engine/ $(SRC_F) $(SRC_CPP) -o docs/index.html

clean:
	rm -rf $(OBJ_FOR_CLEAN_F)
	rm -rf $(TARGET)
	rm -rf bin/assets
	rm -rf html/**/*.*

deep_clean: clean
	rm -rf $(SRC_CPP_O)
	

copy_assets:
	cp -r assets bin

statistics_old:
	echo >> metrics.txt
	git log -1 --format="%H - %s" >> metrics.txt
	pmccabe -vt src/main.c >> metrics.txt

statistics:
	echo >> metrics.txt
	git log -1 --format="%H - %s" >> metrics.txt
	complexity --histogram --score --thresh=3 `ls src/*.c` >> metrics.txt

create_report:
	sed -e '/Total/,/-/!d' metrics.txt 
