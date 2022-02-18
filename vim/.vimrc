" Brian Chrzanowski
" Vim RC
" TODO clean - this file's a mess

" Plugin Section
call plug#begin("~/.vim/plugged")

" check if we're on the work machine
let hostname = substitute(system('hostname'), '\n', '', '')
if hostname == "brian-kla"
	" install all of the c# typescript plugins we'd ever want

	" the main omnisharp plugin
	Plug 'OmniSharp/omnisharp-vim'

	let g:OmniSharp_selector_ui = 'fzf'
	let g:OmniSharp_selector_findusages = 'fzf'

	let g:OmniSharp_popup_mappings = {
		\ 'sigNext': '<C-n>',
		\ 'sigPrev': '<C-p>',
		\ 'lineDown': ['<C-e>', 'j'],
		\ 'lineUp': ['<C-y>', 'k']
	\}

	let g:OmniSharp_popup_mappings.close = '<Esc>'

	" Mappings, code-actions available flag and statusline integration
	Plug 'nickspoons/vim-sharpenup'

	" Linting/error highlighting
	Plug 'dense-analysis/ale'
	let g:ale_enabled = 0

	" Vim FZF integration, used as OmniSharp selector
	Plug 'junegunn/fzf'
	Plug 'junegunn/fzf.vim'

	" Autocompletion
	Plug 'prabirshrestha/asyncomplete.vim'
endif

call plug#end()

com! UUID exe "normal! i".system('uuidgen | tr "[:upper:]" "[:lower:]" | tr -d "\n"')

" set nofixendofline

" --
" Section: General
" --
set history=500

set autoread

" space and comma to be leader (muscle memory is a bitch)
let mapleader = ","
let g:mapleader = ","
nmap <Space> ,

" Fast saving
nmap <leader>w :w!<cr>

" Fast uploading
nmap <leader>U :w !upload<cr>

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

set textwidth=100

" --
" Section: Colors and Fonts
" --

if has("win32") " handle the GUI font
	set guifont=Courier_New:h14
else
	set guifont=FreeMono\ 12
end

set t_Co=256

set background=dark
colorscheme default
" iceberg looks better in gvim, default is just fine for the term
" colorscheme iceberg
hi Visual term=reverse ctermbg=DarkGray

" create an indicator at 80 chars
" set colorcolumn=80,100
set colorcolumn=100
highlight ColorColumn ctermbg=DarkGray
if has("win32")
	highlight ColorColumn ctermbg=DarkGray guibg=#3d3d3d
end

" Set utf8 as standard encoding
set encoding=utf8

" Use Unix as the standard file type, then dos, then mac
set ffs=unix,dos,mac
set ff=unix

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

" Markdown
au BufEnter,BufRead,BufNewFile *.md set nocindent
au BufEnter,BufRead,BufNewFile *.md set syntax=markdown
au BufEnter,BufRead,BufNewFile *.md set textwidth=100

" Better C Syntax Handling
au BufEnter,BufRead,BufNewFile *.[ch] set syntax=on
au BufEnter,BufRead,BufNewFile *.[ch] set cindent
au BufEnter,BufRead,BufNewFile *.[ch] set fo+=ro
au BufEnter,BufRead,BufNewFile *.[ch] set cinoptions=j1,J1,(1s,l1

" notes should be markdown oriented, without the extension
au BufEnter,BufRead,BufNewFile $HOME/.notes set syntax=markdown
au BufEnter,BufRead,BufNewFile $HOME/.notes set autoindent

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
autocmd BufRead,BufNewFile *.markdown,*.mdown,*.mkd,*.mkdn,*.md setf markdown

" work - indentation
autocmd BufRead,BufNewFile *.css  setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.html setlocal shiftwidth=4 softtabstop=4 expandtab

" work - cs
autocmd BufRead,BufNewFile *.cs setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.cs setlocal cindent
autocmd BufRead,BufNewFile *.cs setlocal fo+=ro
autocmd BufRead,BufNewFile *.cs setlocal cinoptions=j1,J1,(1s
" autocmd BufRead,BufNewFile *.cs nmap <F3> :echo "Hello"<CR>
nmap <F3> :OmniSharpFixUsings<CR>

" SQL
autocmd BufRead,BufNewFile *.sql setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.sql setlocal cindent
autocmd BufRead,BufNewFile *.sql setlocal fo+=ro
autocmd BufRead,BufNewFile *.sql setlocal cinoptions+=j1,J1,(1s

" KLA Config Bits
autocmd BufRead,BufNewFile *.ts setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.ts setlocal cindent
autocmd BufRead,BufNewFile *.ts setlocal fo+=ro
autocmd BufRead,BufNewFile *.ts setlocal cinoptions=j1,J1,(1s

autocmd BufRead,BufNewFile *.js setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.js setlocal cindent
autocmd BufRead,BufNewFile *.js setlocal fo+=ro
autocmd BufRead,BufNewFile *.js setlocal cinoptions=j1,J1,(1s

autocmd BufRead,BufNewFile *.json setlocal shiftwidth=4 softtabstop=4 expandtab
autocmd BufRead,BufNewFile *.json setlocal cindent
autocmd BufRead,BufNewFile *.json setlocal fo+=ro
autocmd BufRead,BufNewFile *.json setlocal cinoptions=j1,J1,(1s

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

