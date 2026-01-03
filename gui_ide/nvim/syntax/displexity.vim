" Vim syntax file for Displexity
" Language: Displexity
" Maintainer: Displexity Team

if exists("b:current_syntax")
  finish
endif

" Keywords
syn keyword dispKeyword func return if else while for in to include print input
syn keyword dispKeyword true false null break continue
syn keyword dispKeyword window_create gl_init clear render set_color fill_rect draw_rect
syn keyword dispKeyword draw_line draw_circle fill_circle draw_pixel set_projection_2d
syn keyword dispKeyword is_key_pressed is_mouse_pressed get_mouse_x get_mouse_y
syn keyword dispKeyword http_get ai_query keypress hotkey

" Types
syn keyword dispType int float str bool void var

" Operators
syn match dispOperator "[+\-*/%=<>!&|^~]"
syn match dispOperator "<<"
syn match dispOperator ">>"
syn match dispOperator "=="
syn match dispOperator "!="
syn match dispOperator "<="
syn match dispOperator ">="
syn match dispOperator "&&"
syn match dispOperator "||"

" Numbers
syn match dispNumber "\<\d\+\>"
syn match dispNumber "\<\d\+\.\d*\>"
syn match dispNumber "\<0x[0-9a-fA-F]\+\>"
syn match dispNumber "\<0b[01]\+\>"

" Strings
syn region dispString start='"' end='"' contains=dispEscape
syn match dispEscape "\\[nrt\\\"']" contained

" Comments
syn match dispComment "//.*$"
syn region dispComment start="/\*" end="\*/"

" Functions
syn match dispFunction "\<\w\+\s*(" contains=dispFunctionName
syn match dispFunctionName "\<\w\+" contained

" Highlighting
hi def link dispKeyword Keyword
hi def link dispType Type
hi def link dispOperator Operator
hi def link dispNumber Number
hi def link dispString String
hi def link dispEscape Special
hi def link dispComment Comment
hi def link dispFunction Function
hi def link dispFunctionName Function

let b:current_syntax = "displexity"