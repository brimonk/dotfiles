# Brian Chrzanowski
# 2020-10-07 12:16:52
#
# Bash Aliases

# ls aliases - color support if available
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
    alias ls='ls --color=auto'
    #alias dir='dir --color=auto'
    #alias vdir='vdir --color=auto'

    #alias grep='grep --color=auto'
    #alias fgrep='fgrep --color=auto'
    #alias egrep='egrep --color=auto'
fi

alias ll='ls -l'
alias la='ls -A'
alias l='ls -CF'
alias sl='ls'

# when you get mad at the computer, and just want it to do the thing
alias fuck='sudo "$BASH" -c "$(history -p !!)"'

# set tmux colors
alias tmux='tmux -2'

# make gdb shut up
alias gdb='gdb -q'

# shutdown the nc network socket after EOF
alias nc="nc -N"

