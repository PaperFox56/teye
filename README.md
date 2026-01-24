# termwidgetsuite

My attempt at creating a terminal based widget suite — a small collection of C source files for drawing monochrome bitmap-style output in a terminal and a few example programs (clock, etc.). The project provides a minimal framebuffer-like API (teye) and utilities to render it using ANSI colors.

## Contents

- src/teye.c, src/teye.h — core framebuffer and terminal render code (TEYE_* API)
- src/timer.c, src/timer.h — small cross-platform timing helpers
- src/clock.c — example program that draws a 7-segment-style clock into the TEYE buffer and renders it
- build.sh — small build helper script that compiles a named example together with the library
- bin/ — output directory for compiled binaries (not present in repository; created at build time)

## Features

- Minimal framebuffer API for terminal rendering (TEYE_init, TEYE_render_frame_mode_1/2, TEYE_clear_buffer, TEYE_free)
- Two rendering modes (character-based color blocks and foreground/background blended characters)
- Example: a terminal 7-segment clock (src/clock.c)

## Requirements

- C compiler (gcc/clang)
- POSIX-compatible terminal (Linux, macOS). Windows may work but is untested.

## Build and run

1. Create the bin directory (if it does not exist):

   mkdir -p bin

2. Option A — use the provided helper script (make it executable first):

   chmod +x build.sh
   ./build.sh clock r

   This should compile `src/clock.c` together with `src/teye.c` and `src/timer.c` into `bin/teye-clock` and run it.

   Note: the helper script takes the example name as the first argument (e.g. `clock`) and an optional second argument `r` to run the produced binary. If you see an error from the script, use the manual compile command below.

3. Option B — compile manually with gcc:

   gcc src/clock.c src/teye.c src/timer.c -o bin/teye-clock
   ./bin/teye-clock

Replace `clock` with the name of another example source file (if added later) — the helper script expects `src/<name>.c` to exist and will link it with `src/teye.c` and `src/timer.c`.

## Using the TEYE API (example)\nHere is a minimal example showing how the library is used (based on the existing code):

```c
#include "teye.h"

int main(void) {
    TEYE_Buffer buf = TEYE_init(160, 60); // create a buffer

    // draw directly into buf.frame_buffer[y][x] using small integers as color indices
    // (see src/teye.c for color table and rendering behavior)

    TEYE_render_frame_mode_2(); // render to terminal

    TEYE_free();
    return 0;
}
```
