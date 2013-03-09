# tmux-applet
# Copyright 2011-2013 Vianney le Clément de Saint-Marcq <vleclement@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with This program.  If not, see <http://www.gnu.org/licenses/>.

.PHONY: all test install clean

DESTDIR ?=
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
ETCDIR ?= $(PREFIX)/etc
SHAREDIR ?= $(PREFIX)/share/tmux-applet

CCFLAGS=-Wall
ifdef DEBUG
CCFLAGS+=-g -DDEBUG
endif

all: tmux-applet autostart.sh

tmux-applet: tmux-applet.c
	$(CC) $(CCFLAGS) -o $@ $+

autostart.sh: autostart.sh.in
	sed "s@%SHAREDIR%@$(SHAREDIR)@g" $< >$@

ifdef DEBUG
test: tmux-applet
	PATH=.:$$PATH tmux -f tmux.conf
	killall tmux
endif

install: tmux-applet autostart.sh
	install -Dm755 tmux-applet "$(DESTDIR)$(BINDIR)/tmux-applet"
	install -Dm644 tmux-applet.conf "$(DESTDIR)$(ETCDIR)/tmux-applet.conf"
	install -Dm644 tmux.conf "$(DESTDIR)$(SHAREDIR)/tmux.conf"
	install -Dm644 autostart.sh "$(DESTDIR)$(SHAREDIR)/autostart.sh"

clean:
	-rm tmux-applet
	-rm autostart.sh

# vim:set sw=4 ts=4 noet:
