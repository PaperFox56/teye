# Teye

I have recently been working on many small project where I needed to draw bitmap art in the terminal. I quickly realized that most of the fonctionality I implemented in those projects could be incorporated in a single library that could be maintained easily.
Here it is. I hope this can be useful to someone else. Feel free to use the code in your own project and if you encounter any issues, either open an issue in the corresponding session or change the code to your heart content and  open a PR so that everyone can benefit.

## Licence
See LICENCE for more information.

## Contents

- src/teye.c, include/teye.h - core framebuffer and terminal render code (TEYE_* API)

## Features

- Minimal framebuffer API for terminal rendering (TEYE_init, TEYE_render_frame_mode_1/2, TEYE_clear_buffer, TEYE_free)
- Two rendering modes (character-based color blocks and foreground/background blended characters)

## Requirements

- C compiler (gcc/clang)
- POSIX-compatible terminal (Linux, macOS). Windows may work but is untested.

## Build and run

Use Make

   ```make
      sudo make install
   ```

will build the library and install it in /usr/local/ along with the headers
with any of your projects.



## Using the TEYE API (example)
Here is a minimal example showing how the library is used (based on the existing code):

```c
#include <teye/teye.h>

int main(void) {
    TEYE_Buffer buf = TEYE_init(160, 60); // create a buffer

    // draw directly into buf.frame_buffer[y][x] using small integers as color indices
    // (see src/teye.c for color table and rendering behavior)

    TEYE_render_frame_mode_2(); // render to terminal

    TEYE_free();
    return 0;
}
```
