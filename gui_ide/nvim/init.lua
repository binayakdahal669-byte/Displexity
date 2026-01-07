-- DISP IDE - Neovim Config
vim.g.mapleader = " "
vim.opt.number = true
vim.opt.relativenumber = true
vim.opt.tabstop = 4
vim.opt.shiftwidth = 4
vim.opt.expandtab = true
vim.opt.smartindent = true
vim.opt.termguicolors = true
vim.opt.signcolumn = "yes"
vim.opt.mouse = "a"
vim.opt.guicursor = "n-v-c-sm:block,i-ci-ve:ver25,r-cr-o:hor20"

-- Neovide settings (if using Neovide GUI)
if vim.g.neovide then
  vim.g.neovide_cursor_animation_length = 0.1
  vim.g.neovide_cursor_trail_size = 0.5
  vim.g.neovide_cursor_vfx_mode = "railgun"  -- wiggly/particle effect: railgun, torpedo, pixiedust, sonicboom, ripple, wireframe
  vim.g.neovide_cursor_vfx_particle_density = 10.0
  vim.g.neovide_cursor_vfx_particle_lifetime = 1.5
  vim.g.neovide_cursor_vfx_particle_speed = 10.0
  vim.opt.guifont = "Cascadia Code:h12"
end
vim.opt.clipboard = "unnamedplus"
vim.opt.splitright = true
vim.opt.splitbelow = true

-- Bootstrap lazy.nvim
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.loop.fs_stat(lazypath) then
  vim.fn.system({"git", "clone", "--filter=blob:none", "https://github.com/folke/lazy.nvim.git", "--branch=stable", lazypath})
end
vim.opt.rtp:prepend(lazypath)

