.PHONY: all test clean

CCFLAGS=-Wall
ifdef DEBUG
CCFLAGS+=-g -DDEBUG
endif

all: tmux-applet

tmux-applet: tmux-applet.c
	$(CC) $(CCFLAGS) -o $@ $+

ifdef DEBUG
test: tmux-applet
	PATH=.:$$PATH tmux -f tmux.conf
endif

clean:
	rm tmux-applet
