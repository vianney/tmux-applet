tmux-applet
============

This project enhances [tmux][] with some useful applets. It also contains
a custom configuration file and automatic start-up script.

The following information can be shown:

* current load,
* memory usage
* disk space usage on a specified device,
* failing raid array.

[tmux]: http://tmux.sourceforge.net/


Installation
-------------

To build tmux-applet, simply run

    make

To install system-wide, run as root:

    make install

If you want to launch tmux automatically when you log in, add the following
line at the beginning of your `.bashrc` file.

    . /usr/local/share/tmux-applet/autostart.sh


Configuration
--------------

The `tmux-applet` program will look for a `.tmux-applet.conf` file in your home
directory and fall back to the installed default configuration.
The configuration file contains the applets to be shown in order. Some applets
require arguments. An applet may be preceded by tmux attributes in square
brackets (e.g., `[fg=blue]`) to override the default applet attributes. All
elements are separated by white space. Comments starting with a hash (`#`) are
ignored up to the end of the line.

Applet   | Arguments | Default attr. | Description
-------- | --------- | ------------- | -------------------------------------
`load`   | *none*    | `fg=yellow`   | Show the current load
`memory` | *none*    | `fg=green`    | Show RAM usage and free RAM
`disk`   | `path`    | `fg=magenta`  | Show disk usage and free space of the filesystem mounted at `path`
`raid`   | *none*    | `bg=red`      | Show `RAID` if a RAID array is degraded

To make tmux show the applets, add `#(tmux-applet)` to `status-right` or
`status-left` in your `.tmux.conf` file. Alternatively, you can also use the
provided `tmux.conf`, which includes some additional customizations.


Author and license
-------------------

Copyright 2011-2013 Vianney le Clément de Saint-Marcq <vleclement@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with This program.  If not, see <http://www.gnu.org/licenses/>.
