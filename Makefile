CC = gcc
CFLAGS = -Wall -O2 -s -flto -ansi -shared -fPIC
TARGET = libxwallpaper.so

.PHONY: all clean install

all: $(TARGET) xwallpaper.rs

$(TARGET): xwallpaper.c xwallpaper.h
	$(CC) $< $(CFLAGS) -o $@

xwallpaper.rs: xwallpaper.h
	bindgen $< -o $@

clean:
	rm -f $(TARGET) xwallpaper.rs

install:
	cp $(TARGET) /usr/lib/