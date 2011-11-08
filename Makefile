.PHONY: all clean

all: tmux-applet

tmux-applet: tmux-applet.c
	$(CC) -Wall -o $@ $+

clean:
	rm tmux-applet
