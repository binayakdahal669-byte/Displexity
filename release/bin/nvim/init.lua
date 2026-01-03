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
  vim.g.neovide_cursor_animation_length = 0.2
  vim.g.neovide_cursor_trail_size = 100000.5
  vim.g.neovide_cursor_vfx_mode = ""  -- wiggly/particle effect: railgun, torpedo, pixiedust, sonicboom, ripple, wireframe
  vim.g.neovide_cursor_vfx_particle_density = 10.0
  vim.g.neovide_cursor_vfx_particle_lifetime = 4.5
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
})

-- DISP file type
vim.filetype.add({
  extension = {
    dis = "disp",
    disp = "disp",
    dish = "disp",
    disll = "disp",
    dislog = "disp",
  }
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
