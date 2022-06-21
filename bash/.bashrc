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
HISTCONTROL=ignoreboth:erasedups
shopt -s histappend
HISTSIZE=-1
HISTFILESIZE=-1

# export PROMPT_COMMAND="history -a; history -c; history -r; $PROMPT_COMMAND"

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

# prompt
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

# EXTRA STUFF
# I have a feeling there should really be a better place for this, but do I really
# need a whole other files for these? Certainly not.

# multest: just a simple math test (adds, subs, muls, divs, and mods in base 10 and 16)
function mtest
{
	DIGITS="4"
	# [[ "$#" < 1 ]] && BASE=16 || BASE="$1"
	BASE=16

	OPS[0]="+"

	OPS_LEN=${#OPS[@]}

	while true ; do
		NUM1=$(($RANDOM % $(($BASE))))
		NUM2=0

		# the second value cannot be zero - division
		while [[ $NUM2 -eq 0 || $NUM2 -gt $NUM1 ]]; do
			NUM2=$(($RANDOM % $(($BASE * $BASE))))
		done

		OPER=${OPS[$(($RANDOM % $OPS_LEN))]}
		EXPR="$NUM1 $OPER $NUM2"
		ANS="$(printf '0x%02x' $(($EXPR)))"

		PROMPT="$(printf '0x%02x %s 0x%02x = ' $NUM1 $OPER $NUM2)"

		read -r -p "$PROMPT" UANS
		[[ $? -ne 0 ]] && return 0

		[[ $UANS -ne $ANS ]] && echo "Wrong. $(printf '0x%02x\n' $ANS)"
	done
}

export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
[ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"  # This loads nvm bash_completion

function fzcd() {
	cd $(find . -type d | fzf)
}

function fzvim() {
	FILE=$(find . -type f | fzf)
	if [ ! -z $FILE ]; then
		vim $FILE
	fi
}

if [ $(which fzf) ]; then
	export -f fzcd
	export -f fzvim
fi

# minutes: used to track meeting minutes
function minutes()
{
	if [ "$#" -ne 1 ]; then
		echo "USAGE: minutes <meetingname>"
		return
	fi

	NAME=$1

	while read line; do
		echo $NAME [$(date +"%Y%m%dT%H:%M:%S")] $line >> "$HOME/.minutes"
	done
}

