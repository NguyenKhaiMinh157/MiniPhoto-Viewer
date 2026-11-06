Supported OS: Windows 10 and Windows 11 (64-bit)

# MiniPhoto Viewer

A lightweight Windows image viewer written in C++ using the Win32 API and GDI+.

MiniPhoto Viewer is a simple desktop application designed to open and browse images without the complexity of a full photo-editing suite.

## Features

- Open images using `Ctrl + O`
- Browse images in the same folder
- Navigate between images using the Left and Right arrow keys
- Zoom in and zoom out
- Fit images to the window
- Simple Windows desktop interface
- Custom application icon
- Supports JPG, JPEG, PNG, BMP and GIF images

**Note:** Animated GIF files are displayed as static images.


## Supported formats

| Format | Support |
|---|---|
| JPG / JPEG | Yes |
| PNG | Yes |
| BMP | Yes |
| GIF | Yes (static display) |

## Requirements

- Windows 10 or later
- 64-bit Windows
- Embarcadero Dev-C++ 6.2 or a compatible MinGW-w64 GCC toolchain
- GDI+ (included with Windows)

## Build from source

### 1. Get the source code

Clone this repository or download it as a ZIP file.

### 2. Compile the icon resource

Make sure `windres.exe` is available in your PATH, then open Command Prompt in the project folder and run:

```cmd
windres resource.rc -O coff -o resource.res
```

This creates `resource.res` from `miniphoto.ico`.

### 3. Configure the compiler

Open `MiniPhoto Viewer.cpp` in Embarcadero Dev-C++.

Set the compiler standard to:

```text
-std=c++11
```

Add the following options to the Linker settings:

```text
-static-libgcc -lgdiplus -lcomdlg32 -lgdi32 -luser32 -lkernel32 -lshell32 -mwindows
```

Also add `resource.res` to the linker inputs.

### 4. Compile

Build the application in Dev-C++.

If compilation succeeds, the generated executable will contain the custom application icon.

## Project structure

```text
MiniPhoto-Viewer/
├── MiniPhoto Viewer.cpp
├── miniphoto.ico
├── resource.rc
└── README.md
```

`resource.res` is generated during the build process and does not need to be committed.

## Keyboard shortcuts

| Key | Action |
|---|---|
| Ctrl + O | Open an image |
| Left Arrow | Previous image |
| Right Arrow | Next image |

## About

MiniPhoto Viewer is a personal C++ learning project focused on Windows desktop programming, Win32 API and GDI+.

---

**Project:** MiniPhoto Viewer  
**Language:** C++  
**Platform:** Windows  
**Graphics:** GDI+
