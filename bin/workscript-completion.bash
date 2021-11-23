#!/usr/bin/env bash

complete -f -W \
	"push pop update mostrecent buildall newdb azlogin reap indexify forktree" \
	workscript
