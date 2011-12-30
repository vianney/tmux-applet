.PHONY: all clean

CCFLAGS=-Wall
ifdef DEBUG
CCFLAGS+=-g -DDEBUG
endif

all: tmux-applet

tmux-applet: tmux-applet.c
	$(CC) $(CCFLAGS) -o $@ $+

clean:
	rm tmux-applet
