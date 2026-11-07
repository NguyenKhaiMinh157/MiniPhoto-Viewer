### Supported OS: Windows 10 and Windows 11 (64-bit)

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

### Preview
v1.0.0, v1.0.1
<img width="984" height="693" alt="Preview" src="https://github.com/user-attachments/assets/2fe53111-1a5e-4c68-b824-5b942c066d95" />
<img width="981" height="690" alt="1 0 1 preveww" src="https://github.com/user-attachments/assets/69d7f5c5-0387-4823-a45f-8181a10ba25e" />
v1.0.2 and later
<img width="983" height="690" alt="Capture" src="https://github.com/user-attachments/assets/66a06eb4-7e09-4c85-940a-7b0c6bfc0ca2" />
<img width="982" height="689" alt="1 0 2" src="https://github.com/user-attachments/assets/13e50a75-29ac-4e54-b930-f0fe7d5ac307" />


