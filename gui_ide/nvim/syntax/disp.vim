" Vim syntax file for Displexity (.dis, .disp, .dish, .disll)
" Language: DISP
" Maintainer: Displexity Team

if exists("b:current_syntax")
  finish
endif

" Keywords
syn keyword dispKeyword say show print ask get input read readline when if otherwise else loop while repeat for def func function give return import use include
syn keyword dispKeyword web http key keypress get_keypress hotkey register_hotkey ai ai_query
syn keyword dispKeyword gfx graphics cls clear draw pixel draw_pixel line draw_line rect draw_rect color set_color render flip swap window win window_create
syn keyword dispKeyword opengl gl_init init_gl 2d set_projection_2d 3d set_projection_3d triangle draw_triangle quad draw_quad box fill_rect circle draw_circle ball fill_circle
syn keyword dispKeyword move translate turn rotate size scale push push_matrix pop pop_matrix closed should_close window_should_close bg_color set_clear_color
syn keyword dispKeyword Array

" Types
syn keyword dispType num number int text str string decimal float real flag bool boolean void auto var any

" Format specifiers
syn keyword dispFormat bin hex oct dec binary octal decimal
syn match dispFormat "\<0[bxod]\>"

" Boolean
syn keyword dispBoolean yes true no false

" Numbers
syn match dispNumber "\<\d\+\>"
syn match dispNumber "\<\d\+\.\d*\>"
syn match dispFloat "\<\d\+\.\d\+\>"

" Strings
syn region dispString start='"' end='"' skip='\\"' contains=dispEscape
syn region dispString start="'" end="'" skip="\\'" contains=dispEscape
syn match dispEscape "\\[nrt\\\"']" contained

" Comments
syn match dispComment "//.*$"
syn region dispComment start="/\*" end="\*/"

" Operators
syn match dispOperator "[+\-*/%=<>!&|]"
syn match dispOperator "=="
syn match dispOperator "!="
syn match dispOperator "<="
syn match dispOperator ">="
syn match dispOperator "&&"
syn match dispOperator "||"
syn match dispOperator "->"
syn match dispOperator "<<"
syn match dispOperator ">>"

" Brackets
syn match dispBracket "[\[\]{}()]"

" Semicolon
syn match dispDelimiter ";"
syn match dispDelimiter ","

" Function calls
syn match dispFunction "\<\w\+\s*(" contains=dispBracket

" Highlighting
hi def link dispKeyword Keyword
hi def link dispType Type
hi def link dispFormat Special
hi def link dispBoolean Boolean
hi def link dispNumber Number
hi def link dispFloat Float
hi def link dispString String
hi def link dispEscape Special
hi def link dispComment Comment
hi def link dispOperator Operator
hi def link dispBracket Delimiter
hi def link dispDelimiter Delimiter
hi def link dispFunction Function

let b:current_syntax = "disp"
