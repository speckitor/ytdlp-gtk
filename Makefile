DEBUG ?= -g
CC = gcc
CFLAGS = -Wall -Wextra -pedantic \
	-O3 -march=native -ffast-math \
	$(shell pkg-config --cflags gtk4 glib-2.0 libadwaita-1) \
	$(DEBUG)
LDFLAGS = $(shell pkg-config --libs gtk4 glib-2.0 libadwaita-1)

BINARY=ytdlp-gtk
SOURCE=./src/*.c

PREFIX ?= /usr/local
BIN_DIR = $(PREFIX)/bin
DESKTOP_DIR = $(PREFIX)/share/applications
ICON_DIR = $(PREFIX)/share/icons/hicolor/scalable/apps

$(BINARY): gresources
	$(CC) $(SOURCE) -o $(BINARY) $(CFLAGS) $(LDFLAGS)

gresources:
	glib-compile-resources src/ytdlpgtk.gresource.xml \
		--sourcedir=data/ui \
		--generate-source

install: $(BINARY)
	install -Dm755 $(BINARY) $(BIN_DIR)
	install -Dm644 data/org.speckitor.NekoDownloader.desktop $(DESKTOP_DIR)
	install -Dm644 data/org.speckitor.NekoDownloader.svg $(ICON_DIR)

uninstall:
	rm -f $(BIN_DIR)/$(BINARY)
	rm -f $(DESKTOP_DIR)/org.speckitor.NekoDownloader.desktop
	rm -f $(ICON_DIR)/org.speckitor.NekoDownloader.svg

clean:
	rm -f $(BINARY)
	rm -f ./src/ytdlpgtk.c
