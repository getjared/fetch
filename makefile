CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lX11
SRC = fetch.c
EXEC = fetch
PREFIX = /usr/local

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

install: $(EXEC)
	install -D $(EXEC) $(DESTDIR)$(PREFIX)/bin/$(EXEC)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXEC)

clean:
	rm -f $(EXEC)

.PHONY: all install uninstall clean