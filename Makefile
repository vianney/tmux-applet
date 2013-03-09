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
	killall tmux
endif

clean:
	rm tmux-applet

# vim:set sw=4 ts=4 noet:
