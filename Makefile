BUILD_DIR=build
SRC := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRC))


all: wm


$(BUILD_DIR)/%.o: src/%.c
	gcc -Wall -c -o $@ $<

wm: $(OBJS)
	gcc -o $@ -lX11  $(OBJS)


clean:
	rm -f $(BUILD_DIR)/*


run: all
	./wm
