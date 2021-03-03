" Brian Chrzanowski
" Vim RC
" TODO clean - this file's a mess

syntax on

" --
" Section: General
" --
set history=500
filetype plugin on
filetype indent on

set autoread

" space and comma to be leader (muscle memory is a bitch)
let mapleader = ","
let g:mapleader = ","
nmap <Space> ,

" Fast saving
nmap <leader>w :w!<cr>

set timeoutlen=5000
" :W sudo saves the file 
" (useful for handling the permission-denied error)
" command W w !sudo tee % > /dev/null

" --
" Section: VIM UI
" --
set wildmenu
set wildignore=*.o
set ruler " show current position
set cmdheight=1
set hid " buffers are hidden (better default)

" Configure backspace so it acts as it should act
set backspace=eol,start,indent
set whichwrap+=<,>,h,l

set mouse= " no mice
set ignorecase
set hlsearch
set incsearch 
set lazyredraw 
set number

set magic " for regex
set showmatch 
" How many tenths of a second to blink when matching brackets
set mat=1

" No annoying sound on errors
set noerrorbells
set visualbell t_vb=
autocmd GUIEnter * set visualbell t_vb=

" Disable highlight when <leader><cr> is pressed
map <silent> <leader><cr> :noh<cr>

set laststatus=2 " Always show the status line

" Format the status line
set statusline=%<%f%h%m%r%=%{&ff}\ %l\ %c\ %P

set textwidth=0

" --
" Section: Colors and Fonts
" --

" Enable syntax highlighting
syntax enable 
syntax on

if has("win32") " handle the GUI font
	set guifont=Courier_New:h14
else
	set guifont=FreeMono\ 12
end


colorscheme desert

" create an indicator at 80 chars
set colorcolumn=80,100
highlight ColorColumn ctermbg=DarkGray
if has("win32")
	highlight ColorColumn ctermbg=DarkGray guibg=#3d3d3d
end

" Set utf8 as standard encoding
set encoding=utf8

" Use Unix as the standard file type, then dos, then mac
set ffs=unix,dos,mac

set formatoptions=tcqnj

" no need for backups, things are in source control
set nobackup
set nowb
set noswapfile
" but we do want to undo forever
set undofile
set undodir=~/.vim/undodir

" Be smart when using tabs ;)
set smarttab

" 1 tab == 4 'characters'
set shiftwidth=4
set tabstop=4
set softtabstop=4
set noexpandtab

set nowrap " DO NOT WRAP LINES

" --
" Section: FileType Handling
" --

" Bash Settings

" Better C Syntax Handling
au BufEnter,BufRead,BufNewFile *.[ch] set cindent
au BufEnter,BufRead,BufNewFile *.[ch] set fo+=ro
au BufEnter,BufRead,BufNewFile *.[ch] set cinoptions=j1,J1,(1s,:0,l1

" If we're working in C#, Typescript, HTML, or CSS, set the company
" tab settings so we don't make anyone angry in a PR
au BufEnter *.cs set expandtab fo+=ro cindent
au BufEnter *.html set expandtab fo+=ro cindent
au BufEnter *.css set expandtab fo+=ro cindent
au BufEnter *.ts set expandtab fo+=ro cindent
au BufEnter *.js set expandtab fo+=ro cindent

au BufEnter *.sql set expandtab fo+=ro cindent

" Make sure we can do those C things in glsl things...
au BufEnter *.vert set cindent
au BufEnter *.vert set fo+=ro
au BufEnter *.tesc set cindent
au BufEnter *.tesc set fo+=ro
au BufEnter *.tese set cindent
au BufEnter *.tese set fo+=ro
au BufEnter *.geom set cindent
au BufEnter *.geom set fo+=ro
au BufEnter *.frag set cindent
au BufEnter *.frag set fo+=ro
au BufEnter *.comp set cindent
au BufEnter *.comp set fo+=ro

" Better asm Handling
au BufEnter *.asm set syntax=nasm
au BufEnter *.asm set fo+=ro
au BufEnter *.asm set cindent

" Better Markdown Behavior
autocmd BufRead,BufNewFile *.markdown,*.mdown,*.mkd,*.mkdn,*.md  setf markdown

" work - indentation
autocmd BufRead,BufNewFile *.css  setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.html setlocal shiftwidth=4 softtabstop=4 expandtab

" work - cs
autocmd BufRead,BufNewFile *.cs setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.cs setlocal cindent
autocmd BufRead,BufNewFile *.cs setlocal fo+=ro
autocmd BufRead,BufNewFile *.cs setlocal cinoptions=j1,J1,(1s

" SQL
autocmd BufRead,BufNewFile *.sql setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.sql setlocal cindent
autocmd BufRead,BufNewFile *.sql setlocal fo+=ro
autocmd BufRead,BufNewFile *.sql setlocal cinoptions+=j1,J1,(1s

" work - ts
autocmd BufRead,BufNewFile *.ts setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.ts setlocal cindent
autocmd BufRead,BufNewFile *.ts setlocal fo+=ro
autocmd BufRead,BufNewFile *.ts setlocal cinoptions=j1,J1,(1s

" work - js
autocmd BufRead,BufNewFile *.js setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.js setlocal cindent
autocmd BufRead,BufNewFile *.js setlocal fo+=ro
autocmd BufRead,BufNewFile *.ts setlocal cinoptions=j1,J1,(1s

" Honestly, webdev isn't all that bad...
autocmd BufRead,BufNewFile *.ejs  set filetype=html

" freaking text files
autocmd BufRead,BufNewFile *.txt set fo-=ro

" TOML File Syntax Highlighting
augroup filetypedetect 
  au! BufRead,BufNewFile *.toml set filetype=toml 
augroup END 

" --
" Section: Navigation
" --

" Treat long lines as break lines (useful when moving around in them)
map j gj
map k gk

" Smart way to move between windows
map <C-j> <C-W>j
map <C-k> <C-W>k
map <C-h> <C-W>h
map <C-l> <C-W>l

" navigating buffers
map <leader>e   :bnext<cr>
map <leader>q   :bprevious<cr>

" mapping for vimdiff, getting remote and local changes
map <leader>r  :diffget REMOTE<cr>
map <leader>l  :diffget LOCAL<cr>
map <leader>b  :diffget BASE<cr>

" Specify the behavior when switching between buffers 
try
  set switchbuf=useopen,usetab,newtab
  set stal=2
catch
endtry


" --
" Section: Mappings
" --
" Remap VIM 0 to first non-blank character
map 0 ^

" I fatfinger F1 all the time on my laptop
map <F1> <nop>

" Insert date/time with <leader>d
nnoremap <leader>d "=strftime("%Y-%m-%d %H:%M:%S")<CR>p
" Insert date with <leader>D
nnoremap <leader>D "=strftime("%Y-%m-%d")<CR>p

" Section: Spelling
" Pressing ,ss will toggle and untoggle spell checking
nmap <leader>ss :setlocal spell!<cr>
nmap <leader>ns :set nospell<cr>

" Toggle paste mode on and off
nmap <leader>pp :setlocal paste!<cr>

" binds for splitting and joining
nmap <leader>s :split!<cr>
nmap <leader>v :vsplit!<cr>
nmap <leader>o :only!<cr>

" binds to auto-reload my vimrc (useful when reprogramming some vim junk)
" nmap <leader>r :source $MYVIMRC<cr>

" set some gui options
set guioptions=m " remove menu bar
set guioptions-=T " remove toolbar
set guioptions-=r " remove right-hand scroll-bar
set guioptions-=L " remove right-hand scroll-bar

