#!/usr/bin/env bash

complete -f -W \
	"push pop update mostrecent buildall mkcontainer newdb azlogin reap indexify forktree" \
	workscript
