# gb

Nintendo Game Boy emulator written in C

## Usage

Launch emulator using:
```
main.exe PATH_TO_GAME
```
(if not path is given, runs `test.gb` from emulator directory)

### Controls

| Game Boy Control | System Input |
| --- | --- |
| Up | W |
| Left | A |
| Down | S |
| Right | D |
| Start | Enter |
| Select | Backspace |
| A | . |
| B | , |

| Key | Function |
| - | - |
| F | Fast-Forward (disables wait between each frame) |
| L | Enable per-frame debug output log |
| M | Print memory to output log |
| N | Run until next frame |
| P | Pause |
| V | Toggle VRAM Display Window |
| Right Shift | Enable per-cycle output log |


## Features

- Emulates Nintendo Game Boy games
- Output debug information per-frame or per-cycle
- VRAM viewer window
- Pause
- Frame Stepping
- Fast-Forward

## Building

### Set up MSYS2

1. Download and install [MSYS2](https://github.com/msys2/msys2-installer/releases)
2. Launch `MINGW64`
3. Run `pacman -Syuu` to update `MINGW64`

### Install Pre-Requisites 

After updating `MINGW64`, run the following command to install packages needed:

```
pacman -S make mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL mingw-w64-x86_64-make
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

## Supported Games

- Dr. Mario
- [Flappy Bird](https://laroldsjubilantjunkyard.itch.io/flappy-bird-gameboy)
- Tetris