# gvimfullscreen_win32

Created by Derek McLoughlin

Forked and modified by Ian Halliday for personal customization and also because
hot damn it is fun to mess around with old Win32 code.

## What is this

Toggles gvim to a fullscreen window.  For Windows gvim.

Original credit to Yasuhiro Matsumoto for VimTweak's EnableMaximize
functionality.  Derek McLoughlin modified it to make the window borders
disappear and have the window cover the taskbar.

I have modified the code to simplify the logic and restore the original
position and maximized/restored state of the window.  I opted to accomplish
this via storing the state in a string in a vimscript global and passing it
to and from the ToggleFullScreen call into gvimfullscreen.dll.

## Install and Usage

Put `gvimfullscreen.dll` in your `gvim.exe` folder.

Add this to your vimrc:

```vimscript
let g:gvimfullscreen_state = ""
function! <SID>ToggleFullScreen()
    let g:gvimfullscreen_state = libcall("gvimfullscreen.dll", "ToggleFullScreen", g:gvimfullscreen_state)
endfunction
nnoremap <F11> :call <SID>ToggleFullScreen()<CR>
```

Replace `F11` with which ever key combination you desire.

## Building

Ensure msvc build tools are on your path and run `build.bat`.

`clean.bat` will delete all the files produced by `build.bat`.

Make sure your build architecture matches your `gvim.exe`.  That is, build x64
if your `gvim.exe` is x64, etc.
