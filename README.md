# gb

Nintendo Game Boy / Game Boy Color emulator written in C

## Usage

Launch emulator, and select Game Boy ROM through file picker.

Alternatively, launch emulator using:
```
main.exe [options] [game]
```

### Arguments

| Option | Function |
| - | - |
| `-f` | Start in full-screen |
| `-v` | Show VRAM viewer |

### BIOS

Emulator supports using a Nintendo Game Boy / Game Boy Color boot ROM in the executable directory. 

- To use a Game Boy Color bios, include `cgb_boot.bin` in the executable directory.
- To use an orignal Game Boy bios, include `dmg_boot.bin` in the executable directory.
    - if `dmg_boot.bin` is used the emulator will act as an original Game Boy, and as such Game Boy Color games will refuse to boot.
    - `cgb_boot.bin` will be prioritised over `dmg_boot.bin`.

### Controls

| Game Boy Control | System Input | Alternate Input |
| --- | --- | --- |
| Up D-Pad | Up Arrow Key | W |
| Left D-Pad | Left Arrow Key | A |
| Down D-Pad | Down Arrow Key | S |
| Right D-Pad | Right Arrow Key | D |
| Start | Enter |
| Select | Backspace |
| A | Z | . |
| B | X | , |

| Hotkey | Modifier | Function |
| - | - | - |
| **Esc** | | Show Hotkeys |
| *File Management* |
| **O** | Ctrl | Open new ROM |
| **R** | Ctrl | Restart Game |
| **S** | Ctrl | Save Game |
| **S** | Ctrl + Shift | Save State |
| **L** | Ctrl + Shift | Load State |
| *Video* |
| **F11** | | Toggle Fullscreen |
| **Enter** | Alt | Toggle Fullscreen |
| **F** | Ctrl | Toggle Framerate (FPS) viewer |
| **1-9** | Ctrl | Change Window Size |
| **Alt** | | Show available color palettes |
| **0-9** | Alt | Select color palette |
| **V** | Ctrl | Toggle VRAM Display Window |
| *Speed* |
| **P** | Ctrl | Pause |
| **+** | Ctrl | Increase Speed 10% |
| **-** | Ctrl | Decrease Speed 10% |
| **+** | Ctrl + Shift | Reset Speed to 100% |
| **F** | Ctrl | Fast-Forward (disables wait between each frame) |
| **N** | Ctrl | Run until next frame |
| **N** | Ctrl + Shift | Run until next instruction |
| *Debug Logging*|
| **L** | Ctrl | Enable per-frame debug output log |
| **L** | Ctrl + Shift| Enable per-cycle debug output log |
| **M** | Ctrl | Print memory to output log |

#### Palettess

| Hotkey | Palette |
| - | - |
| 1 | Monochrome Grey |
| 2 | Game specific palette (if available) |
| 3 | DMG Game Boy Green |
| 4 | Monochrome Pink |
| 5 | Yellow and Red (Tetris) |
| 6 | Blue and Green (Alleyway) |
| 7 | Yellow and Pale Blue (Mario Land 2) |
| 8 | Grey, Cyan and Orange (Wario Land) |
| 9 | Blue and Red (Dr. Mario) |
| 0 | Red and Green (Pokemon Red) |

## Features

- Emulates Nintendo Game Boy games
- Emulates Nintendo Game Boy Color games
- Controller Support
- Game saves
- Pause
- Colorise Nintendo Game Boy games with GBC palettes
- Output debug information per-frame or per-instruction
- Frame Stepping
- Instruction Stepping
- Fast-Forward
- VRAM viewer window

<br>

# Building

## Windows

### Set up MSYS2

1. Download and install [MSYS2](https://github.com/msys2/msys2-installer/releases)
2. Launch `MINGW64`
3. Run `pacman -Syuu` to update `MINGW64`

### Install Pre-Requisites 

After updating `MINGW64`, run the following command to install packages needed:

```
pacman -S git make mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL mingw-w64-x86_64-make
```

### Download & Compile

1. Clone repository

```
git clone https://github.com/currall/gb.git
cd gb
```
2. Compile
```
make
```

## Linux
*Building steps for Ubuntu Linux*

### Setup

1. Update system package repositories
```
sudo apt update
```

### Install Pre-Requisites

1. Install GCC compiler and make:
```
sudo apt install build-essential
```

2. Install SDL2:
```
sudo apt-get install libsdl2-dev
```

### Download & Compile

1. Clone repository

```
git clone https://github.com/currall/gb.git
cd gb
```
2. Compile
```
make
```