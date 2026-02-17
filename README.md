# gb

Nintendo Game Boy emulator written in C

## Usage

Launch emulator using:
```
main.exe PATH_TO_GAME
```
(if not path is given, runs `test.gb` from emulator directory)

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

| Hotkey | Function |
| - | - |
| *Game Management* |
| **O** | Open new ROM |
| **R** | Restart Game |
| *Debug Logging*|
| **L** | Enable per-frame debug output log |
| **M** | Print memory to output log |
| **Right Shift** | Enable per-cycle output log |
| *Speed* |
| **F** | Fast-Forward (disables wait between each frame) |
| **N** | Run until next frame |
| **Shift + N** | Run until next instruction |
| **P** | Pause |
| *Video* |
| **V** | Toggle VRAM Display Window |
| **0-9** | Select color palette |



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

## Supported Cartridges

- No MBC (MBC0)
- MBC1

### Compatibility List

- [Flappy Bird](https://laroldsjubilantjunkyard.itch.io/flappy-bird-gameboy)

| Code | Compatibility |
| - | - |
| 🟩 | No known issues |
| 🟦 | Minor issues but gameplay is correct |
| 🟥 | Gameplay does not start |

| | Game Title | Issues |
| - | - | - |
| 🟩 | Dr. Mario | No issues |
| 🟩 | Super Mario Land 2 | No issues |
| 🟦 | Super Mario Land | Freezes on game over |
| 🟦 | Tetris | Freezes on pause screen |
| 🟥 | Alleyway | Skips title screen, doesn't recognise controls in gameplay |
| 🟥 | Legend of Zelda: Link's Awakening | Breaks after leaving first area |
| 🟥 | Pokemon Red | Loads copyright screen, doesn't load to title screen |
| 🟥 | Wario Land | Crashes emulator after selecting save file |
| 🟥 | Wario Land 2 | Doesn't load |