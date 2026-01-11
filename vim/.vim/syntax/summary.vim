" Vim syntax file
" Language:	git commit file
" Maintainer:	Brian Chrzanowski
" Filenames:	$HOME/.summary
" Last Change:	2025-07-01 18:15:35

if exists("b:current_syntax")
  finish
endif

" Highlight comment lines (starting with #)
syntax match summaryComment /^#.*/ contains=summaryTodo
highlight def link summaryComment Comment

let b:current_syntax = "summary"

if exists("b:current_syntax")
  finish
endif

syn case match
syn sync minlines=50

if has("spell")
  syn spell toplevel
endif