require("lazy").setup({
  -- File explorer
  {
    "nvim-tree/nvim-tree.lua",
    dependencies = { "nvim-tree/nvim-web-devicons" },
    config = function()
      require("nvim-tree").setup({
        view = { width = 30, side = "right" },
        filters = { dotfiles = false },
      })
    end
  },
  
  -- Syntax highlighting
  { "nvim-treesitter/nvim-treesitter", build = ":TSUpdate" },
  
  -- Theme
  {
    "folke/tokyonight.nvim",
    priority = 1000,
    config = function() 
      require("tokyonight").setup({
        on_highlights = function(hl, c)
          hl.Cursor = { fg = "#000000", bg = "#7dcfff" }  -- Cyan cursor
          hl.iCursor = { fg = "#000000", bg = "#bb9af7" } -- Purple in insert
          hl.CursorLine = { bg = "#292e42" }
        end
      })
      vim.cmd.colorscheme("tokyonight-night") 
    end
  },
  
  -- Status line
  {
    "nvim-lualine/lualine.nvim",
    config = function()
      require("lualine").setup({ options = { theme = "tokyonight" } })
    end
  },
  
  -- Tabs
  {
    "akinsho/bufferline.nvim",
    dependencies = { "nvim-tree/nvim-web-devicons" },
    config = function()
      require("bufferline").setup({})
    end
  },
  
  -- Terminal
  {
    "akinsho/toggleterm.nvim",
    config = function()
      require("toggleterm").setup({
        size = 15,
        open_mapping = [[<C-`>]],
        direction = "horizontal",
        shell = "powershell.exe"
      })
    end
  },
  
  -- Fuzzy finder
  {
    "nvim-telescope/telescope.nvim",
    dependencies = { "nvim-lua/plenary.nvim" },
  },
  
  -- Auto pairs
  { "windwp/nvim-autopairs", config = function() require("nvim-autopairs").setup({}) end },
  
  -- Comments
  { "numToStr/Comment.nvim", config = function() require("Comment").setup() end },
  
  -- Autocomplete with floating suggestions (like Kiro)
  {
    "hrsh7th/nvim-cmp",
    dependencies = {
      "hrsh7th/cmp-buffer",
      "hrsh7th/cmp-path",
      "hrsh7th/cmp-cmdline",
      "L3MON4D3/LuaSnip",
      "saadparwaiz1/cmp_luasnip",
      "rafamadriz/friendly-snippets",
    },
    config = function()
      local cmp = require("cmp")
      local luasnip = require("luasnip")
      
      -- Load friendly-snippets
      require("luasnip.loaders.from_vscode").lazy_load()
      
      cmp.setup({
        snippet = {
          expand = function(args)
            luasnip.lsp_expand(args.body)
          end,
        },
        window = {
          completion = cmp.config.window.bordered({
            winhighlight = "Normal:Pmenu,FloatBorder:Pmenu,CursorLine:PmenuSel,Search:None",
          }),
          documentation = cmp.config.window.bordered(),
        },
        mapping = cmp.mapping.preset.insert({
          ["<C-b>"] = cmp.mapping.scroll_docs(-4),
          ["<C-f>"] = cmp.mapping.scroll_docs(4),
          ["<C-Space>"] = cmp.mapping.complete(),
          ["<C-e>"] = cmp.mapping.abort(),
          ["<CR>"] = cmp.mapping.confirm({ select = true }),
          ["<Tab>"] = cmp.mapping(function(fallback)
            if cmp.visible() then
              cmp.select_next_item()
            elseif luasnip.expand_or_jumpable() then
              luasnip.expand_or_jump()
            else
              fallback()
            end
          end, { "i", "s" }),
          ["<S-Tab>"] = cmp.mapping(function(fallback)
            if cmp.visible() then
              cmp.select_prev_item()
            elseif luasnip.jumpable(-1) then
              luasnip.jump(-1)
            else
              fallback()
            end
          end, { "i", "s" }),
        }),
        sources = cmp.config.sources({
          { name = "luasnip" },
          { name = "buffer", keyword_length = 2 },
          { name = "path" },
        }),
        formatting = {
          format = function(entry, vim_item)
            vim_item.menu = ({
              buffer = "[Buf]",
              path = "[Path]",
              luasnip = "[Snip]",
            })[entry.source.name]
            return vim_item
          end,
        },
      })
      
      -- Cmdline completion
      cmp.setup.cmdline(":", {
        mapping = cmp.mapping.preset.cmdline(),
        sources = cmp.config.sources({
          { name = "path" },
          { name = "cmdline" },
        }),
      })
      
      -- Search completion
      cmp.setup.cmdline("/", {
        mapping = cmp.mapping.preset.cmdline(),
        sources = {
          { name = "buffer" },
        },
      })
    end,
  },
})

-- DISP file type
vim.filetype.add({
  extension = {
    dis = "disp",
    disp = "disp",
    dish = "disp",
    disll = "disp",
    dislog = "disp",
    tuy = "disp",
    tui = "disp",
  }
})

-- Displexity keyword completions
local disp_keywords = {
  -- Keywords
  "func", "return", "if", "elif", "else", "while", "for", "in", "break", "continue",
  "print", "input", "include", "from", "class", "new", "this", "true", "false",
  "and", "or", "not", "is", "isnt", "null", "void",
  -- Types
  "int", "float", "str", "bool", "array", "map", "object",
  -- Built-in functions
  "len", "type", "range", "abs", "min", "max", "sqrt", "pow", "sin", "cos", "tan",
  "floor", "ceil", "round", "random", "time", "sleep",
  -- Graphics
  "window_create", "window_close", "clear", "draw_pixel", "draw_line", "draw_rect",
  "draw_circle", "draw_triangle", "set_color", "gl_init", "gl_swap",
  -- Input
  "is_key_pressed", "is_key_just_pressed", "is_key_released",
  "get_mouse_x", "get_mouse_y", "is_mouse_pressed", "get_keypress",
  -- TUI
  "gotoxy", "set_cursor", "hide_cursor", "show_cursor", "get_terminal_width", "get_terminal_height",
  -- System
  "file_read", "file_write", "file_exists", "exec", "getenv", "setenv",
  -- Network
  "http_get", "http_post", "socket_connect", "socket_send", "socket_recv",
  -- Special
  "for_this_use_icon", "load_image", "load_sound", "play_sound",
}

-- Multi-language keyword databases
local c_keywords = {
  "auto", "break", "case", "char", "const", "continue", "default", "do", "double",
  "else", "enum", "extern", "float", "for", "goto", "if", "inline", "int", "long",
  "register", "restrict", "return", "short", "signed", "sizeof", "static", "struct",
  "switch", "typedef", "union", "unsigned", "void", "volatile", "while", "_Bool",
  "_Complex", "_Imaginary", "NULL", "true", "false",
  -- Common functions
  "printf", "scanf", "malloc", "free", "memcpy", "memset", "strlen", "strcpy",
  "strcmp", "strcat", "fopen", "fclose", "fread", "fwrite", "fprintf", "fscanf",
}

local cpp_keywords = {
  -- C++ specific
  "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit",
  "atomic_noexcept", "bitand", "bitor", "bool", "catch", "char8_t", "char16_t",
  "char32_t", "class", "compl", "concept", "consteval", "constexpr", "constinit",
  "const_cast", "co_await", "co_return", "co_yield", "decltype", "delete", "dynamic_cast",
  "explicit", "export", "false", "friend", "mutable", "namespace", "new", "noexcept",
  "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected",
  "public", "reflexpr", "reinterpret_cast", "requires", "static_assert", "static_cast",
  "synchronized", "template", "this", "thread_local", "throw", "true", "try", "typeid",
  "typename", "using", "virtual", "wchar_t", "xor", "xor_eq", "override", "final",
  -- STL
  "std", "vector", "string", "map", "set", "unordered_map", "unordered_set", "list",
  "deque", "queue", "stack", "pair", "tuple", "array", "shared_ptr", "unique_ptr",
  "make_shared", "make_unique", "cout", "cin", "endl", "cerr", "ifstream", "ofstream",
}

local python_keywords = {
  "False", "None", "True", "and", "as", "assert", "async", "await", "break", "class",
  "continue", "def", "del", "elif", "else", "except", "finally", "for", "from", "global",
  "if", "import", "in", "is", "lambda", "nonlocal", "not", "or", "pass", "raise",
  "return", "try", "while", "with", "yield", "self", "cls",
  -- Built-ins
  "print", "input", "len", "range", "str", "int", "float", "bool", "list", "dict",
  "set", "tuple", "open", "file", "type", "isinstance", "hasattr", "getattr", "setattr",
  "enumerate", "zip", "map", "filter", "sorted", "reversed", "sum", "min", "max", "abs",
}

local java_keywords = {
  "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char", "class",
  "const", "continue", "default", "do", "double", "else", "enum", "extends", "final",
  "finally", "float", "for", "goto", "if", "implements", "import", "instanceof", "int",
  "interface", "long", "native", "new", "package", "private", "protected", "public",
  "return", "short", "static", "strictfp", "super", "switch", "synchronized", "this",
  "throw", "throws", "transient", "try", "void", "volatile", "while", "true", "false", "null",
  -- Common classes
  "String", "Integer", "Double", "Boolean", "Object", "System", "ArrayList", "HashMap",
  "HashSet", "List", "Map", "Set", "Exception", "Thread", "Runnable", "Override",
}

local javascript_keywords = {
  "await", "break", "case", "catch", "class", "const", "continue", "debugger", "default",
  "delete", "do", "else", "enum", "export", "extends", "false", "finally", "for",
  "function", "if", "implements", "import", "in", "instanceof", "interface", "let",
  "new", "null", "package", "private", "protected", "public", "return", "static",
  "super", "switch", "this", "throw", "true", "try", "typeof", "var", "void", "while",
  "with", "yield", "async", "of", "get", "set", "undefined", "NaN", "Infinity",
  -- Common
  "console", "log", "error", "warn", "document", "window", "Array", "Object", "String",
  "Number", "Boolean", "Date", "Math", "JSON", "Promise", "fetch", "setTimeout",
  "setInterval", "addEventListener", "querySelector", "getElementById",
}

local typescript_keywords = {
  -- TypeScript specific
  "any", "as", "asserts", "bigint", "boolean", "declare", "enum", "infer", "is",
  "keyof", "module", "namespace", "never", "number", "object", "readonly", "string",
  "symbol", "type", "undefined", "unique", "unknown", "abstract", "constructor",
  "interface", "implements", "extends", "public", "private", "protected", "override",
}

local lua_keywords = {
  "and", "break", "do", "else", "elseif", "end", "false", "for", "function", "goto",
  "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until",
  "while", "self",
  -- Common
  "print", "type", "tostring", "tonumber", "pairs", "ipairs", "next", "select",
  "setmetatable", "getmetatable", "rawget", "rawset", "require", "pcall", "xpcall",
  "error", "assert", "table", "string", "math", "io", "os", "coroutine", "debug",
}

local rust_keywords = {
  "as", "async", "await", "break", "const", "continue", "crate", "dyn", "else", "enum",
  "extern", "false", "fn", "for", "if", "impl", "in", "let", "loop", "match", "mod",
  "move", "mut", "pub", "ref", "return", "self", "Self", "static", "struct", "super",
  "trait", "true", "type", "unsafe", "use", "where", "while",
  -- Common types
  "i8", "i16", "i32", "i64", "i128", "isize", "u8", "u16", "u32", "u64", "u128", "usize",
  "f32", "f64", "bool", "char", "str", "String", "Vec", "Option", "Result", "Box",
  "Rc", "Arc", "Cell", "RefCell", "HashMap", "HashSet", "Some", "None", "Ok", "Err",
  "println", "print", "format", "panic", "unwrap", "expect", "clone", "iter", "into_iter",
}

local shell_keywords = {
  "if", "then", "else", "elif", "fi", "case", "esac", "for", "while", "until", "do",
  "done", "in", "function", "select", "time", "coproc", "return", "exit", "break",
  "continue", "local", "declare", "typeset", "readonly", "export", "unset", "shift",
  "source", "alias", "unalias", "set", "shopt", "trap", "eval", "exec", "wait",
  -- Common commands
  "echo", "printf", "read", "cd", "pwd", "ls", "cp", "mv", "rm", "mkdir", "rmdir",
  "cat", "grep", "sed", "awk", "find", "xargs", "sort", "uniq", "wc", "head", "tail",
  "chmod", "chown", "tar", "gzip", "curl", "wget", "ssh", "scp", "git", "make",
}

local batch_keywords = {
  "echo", "set", "if", "else", "for", "in", "do", "goto", "call", "exit", "pause",
  "rem", "cls", "cd", "dir", "copy", "move", "del", "mkdir", "rmdir", "type", "find",
  "findstr", "sort", "more", "start", "tasklist", "taskkill", "net", "ping", "ipconfig",
  "setlocal", "endlocal", "enabledelayedexpansion", "errorlevel", "exist", "not", "equ",
  "neq", "lss", "leq", "gtr", "geq", "defined", "cmdextversion", "nul", "con", "prn",
}

local powershell_keywords = {
  "Begin", "Break", "Catch", "Class", "Continue", "Data", "Define", "Do", "DynamicParam",
  "Else", "ElseIf", "End", "Enum", "Exit", "Filter", "Finally", "For", "ForEach", "From",
  "Function", "Hidden", "If", "In", "InlineScript", "Parallel", "Param", "Process",
  "Return", "Sequence", "Static", "Switch", "Throw", "Trap", "Try", "Until", "Using",
  "Var", "While", "Workflow",
  -- Common cmdlets
  "Get-Item", "Set-Item", "New-Item", "Remove-Item", "Get-Content", "Set-Content",
  "Get-ChildItem", "Copy-Item", "Move-Item", "Get-Process", "Stop-Process", "Start-Process",
  "Get-Service", "Start-Service", "Stop-Service", "Write-Host", "Write-Output", "Read-Host",
  "ForEach-Object", "Where-Object", "Select-Object", "Sort-Object", "Group-Object",
}

-- Custom completion source for Displexity
local disp_source = {}
disp_source.new = function()
  return setmetatable({}, { __index = disp_source })
end
disp_source.get_trigger_characters = function()
  return { "." }
end
disp_source.complete = function(self, params, callback)
  local items = {}
  for _, kw in ipairs(disp_keywords) do
    table.insert(items, {
      label = kw,
      kind = vim.lsp.protocol.CompletionItemKind.Keyword,
    })
  end
  callback({ items = items, isIncomplete = false })
end

-- Generic language completion source factory
local function create_lang_source(keywords, lang_name)
  local source = {}
  source.new = function()
    return setmetatable({}, { __index = source })
  end
  source.get_trigger_characters = function()
    return { ".", ":" }
  end
  source.complete = function(self, params, callback)
    local items = {}
    for _, kw in ipairs(keywords) do
      table.insert(items, {
        label = kw,
        kind = vim.lsp.protocol.CompletionItemKind.Keyword,
        detail = lang_name,
      })
    end
    callback({ items = items, isIncomplete = false })
  end
  return source
end

-- Create sources for each language
local c_source = create_lang_source(c_keywords, "C")
local cpp_source = create_lang_source(cpp_keywords, "C++")
local python_source = create_lang_source(python_keywords, "Python")
local java_source = create_lang_source(java_keywords, "Java")
local js_source = create_lang_source(javascript_keywords, "JavaScript")
local ts_source = create_lang_source(typescript_keywords, "TypeScript")
local lua_source = create_lang_source(lua_keywords, "Lua")
local rust_source = create_lang_source(rust_keywords, "Rust")
local shell_source = create_lang_source(shell_keywords, "Shell")
local batch_source = create_lang_source(batch_keywords, "Batch")
local ps_source = create_lang_source(powershell_keywords, "PowerShell")

-- Register Displexity completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = "disp",
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("disp", disp_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "disp" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register C/C++ completion sources
vim.api.nvim_create_autocmd("FileType", {
  pattern = { "c", "h" },
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("c_lang", c_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "c_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

vim.api.nvim_create_autocmd("FileType", {
  pattern = { "cpp", "hpp", "cc", "hh", "cxx", "hxx" },
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("cpp_lang", cpp_source.new())
    cmp.register_source("c_lang", c_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "cpp_lang" },
        { name = "c_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register Python completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = "python",
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("python_lang", python_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "python_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register Java completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = "java",
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("java_lang", java_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "java_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register JavaScript/TypeScript completion sources
vim.api.nvim_create_autocmd("FileType", {
  pattern = { "javascript", "javascriptreact" },
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("js_lang", js_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "js_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

vim.api.nvim_create_autocmd("FileType", {
  pattern = { "typescript", "typescriptreact" },
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("ts_lang", ts_source.new())
    cmp.register_source("js_lang", js_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "ts_lang" },
        { name = "js_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register Lua completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = "lua",
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("lua_lang", lua_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "lua_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register Rust completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = "rust",
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("rust_lang", rust_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "rust_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register Shell completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = { "sh", "bash", "zsh" },
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("shell_lang", shell_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "shell_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register Batch completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = { "dosbatch", "bat", "cmd" },
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("batch_lang", batch_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "batch_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Register PowerShell completion source
vim.api.nvim_create_autocmd("FileType", {
  pattern = { "ps1", "powershell" },
  callback = function()
    local cmp = require("cmp")
    cmp.register_source("ps_lang", ps_source.new())
    cmp.setup.buffer({
      sources = cmp.config.sources({
        { name = "ps_lang" },
        { name = "luasnip" },
        { name = "buffer" },
        { name = "path" },
      }),
    })
  end
})

-- Load DISP syntax highlighting
vim.api.nvim_create_autocmd({"BufRead", "BufNewFile"}, {
  pattern = {"*.dis", "*.disp", "*.dish", "*.disll"},
  callback = function()
    vim.bo.filetype = "disp"
    -- Try to load custom syntax file
    local syntax_path = vim.fn.stdpath("config") .. "/syntax/disp.vim"
    if vim.fn.filereadable(syntax_path) == 1 then
      vim.cmd("source " .. syntax_path)
    end
  end
})

-- DISP-specific settings
vim.api.nvim_create_autocmd("FileType", {
  pattern = "disp",
  callback = function()
    vim.bo.commentstring = "// %s"
    vim.bo.tabstop = 4
    vim.bo.shiftwidth = 4
    vim.bo.expandtab = true
  end
})

-- Keymaps
local map = vim.keymap.set

-- File explorer
map("n", "<leader>e", ":NvimTreeToggle<CR>", { desc = "Toggle Explorer" })
map("n", "<leader>o", ":NvimTreeFocus<CR>", { desc = "Focus Explorer" })

-- Tabs/Buffers
map("n", "<Tab>", ":BufferLineCycleNext<CR>", { desc = "Next Tab" })
map("n", "<S-Tab>", ":BufferLineCyclePrev<CR>", { desc = "Prev Tab" })
map("n", "<leader>x", ":bd<CR>", { desc = "Close Tab" })

-- Terminal
map("n", "<C-`>", ":ToggleTerm<CR>", { desc = "Toggle Terminal" })
map("t", "<Esc>", [[<C-\><C-n>]], { desc = "Exit Terminal Mode" })

-- File operations
map("n", "<C-s>", ":w<CR>", { desc = "Save" })
map("n", "<C-n>", ":enew<CR>", { desc = "New File" })
map("n", "<C-o>", ":Telescope find_files<CR>", { desc = "Open File" })
map("n", "<C-p>", ":Telescope find_files<CR>", { desc = "Find Files" })
map("n", "<C-f>", ":Telescope live_grep<CR>", { desc = "Search in Files" })

-- Window navigation
map("n", "<C-h>", "<C-w>h", { desc = "Move Left" })
map("n", "<C-j>", "<C-w>j", { desc = "Move Down" })
map("n", "<C-k>", "<C-w>k", { desc = "Move Up" })
map("n", "<C-l>", "<C-w>l", { desc = "Move Right" })

-- Build DISP
map("n", "<F5>", ":w<CR>:TermExec cmd='displexitycompiler %'<CR>", { desc = "Build" })

-- Quit
map("n", "<leader>q", ":qa<CR>", { desc = "Quit All" })
