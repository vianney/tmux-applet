unset TMUX_OPTS
[ -f "~/.tmux.conf" ] || TMUX_OPTS="-f /usr/share/tmux-applet/tmux.conf"
[ -z "$TMUX" ] && exec tmux ${TMUX_OPTS} attach
