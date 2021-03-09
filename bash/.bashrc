# Brian Chrzanowski
# 2020-10-07 12:02:20
#
# My Bash RC

# If not running interactively, don't do anything
case $- in
    *i*) ;;
      *) return;;
esac

# HISTORY CONTROLS
# - ignore leading spaces & duplicates
# - append to the history file, don't overwrite it
# - store all history (might have to truncate it manually later)
HISTCONTROL=ignoreboth
shopt -s histappend
HISTSIZE=-1
HISTFILESIZE=-1

# PATTERN CONTROLS
# - set pathname expansion with "**"
shopt -s globstar

# PAGER CONTROLS (less)
# - set `less`
# - make less more friendly for binary files
export PAGER="less"
export LESS="-S -i"
[ -x /usr/bin/lesspipe ] && eval "$(SHELL=/bin/sh lesspipe)"

# TERMINAL CONTROLS
# - check window size after every command and update LINES and COLUMNS
shopt -s checkwinsize

# - set a fancy prompt (non-color, unless we know we "want" color)
case "$TERM" in
    xterm-color) color_prompt=yes;;
esac

# - set prompt
export PS1="\u@\h : \W \\$ "

# - enable completion, if available
if ! shopt -oq posix; then
  if [ -f /usr/share/bash-completion/bash_completion ]; then
    . /usr/share/bash-completion/bash_completion
  elif [ -f /etc/bash_completion ]; then
    . /etc/bash_completion
  fi
fi

# EDITOR CONTROLS
export VISUAL=vim
export EDITOR="$VISUAL"

# GCC SETTINGS
# - colored warnings / errors
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'

# MISC ALIASES
# - all of the aliases are in a file called `.bash_aliases`
if [ -f ~/.bash_aliases ]; then
    . ~/.bash_aliases
fi

# MACHINE SPECIFIC SETTINGS
# - settings for the current machine we're working on
if [ -f ~/.bash_machine ]; then
	. ~/.bash_machine
fi

# ADDITIONAL FUNCTIONS

# upload : uses files.chrzanowski.me to store either $1 or stdin
function upload
{
	[ $# -ge 1 -a -f "$1" ] && input="$1" || input="-"
	cat $input | curl --data-binary @- https://files.chrzanowski.me/upload
}


