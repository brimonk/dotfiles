# Brian Chrzanowski
# 2020-10-07 12:02:20
#
# Executed when we logout

# PRIVACY
# - clear the screen when we logout
if [ "$SHLVL" = 1 ]; then
    [ -x /usr/bin/clear_console ] && /usr/bin/clear_console -q
fi

