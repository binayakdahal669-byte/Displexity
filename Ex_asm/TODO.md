# TODO: Complete IDE Enhancements

## Vulkan Setup
- [x] Enumerate physical devices and select suitable one
- [x] Create logical device and queues
- [x] Create command pool and descriptor pool
- [x] Create swapchain, render pass, and framebuffers
- [x] Initialize ImGui_ImplVulkan with Vulkan resources

## Render Loop Updates
- [x] Update main loop to acquire swapchain images
- [x] Record command buffers for rendering
- [x] Present using vkQueuePresentKHR

## Editor Panel Enhancements
- [x] Add multiline text input widget for .dis files
- [ ] Integrate basic syntax highlighting using syntax module
- [x] Add file loading and saving functionality

## Explorer Panel
- [x] Implement project tree using ImGui TreeNode
- [x] Display files and directories
- [x] Add file selection and opening

## Build Panel
- [x] Add buttons for build commands (compile .dis to .disp)
- [x] Integrate terminal module for command execution
- [x] Display build output in text area

## Outline Panel
- [x] Integrate outline generation for current file
- [x] Display functions and classes

## Testing and Followup
- [ ] Update build instructions in README.md if needed
- [ ] Test compilation and execution on Windows with MSYS2 UCRT64
- [ ] Verify all features work correctly
