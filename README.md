# gvimfullscreen_win32

Created by Derek McLoughlin

Forked and modified by Ian Halliday for personal customization and also because
hot damn it is fun to mess around with old Win32 code.

## What is this

Toggles gvim to a fullscreen window.  For Windows gvim.

Original credit to Yasuhiro Matsumoto for VimTweak's EnableMaximize
functionality.  Derek McLoughlin modified it to make the window borders
disappear and have the window cover the taskbar.

## Install and Usage

Put `gvimfullscreen.dll` in your `gvim.exe` folder.

Add this to your vimrc:

```vimscript
nnoremap <F11> :call libcallnr("gvimfullscreen.dll", "ToggleFullScreen", 0)<CR>
```

Replace `F11` with which ever key combination you desire.

## Building

Ensure msvc build tools are on your path and run `build.bat`.

`clean.bat` will delete all the files produced by `build.bat`.

Make sure your build architecture matches your `gvim.exe`.  That is, build x64
if your `gvim.exe` is x64, etc.
