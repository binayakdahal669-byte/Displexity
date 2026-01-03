-- Add this to your Neovim config (~/.config/nvim/init.lua or init.vim)

-- Enable code folding
vim.opt.foldmethod = "expr"
vim.opt.foldexpr = "nvim_treesitter#foldexpr()"  -- If using treesitter
-- OR for indent-based folding:
-- vim.opt.foldmethod = "indent"

-- Don't fold everything by default
vim.opt.foldlevel = 99
vim.opt.foldlevelstart = 99

-- Show fold column (the visual indicator on the left)
vim.opt.foldcolumn = "1"

-- Custom fold text (optional - makes it look cleaner)
vim.opt.fillchars = {
  fold = " ",
  foldopen = "",
  foldclose = "",
  foldsep = " ",
}

-- Keymaps for folding
vim.keymap.set("n", "zR", require("ufo").openAllFolds, { desc = "Open all folds" })
vim.keymap.set("n", "zM", require("ufo").closeAllFolds, { desc = "Close all folds" })

-- If you want the exact look from the image, install nvim-ufo plugin:
-- Add to your plugin manager (lazy.nvim example):
--[[
{
  "kevinhwang91/nvim-ufo",
  dependencies = "kevinhwang91/promise-async",
  config = function()
    require("ufo").setup({
      provider_selector = function()
        return { "treesitter", "indent" }
      end
    })
  end
}
]]